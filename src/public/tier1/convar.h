#pragma once
#include <EASTL/string.h>

#include "utils/interfaces/interfaces.h"
#include "tier0/assert.h"
#include "cvar/icvar.h"

//-----------------------------------------------------------------------------
// Macroses for implement a console command
//
// Usage example:
// CON_COMMAND( mycommand, "My console command", FCVAR_CHEAT )
// {
//		Msg( "It's work!" );
// }
//-----------------------------------------------------------------------------
#define CON_COMMAND( Name, Description, Flags )                                 \
	static void	   ConCmdExec_##Name( uint32 argc, const char** argv );         \
	static CConCmd s_cmd##Name( #Name, ConCmdExec_##Name, Description, Flags ); \
	static void	   ConCmdExec_##Name( uint32 argc, const char** argv )

#define CON_COMMAND_EXTERN( Name, FuncName, Description, Flags )       \
	void		   FuncName( uint32 argc, const char** argv );         \
	static CConCmd s_cmd##Name( #Name, FuncName, Description, Flags ); \
	void		   FuncName( uint32 argc, const char** argv )

#define CON_COMMAND_METHOD( Name, MethodName, Description, Flags ) \
	static CConCmd s_cmd##Name( #Name, MethodName, Description, Flags );

//-----------------------------------------------------------------------------
// Type execute function of a console command
//-----------------------------------------------------------------------------
typedef void ( *conCmdExecFn_t )( uint32 argc, const char** argv );

//-----------------------------------------------------------------------------
// Any executable that wants to use IConVars/IConCmds need to implement one of these to hook up access to console variables
//-----------------------------------------------------------------------------
class ICvarAccessor
{
public:
	virtual void RegisterCommand( IConCmdBase* pCommand ) = 0;
};

//-----------------------------------------------------------------------------
// Register console invoked command/variable in a local module
//-----------------------------------------------------------------------------
class CCvarLocalRegister
{
	friend void ConVar_Register( uint32 flags, IConVarsOverrider* pConVarsOverrider, ICvarAccessor* pAccessor );
	friend void ConVar_Unregister();

protected:
	static bool				   s_bCVarsRegistered;	// Is IConVars and IConCmds registered in the engine's system
	static cvarDLLIdentifier_t s_dllIdentifier;
	static IConCmdBase*		   s_pConCmdList;  // Statically constructed list of IConCmdBases, used for registering them with the ICvar interface
	static ICvarAccessor*	   s_pAccessor;	   // IConVars and IConCmds in this executable use this 'global' to access values
};

//-----------------------------------------------------------------------------
// The base console invoked command/variable
//-----------------------------------------------------------------------------
template<class TBaseClass>
class TConCmdBase : public TBaseClass, public CCvarLocalRegister
{
public:
	// NOTE: pName and pHelpTest must be static strings
	TConCmdBase( const char* pName, const char* pHelpText = "", uint32 flags = FCVAR_NONE )
		: bRegistered( false )
		, flags( flags )
		, pName( pName )
		, pHelpText( pHelpText )
		, pNext( NULL )
	{
		Assert( pName );

		// Register in the local list
		pNext							  = CCvarLocalRegister::s_pConCmdList;
		CCvarLocalRegister::s_pConCmdList = this;

		// If s_pAccessor is already set, register it
		if ( s_pAccessor )
		{
			s_pAccessor->RegisterCommand( this );
		}
	}
	~TConCmdBase()
	{
		if ( g_pCvar )
		{
			g_pCvar->UnregisterCommand( this );
		}
	}

	// IConCmdBase interface
	// Functions to check/add flags (cvarFlags_t)
	virtual bool   IsFlagSet( uint32 flag ) const override;
	virtual uint32 GetFlags() const override;
	void		   AddFlags( uint32 flags ) override;

	virtual bool				IsCommand() const override;
	void						SetHelpText( const char* pHelpText ) override;
	const char*					GetName() const override;
	virtual const char*			GetHelpText() const override;
	virtual cvarDLLIdentifier_t GetDLLIdentifier() const override;

	// Set/get next cvar in the global list
	virtual void		 SetNext( IConCmdBase* pNext ) override;
	virtual IConCmdBase* GetNext() const override;

	// Internal usage only!
	virtual void SetRegistered( bool bRegistered ) override;
	virtual bool IsRegistered() const override;

private:
	bool		 bRegistered;
	uint32		 flags;
	const char*	 pName;
	const char*	 pHelpText;
	IConCmdBase* pNext;	 // Next IConVar/IConCmds in a chain
};

//-----------------------------------------------------------------------------
// The console invoked command
//-----------------------------------------------------------------------------
class CConCmd : public TConCmdBase<IConCmd>
{
public:
	CConCmd( const char* pName, conCmdExecFn_t pExecFn, const char* pHelpText = "", uint32 flags = FCVAR_NONE );

	// IConCmdBase interface
	virtual bool IsCommand() const override;

	// IConCmd interface
	virtual void Exec( uint32 argc, const char** argv ) override;

private:
	conCmdExecFn_t pExecFn;
};

//-----------------------------------------------------------------------------
// The console variable
//-----------------------------------------------------------------------------
class CConVar : public TConCmdBase<IConVar>
{
public:
	CConVar( const char* pName, const char* pDefaultValue, const char* pHelpText = "", uint32 flags = FCVAR_NONE, conVarChangeCallbackFn_t pChangeCallbackFn = NULL );
	CConVar( const char* pName, const char* pDefaultValue, bool bHasMin, float min, bool bHasMax, float max, const char* pHelpText = "", uint32 flags = FCVAR_NONE, conVarChangeCallbackFn_t pChangeCallbackFn = NULL );

	// IConCmdBase interface
	virtual bool IsCommand() const override;

	// IConVar interface
	virtual void Reset() override;

	virtual void SetInt( int32 value ) override;
	virtual void SetFloat( float value ) override;
	virtual void SetBool( bool value ) override;
	virtual void SetString( const char* pValue ) override;
	virtual void SetChangeCallback( conVarChangeCallbackFn_t pChangeCallbackFn ) override;

	// Internal usage only!
	// This either points to self or it points to the original declaration of a IConVar.
	// This allows IConVars to exist in separate modules, and they all use the first one to be declared
	//
	// pParentVar	Parent variable. pParentVar->GetParent() must equal pParentVar (ie: pParentVar must be the root, or original IConVar)
	virtual void	 SetParent( IConVar* pParentVar ) override;
	virtual IConVar* GetParent() const override;

	virtual int32		GetInt() const override;
	virtual float		GetFloat() const override;
	virtual bool		GetBool() const override;
	virtual const char* GetString() const override;
	virtual const char* GetDefault() const override;

	virtual conVarChangeCallbackFn_t GetChangeCallback() const override;
	virtual bool					 HasMin() const override;
	virtual bool					 HasMax() const override;
	virtual float					 GetMin() const override;
	virtual float					 GetMax() const override;

private:
	bool ClampValue( float& value );
	void OnChangeValue();

	bool					 bHasMin;
	bool					 bHasMax;
	float					 minValue;
	float					 maxValue;
	int32					 intValue;
	float					 floatValue;
	eastl::string			 stringValue;
	const char*				 pDefaultValue;
	IConVar*				 pParent;  // This either points to self or it points to the original declaration of a IConVar
	conVarChangeCallbackFn_t pChangeCallbackFn;
};

//-----------------------------------------------------------------------------
// Reference to IConVars that already exist (replaces the FindVar method)
//-----------------------------------------------------------------------------
class CConVarRef
{
public:
	CConVarRef( const char* pName, bool bIgnoreMissing = false )
		: pConVar( NULL )
	{
		Init( pName, bIgnoreMissing );
	}
	CConVarRef( IConVar* pConVar )
		: pConVar( pConVar )
	{
	}

	void Init( const char* pName, bool bIgnoreMissing = false );
	bool IsValid() const;

				operator bool() const;
				operator IConVar*() const;
				operator IConVar*&();
	CConVarRef& operator=( IConVar* pConVar );
	CConVarRef& operator=( const CConVarRef& copy );
	bool		operator==( const CConVarRef& right ) const;
	bool		operator==( IConVar* pRight ) const;
	bool		operator!=( const CConVarRef& right ) const;
	bool		operator!=( IConVar* pRight ) const;
	IConVar*	operator->() const;

private:
	IConVar* pConVar;
};

//-----------------------------------------------------------------------------
// Register and unregister IConVars and IConCmds with the ICvar
//
// flags - Add these flags to IConVars and IConCmds
// pAccessor - Accessor for register IConVars and IConCmds. If NULL use default accessor
// pConVarsOverrider - Used by ICvar to override IConVars (i.g for a command line). If NULL isn't be used
//-----------------------------------------------------------------------------
void ConVar_Register( uint32 flags = FCVAR_NONE, IConVarsOverrider* pConVarsOverrider = NULL, ICvarAccessor* pAccessor = NULL );
void ConVar_Unregister();

#include "tier1/convar.inl"
