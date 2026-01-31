#pragma once

/*
==================
CKeyValues::CKeyValues
==================
*/
FORCEINLINE CKeyValues::CKeyValues( const char* pName, CKeyValues* pParentKey /*= NULL*/ )
	: nameID( INVALID_INDEX )
	, bOwnNamePool( false )
	, dataType( KEYVALUES_DATA_TYPE_NONE )
	, pParentKey( NULL )
	, pNamePool( NULL )
{
	if ( pParentKey )
	{
		SetParent( pParentKey );
	}
	else
	{
		pNamePool	 = new namePool_t();
		bOwnNamePool = true;
	}

	SetName( pName );
}

/*
==================
CKeyValues::CKeyValues
==================
*/
FORCEINLINE CKeyValues::CKeyValues( const char* pName, uint32 length, CKeyValues* pParentKey /*= NULL*/ )
	: nameID( INVALID_INDEX )
	, bOwnNamePool( false )
	, dataType( KEYVALUES_DATA_TYPE_NONE )
	, pParentKey( NULL )
	, pNamePool( NULL )
{
	if ( pParentKey )
	{
		SetParent( pParentKey );
	}
	else
	{
		pNamePool	 = new namePool_t();
		bOwnNamePool = true;
	}

	SetName( pName, length );
}

/*
==================
CKeyValues::~CKeyValues
==================
*/
FORCEINLINE CKeyValues::~CKeyValues()
{
	if ( bOwnNamePool && pNamePool )
	{
		delete pNamePool;
	}

	DetachFromParent();
	RemoveAllSubKeys();
}

/*
==================
CKeyValues::AddSubKey
==================
*/
FORCEINLINE void CKeyValues::AddSubKey( CKeyValues* pKeyValue )
{
	Assert( pKeyValue );
	pKeyValue->SetParent( this );
}

/*
==================
CKeyValues::RemoveSubKey
==================
*/
FORCEINLINE void CKeyValues::RemoveSubKey( CKeyValues* pKeyValue, bool bDelete /* = true */ )
{
	Assert( pKeyValue );
	if ( bDelete )
	{
		delete pKeyValue;
	}
	else
	{
		pKeyValue->SetParent( NULL );
	}
}

/*
==================
CKeyValues::Clear
==================
*/
FORCEINLINE void CKeyValues::Clear()
{
	nameID = INVALID_INDEX;
	RemoveAllSubKeys();
	SetParent( NULL );
}

/*
==================
CKeyValues::DetachFromParent
==================
*/
FORCEINLINE void CKeyValues::DetachFromParent()
{
	if ( pParentKey )
	{
		pParentKey->subKeys.remove( this );
		pParentKey = NULL;
	}
}

/*
==================
CKeyValues::SetName
==================
*/
FORCEINLINE void CKeyValues::SetName( const char* pName )
{
	nameID = pNamePool->FindOrAdd( pName, S_Strlen( pName ) );
}

/*
==================
CKeyValues::SetName
==================
*/
FORCEINLINE void CKeyValues::SetName( const char* pName, uint32 length )
{
	nameID = pNamePool->FindOrAdd( pName, length );
}

/*
==================
CKeyValues::SetBool
==================
*/
FORCEINLINE void CKeyValues::SetBool( bool value )
{
	SetInt( (int32)value );
}

/*
==================
CKeyValues::SetInt
==================
*/
FORCEINLINE void CKeyValues::SetInt( int32 value )
{
	dataType   = KEYVALUES_DATA_TYPE_INT;
	valueInt32 = value;
	valueString.clear();
}

/*
==================
CKeyValues::SetInt64
==================
*/
FORCEINLINE void CKeyValues::SetInt64( int64 value )
{
	dataType   = KEYVALUES_DATA_TYPE_INT64;
	valueInt64 = value;
	valueString.clear();
}

/*
==================
CKeyValues::SetFloat
==================
*/
FORCEINLINE void CKeyValues::SetFloat( float value )
{
	dataType   = KEYVALUES_DATA_TYPE_FLOAT;
	valueFloat = value;
	valueString.clear();
}

/*
==================
CKeyValues::SetDouble
==================
*/
FORCEINLINE void CKeyValues::SetDouble( double value )
{
	dataType	= KEYVALUES_DATA_TYPE_DOUBLE;
	valueDouble = value;
	valueString.clear();
}

/*
==================
CKeyValues::SetString
==================
*/
FORCEINLINE void CKeyValues::SetString( const char* pValue )
{
	dataType	= KEYVALUES_DATA_TYPE_STRING;
	valueString = pValue;
}

/*
==================
CKeyValues::SetString
==================
*/
FORCEINLINE void CKeyValues::SetString( const char* pValue, uint32 length )
{
	dataType	= KEYVALUES_DATA_TYPE_STRING;
	valueString = eastl::string_view( pValue, length );
}

/*
==================
CKeyValues::GetName
==================
*/
FORCEINLINE const char* CKeyValues::GetName() const
{
	return pNamePool->GetString( nameID );
}

/*
==================
CKeyValues::GetBool
==================
*/
FORCEINLINE bool CKeyValues::GetBool( bool defaultValue /* = false */ ) const
{
	switch ( dataType )
	{
	case KEYVALUES_DATA_TYPE_INT: return valueInt32 != 0;
	case KEYVALUES_DATA_TYPE_INT64: return valueInt64 != 0;
	case KEYVALUES_DATA_TYPE_FLOAT: return valueFloat != 0;
	case KEYVALUES_DATA_TYPE_DOUBLE: return valueDouble != 0;
	case KEYVALUES_DATA_TYPE_STRING: return CStringToBool::Convert( valueString.c_str() );
	default: return defaultValue;
	}
}

/*
==================
CKeyValues::GetInt
==================
*/
FORCEINLINE int32 CKeyValues::GetInt( int32 defaultValue /* = 0 */ ) const
{
	switch ( dataType )
	{
	case KEYVALUES_DATA_TYPE_INT: return valueInt32;
	case KEYVALUES_DATA_TYPE_INT64: return (int32)valueInt64;
	case KEYVALUES_DATA_TYPE_FLOAT: return (int32)valueFloat;
	case KEYVALUES_DATA_TYPE_DOUBLE: return (int32)valueDouble;
	case KEYVALUES_DATA_TYPE_STRING: return S_Atoi( valueString.c_str() );
	default: return defaultValue;
	}
}

/*
==================
CKeyValues::GetInt64
==================
*/
FORCEINLINE int64 CKeyValues::GetInt64( int64 defaultValue /* = 0 */ ) const
{
	switch ( dataType )
	{
	case KEYVALUES_DATA_TYPE_INT: return (int64)valueInt32;
	case KEYVALUES_DATA_TYPE_INT64: return valueInt64;
	case KEYVALUES_DATA_TYPE_FLOAT: return (int64)valueFloat;
	case KEYVALUES_DATA_TYPE_DOUBLE: return (int64)valueDouble;
	case KEYVALUES_DATA_TYPE_STRING: return S_Atoi64( valueString.c_str() );
	default: return defaultValue;
	}
}

/*
==================
CKeyValues::GetFloat
==================
*/
FORCEINLINE float CKeyValues::GetFloat( float defaultValue /* = 0.f */ ) const
{
	switch ( dataType )
	{
	case KEYVALUES_DATA_TYPE_INT: return (float)valueInt32;
	case KEYVALUES_DATA_TYPE_INT64: return (float)valueInt64;
	case KEYVALUES_DATA_TYPE_FLOAT: return valueFloat;
	case KEYVALUES_DATA_TYPE_DOUBLE: return (float)valueDouble;
	case KEYVALUES_DATA_TYPE_STRING: return S_Atof( valueString.c_str() );
	default: return defaultValue;
	}
}

/*
==================
CKeyValues::GetDouble
==================
*/
FORCEINLINE double CKeyValues::GetDouble( double defaultValue /* = 0.0 */ ) const
{
	switch ( dataType )
	{
	case KEYVALUES_DATA_TYPE_INT: return (double)valueInt32;
	case KEYVALUES_DATA_TYPE_INT64: return (double)valueInt64;
	case KEYVALUES_DATA_TYPE_FLOAT: return (double)valueFloat;
	case KEYVALUES_DATA_TYPE_DOUBLE: return valueDouble;
	case KEYVALUES_DATA_TYPE_STRING: return S_Atod( valueString.c_str() );
	default: return defaultValue;
	}
}

/*
==================
CKeyValues::GetString
==================
*/
FORCEINLINE const char* CKeyValues::GetString( const char* pDefaultValue /* = "" */ ) const
{
	eastl::string& valueStringRef = (eastl::string&)valueString;
	switch ( dataType )
	{
	case KEYVALUES_DATA_TYPE_INT: valueStringRef = S_Sprintf( "%i", valueInt32 ); break;
	case KEYVALUES_DATA_TYPE_INT64: valueStringRef = S_Sprintf( "%lld", valueInt64 ); break;
	case KEYVALUES_DATA_TYPE_FLOAT: valueStringRef = S_Sprintf( "%.*g", S_MaxDigits10<float>(), valueFloat ); break;
	case KEYVALUES_DATA_TYPE_DOUBLE: valueStringRef = S_Sprintf( "%.*g", S_MaxDigits10<double>(), valueDouble ); break;
	case KEYVALUES_DATA_TYPE_STRING: break;
	default: return pDefaultValue;
	}

	return valueString.c_str();
}

/*
==================
CKeyValues::GetDataType
==================
*/
FORCEINLINE keyValuesDataType_t CKeyValues::GetDataType() const
{
	return dataType;
}

/*
==================
CKeyValues::GetParentKey
==================
*/
FORCEINLINE CKeyValues* CKeyValues::GetParentKey() const
{
	return pParentKey;
}

/*
==================
CKeyValues::GetSubKeys
==================
*/
FORCEINLINE const eastl::list<CKeyValues*>& CKeyValues::GetSubKeys() const
{
	return subKeys;
}
