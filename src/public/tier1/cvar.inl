#pragma once

/*
==================
CCVar::CCVar
==================
*/
FORCEINLINE CCVar::CCVar( const char* pName, const char* pDefaultValue, const char* pDescription /* = "" */, uint32 flags /* = CVAR_FLAG_NONE */, cvarChangeCallbackFn_t pChangeCallbackFn /* = NULL */ )
	: bRegistered( false )
	, bHasMin( false )
	, bHasMax( false )
	, flags( flags )
	, minValue( 0.f )
	, maxValue( 0.f )
	, intValue( 0 )
	, floatValue( 0.f )
	, pName( pName )
	, pDescription( pDescription )
	, pDefaultValue( pDefaultValue )
	, pParentCVar( this )
	, pNext( CCVarsLocalRegister::s_pCVarList )
{
	if ( pChangeCallbackFn )
	{
		changeCallbackFns.emplace_back( pChangeCallbackFn );
	}

	CCVarsLocalRegister::s_pCVarList = this;
	SetString( pDefaultValue );
}

/*
==================
CCVar::CCVar
==================
*/
FORCEINLINE CCVar::CCVar( const char* pName, const char* pDefaultValue, bool bHasMin, float min, bool bHasMax, float max, const char* pDescription /* = "" */, uint32 flags /* = CVAR_FLAG_NONE */, cvarChangeCallbackFn_t pChangeCallbackFn /* = NULL */ )
	: bRegistered( false )
	, bHasMin( bHasMin )
	, bHasMax( bHasMax )
	, flags( flags )
	, minValue( min )
	, maxValue( max )
	, intValue( 0 )
	, floatValue( 0.f )
	, pName( pName )
	, pDescription( pDescription )
	, pDefaultValue( pDefaultValue )
	, pParentCVar( this )
	, pNext( CCVarsLocalRegister::s_pCVarList )
{
	if ( pChangeCallbackFn )
	{
		changeCallbackFns.emplace_back( pChangeCallbackFn );
	}

	CCVarsLocalRegister::s_pCVarList = this;
	SetString( pDefaultValue );
}

/*
==================
CCVar::GetNext
==================
*/
FORCEINLINE CCVar* CCVar::GetNext() const
{
	return pNext;
}

/*
==================
CCVar::ClampValue
==================
*/
FORCEINLINE bool CCVar::ClampValue( float& value )
{
	if ( bHasMin && value < minValue )
	{
		value = minValue;
		return true;
	}

	if ( bHasMax && value > maxValue )
	{
		value = maxValue;
		return true;
	}

	return false;
}

/*
==================
CCVarRef::CCVarRef
==================
*/
FORCEINLINE CCVarRef::CCVarRef( const char* pName, bool bIgnoreMissing /* = false */ )
	: pCVar( NULL )
{
	Init( pName, bIgnoreMissing );
}

/*
==================
CCVarRef::CCVarRef
==================
*/
FORCEINLINE CCVarRef::CCVarRef( ICVar* pCVar )
	: pCVar( pCVar )
{
}

/*
==================
CCVarRef::IsValid
==================
*/
FORCEINLINE bool CCVarRef::IsValid() const
{
	return !!pCVar;
}

/*
==================
CCVarRef::operator bool
==================
*/
FORCEINLINE CCVarRef::operator bool() const
{
	return IsValid();
}

/*
==================
CCVarRef::operator ICVar*
==================
*/
FORCEINLINE CCVarRef::operator ICVar*() const
{
	return pCVar;
}

/*
==================
CCVarRef::operator ICVar*&
==================
*/
FORCEINLINE CCVarRef::operator ICVar*&()
{
	return pCVar;
}

/*
==================
CCVarRef::operator=
==================
*/
FORCEINLINE CCVarRef& CCVarRef::operator=( ICVar* pCVar )
{
	CCVarRef::pCVar = pCVar;
	return *this;
}

/*
==================
CCVarRef::operator=
==================
*/
FORCEINLINE CCVarRef& CCVarRef::operator=( const CCVarRef& other )
{
	pCVar = other.pCVar;
	return *this;
}

/*
==================
CCVarRef::operator==
==================
*/
FORCEINLINE bool CCVarRef::operator==( const CCVarRef& right ) const
{
	return pCVar == right.pCVar;
}

/*
==================
CCVarRef::operator==
==================
*/
FORCEINLINE bool CCVarRef::operator==( ICVar* pRight ) const
{
	return pCVar == pRight;
}

/*
==================
CCVarRef::operator!=
==================
*/
FORCEINLINE bool CCVarRef::operator!=( const CCVarRef& right ) const
{
	return pCVar != right.pCVar;
}

/*
==================
CCVarRef::operator!=
==================
*/
FORCEINLINE bool CCVarRef::operator!=( ICVar* pRight ) const
{
	return pCVar != pRight;
}

/*
==================
CCVarRef::operator->
==================
*/
FORCEINLINE ICVar* CCVarRef::operator->() const
{
	Assert( pCVar );
	return pCVar;
}
