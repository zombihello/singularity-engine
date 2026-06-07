#include "pch_tier1.h"
#include "tier1/cvar.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
CCVar*				CCVarsLocalRegister::s_pCVarList	 = NULL;
cvarDLLIdentifier_t CCVarsLocalRegister::s_dllIdentifier = INVALID_INDEX;

/*
==================
CCVar::SetDescription
==================
*/
void CCVar::SetDescription( const char* pDescription )
{
	CCVar::pDescription = pDescription ? pDescription : "";
}

/*
==================
CCVar::GetName
==================
*/
const char* CCVar::GetName() const
{
	return pParentCVar == this ? pName : pParentCVar->GetName();
}

/*
==================
CCVar::GetDescription
==================
*/
const char* CCVar::GetDescription() const
{
	return pParentCVar == this ? pDescription : pParentCVar->GetDescription();
}

/*
==================
CCVar::IsFlagSet
==================
*/
bool CCVar::IsFlagSet( uint32 flag ) const
{
	return ( GetFlags() & flag ) ? true : false;
}

/*
==================
CCVar::GetFlags
==================
*/
uint32 CCVar::GetFlags() const
{
	return pParentCVar == this ? flags : pParentCVar->GetFlags();
}

/*
==================
CCVar::GetDLLIdentifier
==================
*/
cvarDLLIdentifier_t CCVar::GetDLLIdentifier() const
{
	return CCVarsLocalRegister::s_dllIdentifier;
}

/*
==================
CCVar::SetInt
==================
*/
void CCVar::SetInt( int32 value )
{
	if ( pParentCVar == this )
	{
		if ( value != intValue )
		{
			// Redetermine value
			float oldFloatValue = floatValue;
			floatValue			= (float)value;
			ClampValue( floatValue );
			intValue = (int32)floatValue;
			ChangeStringValue( va( "%i", intValue ), oldFloatValue );
		}
	}
	else
	{
		pParentCVar->SetInt( value );
	}
}

/*
==================
CCVar::SetFloat
==================
*/
void CCVar::SetFloat( float value )
{
	if ( pParentCVar == this )
	{
		if ( value != floatValue )
		{
			// Redetermine value
			float oldFloatValue = floatValue;
			ClampValue( value );
			floatValue = value;
			intValue   = (int32)value;
			ChangeStringValue( va( "%f", value ), oldFloatValue );
		}
	}
	else
	{
		pParentCVar->SetFloat( value );
	}
}

/*
==================
CCVar::SetString
==================
*/
void CCVar::SetString( const char* pValue )
{
	if ( pParentCVar == this )
	{
		if ( stringValue != pValue )
		{
			// Redetermine value
			float oldFloatValue = floatValue;
			floatValue			= !pValue ? 0.f : S_Atof( pValue );
			bool bValueClamped	= ClampValue( floatValue );
			intValue			= (int32)floatValue;
			ChangeStringValue( !bValueClamped ? pValue : va( "%f", floatValue ), oldFloatValue );
		}
	}
	else
	{
		pParentCVar->SetString( pValue );
	}
}

/*
==================
CCVar::ChangeStringValue
==================
*/
void CCVar::ChangeStringValue( const char* pNewStringValue, float oldIntFloatValue )
{
	// Mark the cvar as modified and cvar's flags into the cvar system as modified
	SetModified();
	if ( g_pCVarSystem )
	{
		g_pCVarSystem->SetModifiedFlags( flags );
	}

	// Copy to the stack old string value
	uint32 oldStringValueLen = (uint32)stringValue.size();
	char*  pOldStringValue	 = (char*)Mem_Alloca( oldStringValueLen * sizeof( char ) + 1 );
	Mem_Memcpy( pOldStringValue, stringValue.c_str(), oldStringValueLen * sizeof( char ) );
	pOldStringValue[oldStringValueLen] = '\0';

	// Redetermine string value
	stringValue = pNewStringValue;

	// Invoke any necessary callback function
	for ( uint32 index = 0, count = (uint32)changeCallbackFns.size(); index < count; ++index )
	{
		changeCallbackFns[index]( this, pOldStringValue, oldIntFloatValue );
	}
}

/*
==================
CCVar::Reset
==================
*/
void CCVar::Reset()
{
	if ( pParentCVar == this )
	{
		SetString( GetDefault() );
		ClearModified();
	}
	else
	{
		pParentCVar->Reset();
	}
}

/*
==================
CCVar::IsModified
==================
*/
bool CCVar::IsModified() const
{
	return pParentCVar == this ? flags & CVAR_FLAG_MODIFIED : pParentCVar->IsModified();
}

/*
==================
CCVar::SetModified
==================
*/
void CCVar::SetModified()
{
	if ( pParentCVar == this )
	{
		flags |= CVAR_FLAG_MODIFIED;
	}
	else
	{
		pParentCVar->SetModified();
	}
}

/*
==================
CCVar::ClearModified
==================
*/
void CCVar::ClearModified()
{
	if ( pParentCVar == this )
	{
		flags &= ~CVAR_FLAG_MODIFIED;
	}
	else
	{
		pParentCVar->ClearModified();
	}
}

/*
==================
CCVar::HasMin
==================
*/
bool CCVar::HasMin() const
{
	return pParentCVar == this ? bHasMin : pParentCVar->HasMin();
}

/*
==================
CCVar::HasMax
==================
*/
bool CCVar::HasMax() const
{
	return pParentCVar == this ? bHasMax : pParentCVar->HasMax();
}

/*
==================
CCVar::GetMin
==================
*/
float CCVar::GetMin() const
{
	return pParentCVar == this ? minValue : pParentCVar->GetMin();
}

/*
==================
CCVar::GetMax
==================
*/
float CCVar::GetMax() const
{
	return pParentCVar == this ? maxValue : pParentCVar->GetMax();
}

/*
==================
CCVar::GetInt
==================
*/
int32 CCVar::GetInt() const
{
	return pParentCVar == this ? intValue : pParentCVar->GetInt();
}

/*
==================
CCVar::GetFloat
==================
*/
float CCVar::GetFloat() const
{
	return pParentCVar == this ? floatValue : pParentCVar->GetFloat();
}

/*
==================
CCVar::GetString
==================
*/
const char* CCVar::GetString() const
{
	return pParentCVar == this ? stringValue.c_str() : pParentCVar->GetString();
}

/*
==================
CCVar::GetDefault
==================
*/
const char* CCVar::GetDefault() const
{
	return pParentCVar == this ? pDefaultValue : pParentCVar->GetDefault();
}

/*
==================
CCVar::InstallChangeCallback
==================
*/
void CCVar::InstallChangeCallback( cvarChangeCallbackFn_t pCallbackFn, bool bInvoke /* = true */ )
{
	if ( pParentCVar == this )
	{
		// Make sure that a callback is valid
		if ( !pCallbackFn )
		{
			Warning( "CVar: InstallChangeCallback called with NULL callback, ignoring (cvar: '%s')", pName );
			return;
		}

		// Make sure that the callback wasn't added yet
		if ( eastl::find( changeCallbackFns.begin(), changeCallbackFns.end(), pCallbackFn ) != changeCallbackFns.end() )
		{
			Warning( "CVar: InstallChangeCallback ignoring duplicate change callback (cvar: '%s')", pName );
			return;
		}
		changeCallbackFns.emplace_back( pCallbackFn );

		// Call it immediately to set the initial value
		if ( bInvoke )
		{
			pCallbackFn( this, stringValue.c_str(), floatValue );
		}
	}
	else
	{
		pParentCVar->InstallChangeCallback( pCallbackFn, bInvoke );
	}
}

/*
==================
CCVar::RemoveChangeCallback
==================
*/
void CCVar::RemoveChangeCallback( cvarChangeCallbackFn_t pCallbackFnToRemove )
{
	if ( pParentCVar == this )
	{
		changeCallbackFns.erase_first( pCallbackFnToRemove );
	}
	else
	{
		pParentCVar->RemoveChangeCallback( pCallbackFnToRemove );
	}
}

/*
==================
CCVar::RemoveAllChangeCallbacks
==================
*/
void CCVar::RemoveAllChangeCallbacks()
{
	if ( pParentCVar == this )
	{
		changeCallbackFns.clear();
	}
	else
	{
		pParentCVar->RemoveAllChangeCallbacks();
	}
}

/*
==================
CCVar::GetNumChangeCallbacks
==================
*/
uint32 CCVar::GetNumChangeCallbacks() const
{
	return pParentCVar == this ? (uint32)changeCallbackFns.size() : pParentCVar->GetNumChangeCallbacks();
}

/*
==================
CCVar::GetChangeCallback
==================
*/
cvarChangeCallbackFn_t CCVar::GetChangeCallback( uint32 slot ) const
{
	Assert( slot < GetNumChangeCallbacks() );
	return pParentCVar == this ? changeCallbackFns[slot] : pParentCVar->GetChangeCallback( slot );
}

/*
==================
CCVar::SetParent
==================
*/
void CCVar::SetParent( ICVar* pParentCVar )
{
	if ( pParentCVar && pParentCVar != this )
	{
		Assert( pParentCVar->GetParent() == pParentCVar );
		CCVar::pParentCVar = pParentCVar;
	}
	else
	{
		CCVar::pParentCVar = this;
	}
}

/*
==================
CCVar::GetParent
==================
*/
ICVar* CCVar::GetParent() const
{
	return pParentCVar;
}

/*
==================
CCVar::SetRegistered
==================
*/
void CCVar::SetRegistered( bool bRegistered )
{
	CCVar::bRegistered = bRegistered;
}

/*
==================
CCVar::IsRegistered
==================
*/
bool CCVar::IsRegistered() const
{
	return bRegistered;
}

/*
==================
CCVarRef::Init
==================
*/
void CCVarRef::Init( const char* pName, bool bIgnoreMissing /* = false */ )
{
	// Try find a cvar in the cvar system
	pCVar = g_pCVarSystem ? g_pCVarSystem->FindVariable( pName ) : NULL;

	// If pCVar isn't found print warning
	if ( !IsValid() && !bIgnoreMissing )
	{
		static bool s_bFirst = true;
		if ( s_bFirst )
		{
			Warning( "Tier1: CCVarRef '%s' doesn't point to an existing cvar", pName );
			s_bFirst = false;
		}
	}
}

/*
==================
LinkCVars
==================
*/
void LinkCVars( uint32 flags /* = CVAR_FLAG_NONE */ )
{
	if ( !g_pCVarSystem || CCVarsLocalRegister::s_dllIdentifier != INVALID_INDEX )
	{
		return;
	}

	CCVarsLocalRegister::s_dllIdentifier = g_pCVarSystem->AllocateDLLIdentifier();
	for ( CCVar* pCurCVar = CCVarsLocalRegister::s_pCVarList; pCurCVar; pCurCVar = pCurCVar->GetNext() )
	{
		g_pCVarSystem->RegisterVariable( pCurCVar );
	}
}

/*
==================
UnlinkCVars
==================
*/
void UnlinkCVars()
{
	if ( !g_pCVarSystem || CCVarsLocalRegister::s_dllIdentifier == INVALID_INDEX )
	{
		return;
	}

	g_pCVarSystem->UnregisterAllVariables( CCVarsLocalRegister::s_dllIdentifier );
	CCVarsLocalRegister::s_dllIdentifier = INVALID_INDEX;
}
