#pragma once
#include <EASTL/string.h>
#include "cvar/icvarsystem.h"

//-----------------------------------------------------------------------------
// A console variable
//-----------------------------------------------------------------------------
class CCVar : public ICVar
{
public:
	// ICVar interface
	virtual void				SetDescription( const char* pDescription ) override;
	virtual const char*			GetName() const override;
	virtual const char*			GetDescription() const override;
	virtual bool				IsFlagSet( uint32 flag ) const override;
	virtual uint32				GetFlags() const override;
	virtual cvarDLLIdentifier_t GetDLLIdentifier() const override;

	virtual void SetInt( int32 value ) override;
	virtual void SetFloat( float value ) override;
	virtual void SetString( const char* pValue ) override;
	virtual void Reset() override;

	virtual bool IsModified() const override;
	virtual void SetModified() override;
	virtual void ClearModified() override;

	virtual bool  HasMin() const override;
	virtual bool  HasMax() const override;
	virtual float GetMin() const override;
	virtual float GetMax() const override;

	virtual int32		GetInt() const override;
	virtual float		GetFloat() const override;
	virtual const char* GetString() const override;
	virtual const char* GetDefault() const override;

	// Install a change callback (there shouldn't already be one)
	virtual void InstallChangeCallback( cvarChangeCallbackFn_t pCallbackFn, bool bInvoke = true ) override;
	virtual void RemoveChangeCallback( cvarChangeCallbackFn_t pCallbackFnToRemove ) override;
	virtual void RemoveAllChangeCallbacks() override;

	virtual uint32				   GetNumChangeCallbacks() const override;
	virtual cvarChangeCallbackFn_t GetChangeCallback( uint32 slot ) const override;

	// Internal usage only!
	// This either points to self or it points to the original declaration of a cvar.
	// This allows cvars to exist in separate modules, and they all use the first one to be declared
	//
	// pParentCVar - Parent variable. pParentCVar->GetParent() must equal pParentCVar (ie: pParentCVar must be the root, or original cvar)
	virtual void   SetParent( ICVar* pParentCVar ) override;
	virtual ICVar* GetParent() const override;

	virtual void SetRegistered( bool bRegistered ) override;
	virtual bool IsRegistered() const override;

	CCVar( const char* pName, const char* pDefaultValue, const char* pDescription = "", uint32 flags = CVAR_FLAG_NONE, cvarChangeCallbackFn_t pChangeCallbackFn = NULL );
	CCVar( const char* pName, const char* pDefaultValue, bool bHasMin, float min, bool bHasMax, float max, const char* pDescription = "", uint32 flags = CVAR_FLAG_NONE, cvarChangeCallbackFn_t pChangeCallbackFn = NULL );
	CCVar* GetNext() const;

private:
	bool ClampValue( float& value );
	void ChangeStringValue( const char* pNewStringValue, float oldIntFloatValue );

	bool								  bRegistered;
	bool								  bHasMin;
	bool								  bHasMax;
	uint32								  flags;
	float								  minValue;
	float								  maxValue;
	int32								  intValue;
	float								  floatValue;
	eastl::string						  stringValue;
	const char*							  pName;
	const char*							  pDescription;
	const char*							  pDefaultValue;
	ICVar*								  pParentCVar;	// This either points to self or it points to the original declaration of a IConVar
	CCVar*								  pNext;
	eastl::vector<cvarChangeCallbackFn_t> changeCallbackFns;
};

//-----------------------------------------------------------------------------
// Register console variables implemented in a local module
//-----------------------------------------------------------------------------
class CCVarsLocalRegister
{
	friend CCVar;
	friend void LinkCVars( uint32 flags );
	friend void UnlinkCVars();

protected:
	static CCVar*			   s_pCVarList;	 // Statically constructed list of CCvar, used for link them in the cvar system
	static cvarDLLIdentifier_t s_dllIdentifier;
};

//-----------------------------------------------------------------------------
// Reference to a cvar that already exist (replaces the Find method)
//----------------------------------------------------------------------------
class CCVarRef
{
public:
	CCVarRef( const char* pName, bool bIgnoreMissing = false );
	CCVarRef( ICVar* pCVar );

	void Init( const char* pName, bool bIgnoreMissing = false );
	bool IsValid() const;

			  operator bool() const;
			  operator ICVar*() const;
			  operator ICVar*&();
	CCVarRef& operator=( ICVar* pCVar );
	CCVarRef& operator=( const CCVarRef& other );
	bool	  operator==( const CCVarRef& right ) const;
	bool	  operator==( ICVar* pRight ) const;
	bool	  operator!=( const CCVarRef& right ) const;
	bool	  operator!=( ICVar* pRight ) const;
	ICVar*	  operator->() const;

private:
	ICVar* pCVar;
};

//-----------------------------------------------------------------------------
// Link and unlink cvars in the cvar system
// flags - Add these flags to cvars
//-----------------------------------------------------------------------------
void LinkCVars( uint32 flags = CVAR_FLAG_NONE );
void UnlinkCVars();

#include "tier1/cvar.inl"
