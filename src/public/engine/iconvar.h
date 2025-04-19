#ifndef ICONVAR_H
#define ICONVAR_H

#include "engine/iconcmd.h"

//-----------------------------------------------------------------------------
// The console variable interface
//-----------------------------------------------------------------------------
typedef void ( *conVarChangeCallbackFn_t )( class IConVar* pConVar );

class IConVar : public IConCmdBase
{
public:
	virtual void Reset() = 0;

	virtual void SetInt( int32 value ) = 0;
	virtual void SetFloat( float value ) = 0;
	virtual void SetBool( bool value ) = 0;
	virtual void SetString( const achar* pValue ) = 0;
	virtual void SetChangeCallback( conVarChangeCallbackFn_t pChangeCallbackFn ) = 0;

	// Internal usage only!
	// This either points to self or it points to the original declaration of a IConVar.
	// This allows IConVars to exist in separate modules, and they all use the first one to be declared
	//
	// pParentVar	Parent variable. pParentVar->GetParent() must equal pParentVar (ie: pParentVar must be the root, or original IConVar)
	virtual void SetParent( IConVar* pParentVar ) = 0;
	virtual IConVar* GetParent() const = 0;

	virtual int32 GetInt() const = 0;
	virtual float GetFloat() const = 0;
	virtual bool GetBool() const = 0;
	virtual const achar* GetString() const = 0;
	virtual const achar* GetDefault() const = 0;

	virtual conVarChangeCallbackFn_t GetChangeCallback() const = 0;
	virtual bool HasMin() const = 0;
	virtual bool HasMax() const = 0;
	virtual float GetMin() const = 0;
	virtual float GetMax() const = 0;
};

#endif // !ICONVAR_H