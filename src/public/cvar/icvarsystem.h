#pragma once
#include "tier1/cmdargs.h"
#include "appframework/iappsystem.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStreamDataWriter;
class ICVar;

//-----------------------------------------------------------------------------
// Console variables are marked as having a particular DLL identifier
//-----------------------------------------------------------------------------
typedef uint32 cvarDLLIdentifier_t;

//-----------------------------------------------------------------------------
// Console variable flags
//-----------------------------------------------------------------------------
enum cvarFlag_t
{
	CVAR_FLAG_NONE	   = 0,
	CVAR_FLAG_CHEAT	   = BIT( 0 ),	// Command is considered a cheat
	CVAR_FLAG_ARCHIVE  = BIT( 1 ),	// Set to cause it to be saved to a config file
	CVAR_FLAG_MODIFIED = BIT( 2 ),	// Set when the variable is modified
	CVAR_FLAG_ALL	   = -1,
};

//-----------------------------------------------------------------------------
// CVar callback
//-----------------------------------------------------------------------------
typedef void ( *cvarChangeCallbackFn_t )( ICVar* pCVar, const char* pOldValueStr, float oldValue );

//-----------------------------------------------------------------------------
// A console variable interface
//-----------------------------------------------------------------------------
class ICVar
{
public:
	virtual void				SetDescription( const char* pDescription ) = 0;
	virtual const char*			GetName() const							   = 0;
	virtual const char*			GetDescription() const					   = 0;
	virtual bool				IsFlagSet( uint32 flag ) const			   = 0;
	virtual uint32				GetFlags() const						   = 0;
	virtual cvarDLLIdentifier_t GetDLLIdentifier() const				   = 0;

	virtual void SetInt( int32 value )			 = 0;
	virtual void SetFloat( float value )		 = 0;
	virtual void SetString( const char* pValue ) = 0;
	virtual void Reset()						 = 0;

	virtual bool IsModified() const = 0;
	virtual void SetModified()		= 0;
	virtual void ClearModified()	= 0;

	virtual bool  HasMin() const = 0;
	virtual bool  HasMax() const = 0;
	virtual float GetMin() const = 0;
	virtual float GetMax() const = 0;

	virtual int32		GetInt() const	   = 0;
	virtual float		GetFloat() const   = 0;
	virtual const char* GetString() const  = 0;
	virtual const char* GetDefault() const = 0;

	// Install a change callback (there shouldn't already be one)
	virtual void InstallChangeCallback( cvarChangeCallbackFn_t pCallbackFn, bool bInvoke = true ) = 0;
	virtual void RemoveChangeCallback( cvarChangeCallbackFn_t pCallbackFnToRemove )				  = 0;
	virtual void RemoveAllChangeCallbacks()														  = 0;

	virtual uint32				   GetNumChangeCallbacks() const		  = 0;
	virtual cvarChangeCallbackFn_t GetChangeCallback( uint32 slot ) const = 0;

	// Internal usage only!
	// This either points to self or it points to the original declaration of a cvar.
	// This allows cvars to exist in separate modules, and they all use the first one to be declared
	//
	// pParentCVar - Parent variable. pParentCVar->GetParent() must equal pParentCVar (ie: pParentCVar must be the root, or original cvar)
	virtual void   SetParent( ICVar* pParentCVar ) = 0;
	virtual ICVar* GetParent() const			   = 0;

	virtual void SetRegistered( bool bRegistered ) = 0;
	virtual bool IsRegistered() const			   = 0;
};

//-----------------------------------------------------------------------------
// The console variable system interface
//-----------------------------------------------------------------------------
#define CVARSYSTEM_INTERFACE_VERSION "SCVarSystem001"
class ICVarSystem : public IAppSystem
{
public:
	// Allocate a new identifier for a DLL
	virtual cvarDLLIdentifier_t AllocateDLLIdentifier() = 0;

	// Register and unregister a cvar
	// NOTE: If set dllIdentifier then will be removed a cvar of specific a dll module
	virtual void RegisterVariable( ICVar* pCVar )											 = 0;
	virtual void UnregisterVariable( ICVar* pCVar )											 = 0;
	virtual void UnregisterAllVariables( cvarDLLIdentifier_t dllIdentifier = INVALID_INDEX ) = 0;

	// Finds the cvar with the given name
	// Returns NULL if there is no cvar with the given name
	virtual ICVar* FindVariable( const char* pName, uint32 length ) const = 0;
	ICVar*		   FindVariable( const char* pName ) const;

	// Called by the command system when argv(0) doesn't match a known command
	// Returns true if argv(0) is a variable reference and prints or changes the CVar
	virtual bool ExecuteCommand( const CCmdArgs& args ) = 0;

	// Sets/gets/clears modified flags that tell what kind of cvars have changed
	virtual void   SetModifiedFlags( uint32 flags )	  = 0;
	virtual uint32 GetModifiedFlags() const			  = 0;
	virtual void   ClearModifiedFlags( uint32 flags ) = 0;

	// Resets variables with one of the given flags set
	virtual void ResetFlaggedVariables( uint32 flags ) = 0;

	// Writes variables with one of the given flags set to the given stream data.
	virtual void WriteFlaggedVariables( uint32 flags, IStreamDataWriter* pStreamData ) const = 0;
};

#include "cvar/icvarsystem.inl"
