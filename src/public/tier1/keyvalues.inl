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
CKeyValues::LoadFromFile
==================
*/
FORCEINLINE bool CKeyValues::LoadFromFile( const char* pPath )
{
	// Do nothing if file system isn't valid
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try open file
	CRefPtr<IStreamDataReader> pFile = g_pFileSystem->CreateFileReader( pPath );
	if ( !pFile )
	{
		return false;
	}

	// Parse the key values
	return LoadFromStream( pFile );
}

/*
==================
CKeyValues::LoadFromBuffer
==================
*/
FORCEINLINE bool CKeyValues::LoadFromBuffer( const char* pBuffer, uint64 size )
{
	PROFILER_SCOPE_FUNC();
	CStreamDataMemoryReader streamReader( (byte*)pBuffer, size );
	return LoadFromStream( &streamReader );
}

/*
==================
CKeyValues::SaveToFile
==================
*/
FORCEINLINE bool CKeyValues::SaveToFile( const char* pPath ) const
{
	// Do nothing if file system isn't valid
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try to open a file
	CRefPtr<IStreamDataWriter> pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Warning( "KeyValues: Failed to create file '%s'", pPath );
		return false;
	}

	// Serialize key values to the file
	SaveToStream( pFile );
	return true;
}

/*
==================
CKeyValues::SaveToBuffer
==================
*/
FORCEINLINE void CKeyValues::SaveToBuffer( eastl::vector<byte>& buffer ) const
{
	PROFILER_SCOPE_FUNC();
	CStreamDataMemoryWriter streamWriter( buffer );
	return SaveToStream( &streamWriter );
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
CKeyValues::SetSchema
==================
*/
FORCEINLINE void CKeyValues::SetSchema( const char* pSchema )
{
	valueSchema = pSchema;
}

/*
==================
CKeyValues::SetBool
==================
*/
FORCEINLINE void CKeyValues::SetBool( const char* pKeyName, bool value, const char* pSchema /* = NULL */ )
{
	SetInt( pKeyName, (int32)value, pSchema );
}

/*
==================
CKeyValues::SetInt
==================
*/
FORCEINLINE void CKeyValues::SetInt( const char* pKeyName, int32 value, const char* pSchema /* = NULL */ )
{
	CKeyValues* pKeyValues = FindKey( pKeyName, true );
	pKeyValues->dataType   = KEYVALUES_DATA_TYPE_INT;
	pKeyValues->valueInt32 = value;
	pKeyValues->valueString.clear();
	if ( pSchema )
	{
		pKeyValues->valueSchema = pSchema;
	}
}

/*
==================
CKeyValues::SetInt64
==================
*/
FORCEINLINE void CKeyValues::SetInt64( const char* pKeyName, int64 value, const char* pSchema /* = NULL */ )
{
	CKeyValues* pKeyValues = FindKey( pKeyName, true );
	pKeyValues->dataType   = KEYVALUES_DATA_TYPE_INT64;
	pKeyValues->valueInt64 = value;
	pKeyValues->valueString.clear();
	if ( pSchema )
	{
		pKeyValues->valueSchema = pSchema;
	}
}

/*
==================
CKeyValues::SetFloat
==================
*/
FORCEINLINE void CKeyValues::SetFloat( const char* pKeyName, float value, const char* pSchema /* = NULL */ )
{
	CKeyValues* pKeyValues = FindKey( pKeyName, true );
	pKeyValues->dataType   = KEYVALUES_DATA_TYPE_FLOAT;
	pKeyValues->valueFloat = value;
	pKeyValues->valueString.clear();
	if ( pSchema )
	{
		pKeyValues->valueSchema = pSchema;
	}
}

/*
==================
CKeyValues::SetDouble
==================
*/
FORCEINLINE void CKeyValues::SetDouble( const char* pKeyName, double value, const char* pSchema /* = NULL */ )
{
	CKeyValues* pKeyValues	= FindKey( pKeyName, true );
	pKeyValues->dataType	= KEYVALUES_DATA_TYPE_DOUBLE;
	pKeyValues->valueDouble = value;
	pKeyValues->valueString.clear();
	if ( pSchema )
	{
		pKeyValues->valueSchema = pSchema;
	}
}

/*
==================
CKeyValues::SetString
==================
*/
FORCEINLINE void CKeyValues::SetString( const char* pKeyName, const char* pValue, const char* pSchema /* = NULL */ )
{
	CKeyValues* pKeyValues	= FindKey( pKeyName, true );
	pKeyValues->dataType	= KEYVALUES_DATA_TYPE_STRING;
	pKeyValues->valueString = pValue;
	if ( pSchema )
	{
		pKeyValues->valueSchema = pSchema;
	}
}

/*
==================
CKeyValues::HasData
==================
*/
FORCEINLINE bool CKeyValues::IsEmpty() const
{
	return !HasData() && !HasSubKeys();
}

/*
==================
CKeyValues::HasData
==================
*/
FORCEINLINE bool CKeyValues::HasData() const
{
	return dataType != KEYVALUES_DATA_TYPE_NONE;
}

/*
==================
CKeyValues::HasSubKeys
==================
*/
FORCEINLINE bool CKeyValues::HasSubKeys() const
{
	return !subKeys.empty();
}

/*
==================
CKeyValues::HasSchema
==================
*/
FORCEINLINE bool CKeyValues::HasSchema() const
{
	return !valueSchema.empty();
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
CKeyValues::GetSchema
==================
*/
FORCEINLINE const char* CKeyValues::GetSchema( const char* pKeyName ) const
{
	CKeyValues* pKeyValues = const_cast<CKeyValues*>( this )->FindKey( pKeyName );
	return pKeyValues ? pKeyValues->HasSchema() ? pKeyValues->valueSchema.c_str() : NULL : NULL;
}

/*
==================
CKeyValues::GetBool
==================
*/
FORCEINLINE bool CKeyValues::GetBool( const char* pKeyName, bool defaultValue /* = false */, const char** pSchema /* = NULL */, bool* pbGotDefaultValue /* = NULL */ ) const
{
	CKeyValues* pKeyValues = const_cast<CKeyValues*>( this )->FindKey( pKeyName );
	if ( pKeyValues )
	{
		if ( pbGotDefaultValue )
		{
			*pbGotDefaultValue = false;
		}
		if ( pSchema )
		{
			*pSchema = pKeyValues->HasSchema() ? pKeyValues->valueSchema.c_str() : NULL;
		}

		switch ( pKeyValues->dataType )
		{
		case KEYVALUES_DATA_TYPE_INT: return pKeyValues->valueInt32 != 0;
		case KEYVALUES_DATA_TYPE_INT64: return pKeyValues->valueInt64 != 0;
		case KEYVALUES_DATA_TYPE_FLOAT: return pKeyValues->valueFloat != 0;
		case KEYVALUES_DATA_TYPE_DOUBLE: return pKeyValues->valueDouble != 0;
		case KEYVALUES_DATA_TYPE_STRING: return CStringToBool::Convert( pKeyValues->valueString.c_str() );
		}
	}

	if ( pbGotDefaultValue )
	{
		*pbGotDefaultValue = true;
	}
	if ( pSchema )
	{
		*pSchema = NULL;
	}
	return defaultValue;
}

/*
==================
CKeyValues::GetInt
==================
*/
FORCEINLINE int32 CKeyValues::GetInt( const char* pKeyName, int32 defaultValue /* = 0 */, const char** pSchema /* = NULL */, bool* pbGotDefaultValue /* = NULL */ ) const
{
	CKeyValues* pKeyValues = const_cast<CKeyValues*>( this )->FindKey( pKeyName );
	if ( pKeyValues )
	{
		if ( pbGotDefaultValue )
		{
			*pbGotDefaultValue = false;
		}
		if ( pSchema )
		{
			*pSchema = pKeyValues->HasSchema() ? pKeyValues->valueSchema.c_str() : NULL;
		}

		switch ( pKeyValues->dataType )
		{
		case KEYVALUES_DATA_TYPE_INT: return pKeyValues->valueInt32;
		case KEYVALUES_DATA_TYPE_INT64: return (int32)pKeyValues->valueInt64;
		case KEYVALUES_DATA_TYPE_FLOAT: return (int32)pKeyValues->valueFloat;
		case KEYVALUES_DATA_TYPE_DOUBLE: return (int32)pKeyValues->valueDouble;
		case KEYVALUES_DATA_TYPE_STRING: return S_Atoi( pKeyValues->valueString.c_str() );
		}
	}

	if ( pbGotDefaultValue )
	{
		*pbGotDefaultValue = true;
	}
	if ( pSchema )
	{
		*pSchema = NULL;
	}
	return defaultValue;
}

/*
==================
CKeyValues::GetInt64
==================
*/
FORCEINLINE int64 CKeyValues::GetInt64( const char* pKeyName, int64 defaultValue /* = 0 */, const char** pSchema /* = NULL */, bool* pbGotDefaultValue /* = NULL */ ) const
{
	CKeyValues* pKeyValues = const_cast<CKeyValues*>( this )->FindKey( pKeyName );
	if ( pKeyValues )
	{
		if ( pbGotDefaultValue )
		{
			*pbGotDefaultValue = false;
		}
		if ( pSchema )
		{
			*pSchema = pKeyValues->HasSchema() ? pKeyValues->valueSchema.c_str() : NULL;
		}

		switch ( pKeyValues->dataType )
		{
		case KEYVALUES_DATA_TYPE_INT: return (int64)pKeyValues->valueInt32;
		case KEYVALUES_DATA_TYPE_INT64: return pKeyValues->valueInt64;
		case KEYVALUES_DATA_TYPE_FLOAT: return (int64)pKeyValues->valueFloat;
		case KEYVALUES_DATA_TYPE_DOUBLE: return (int64)pKeyValues->valueDouble;
		case KEYVALUES_DATA_TYPE_STRING: return S_Atoi64( pKeyValues->valueString.c_str() );
		}
	}

	if ( pbGotDefaultValue )
	{
		*pbGotDefaultValue = true;
	}
	if ( pSchema )
	{
		*pSchema = NULL;
	}
	return defaultValue;
}

/*
==================
CKeyValues::GetFloat
==================
*/
FORCEINLINE float CKeyValues::GetFloat( const char* pKeyName, float defaultValue /* = 0.f */, const char** pSchema /* = NULL */, bool* pbGotDefaultValue /* = NULL */ ) const
{
	CKeyValues* pKeyValues = const_cast<CKeyValues*>( this )->FindKey( pKeyName );
	if ( pKeyValues )
	{
		if ( pbGotDefaultValue )
		{
			*pbGotDefaultValue = false;
		}
		if ( pSchema )
		{
			*pSchema = pKeyValues->HasSchema() ? pKeyValues->valueSchema.c_str() : NULL;
		}

		switch ( pKeyValues->dataType )
		{
		case KEYVALUES_DATA_TYPE_INT: return (float)pKeyValues->valueInt32;
		case KEYVALUES_DATA_TYPE_INT64: return (float)pKeyValues->valueInt64;
		case KEYVALUES_DATA_TYPE_FLOAT: return pKeyValues->valueFloat;
		case KEYVALUES_DATA_TYPE_DOUBLE: return (float)pKeyValues->valueDouble;
		case KEYVALUES_DATA_TYPE_STRING: return S_Atof( pKeyValues->valueString.c_str() );
		}
	}

	if ( pbGotDefaultValue )
	{
		*pbGotDefaultValue = true;
	}
	if ( pSchema )
	{
		*pSchema = NULL;
	}
	return defaultValue;
}

/*
==================
CKeyValues::GetDouble
==================
*/
FORCEINLINE double CKeyValues::GetDouble( const char* pKeyName, double defaultValue /* = 0.0 */, const char** pSchema /* = NULL */, bool* pbGotDefaultValue /* = NULL */ ) const
{
	CKeyValues* pKeyValues = const_cast<CKeyValues*>( this )->FindKey( pKeyName );
	if ( pKeyValues )
	{
		if ( pbGotDefaultValue )
		{
			*pbGotDefaultValue = false;
		}
		if ( pSchema )
		{
			*pSchema = pKeyValues->HasSchema() ? pKeyValues->valueSchema.c_str() : NULL;
		}

		switch ( pKeyValues->dataType )
		{
		case KEYVALUES_DATA_TYPE_INT: return (double)pKeyValues->valueInt32;
		case KEYVALUES_DATA_TYPE_INT64: return (double)pKeyValues->valueInt64;
		case KEYVALUES_DATA_TYPE_FLOAT: return (double)pKeyValues->valueFloat;
		case KEYVALUES_DATA_TYPE_DOUBLE: return pKeyValues->valueDouble;
		case KEYVALUES_DATA_TYPE_STRING: return S_Atod( pKeyValues->valueString.c_str() );
		}
	}

	if ( pbGotDefaultValue )
	{
		*pbGotDefaultValue = true;
	}
	if ( pSchema )
	{
		*pSchema = NULL;
	}
	return defaultValue;
}

/*
==================
CKeyValues::GetString
==================
*/
FORCEINLINE const char* CKeyValues::GetString( const char* pKeyName, const char* pDefaultValue /* = "" */, const char** pSchema /* = NULL */, bool* pbGotDefaultValue /* = NULL */ ) const
{
	CKeyValues* pKeyValues = const_cast<CKeyValues*>( this )->FindKey( pKeyName );
	if ( pKeyValues )
	{
		if ( pbGotDefaultValue )
		{
			*pbGotDefaultValue = false;
		}
		if ( pSchema )
		{
			*pSchema = pKeyValues->HasSchema() ? pKeyValues->valueSchema.c_str() : NULL;
		}

		eastl::string& valueStringRef = (eastl::string&)pKeyValues->valueString;
		switch ( pKeyValues->dataType )
		{
		case KEYVALUES_DATA_TYPE_INT: valueStringRef = S_Sprintf( "%i", pKeyValues->valueInt32 ); break;
		case KEYVALUES_DATA_TYPE_INT64: valueStringRef = S_Sprintf( "%lld", pKeyValues->valueInt64 ); break;
		case KEYVALUES_DATA_TYPE_FLOAT: valueStringRef = S_Sprintf( "%.*g", S_MaxDigits10<float>(), pKeyValues->valueFloat ); break;
		case KEYVALUES_DATA_TYPE_DOUBLE: valueStringRef = S_Sprintf( "%.*g", S_MaxDigits10<double>(), pKeyValues->valueDouble ); break;
		case KEYVALUES_DATA_TYPE_STRING: break;
		}
		return pKeyValues->valueString.c_str();
	}

	if ( pbGotDefaultValue )
	{
		*pbGotDefaultValue = true;
	}
	if ( pSchema )
	{
		*pSchema = NULL;
	}
	return pDefaultValue;
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

/*
==================
CKeyValuesSubKeysIterator::CKeyValuesSubKeysIterator
==================
*/
FORCEINLINE CKeyValuesSubKeysIterator::CKeyValuesSubKeysIterator( CKeyValues* pKeyValues, bool bAllowValues /* = true */, bool bAllowSubKeys /* = false */, bool bAllowEmpty /* = false */ )
	: currentIndex( INVALID_INDEX )
{
	Init( pKeyValues, NULL, bAllowValues, bAllowSubKeys, bAllowEmpty );
}

/*
==================
CKeyValuesSubKeysIterator::CKeyValuesSubKeysIterator
==================
*/
FORCEINLINE CKeyValuesSubKeysIterator::CKeyValuesSubKeysIterator( CKeyValues* pKeyValues, const char* pKeyName, bool bAllowValues /* = true */, bool bAllowSubKeys /* = false */, bool bAllowEmpty /* = false */ )
	: currentIndex( INVALID_INDEX )
{
	Init( pKeyValues, pKeyName, bAllowValues, bAllowSubKeys, bAllowEmpty );
}

/*
==================
CKeyValuesSubKeysIterator::operator++
==================
*/
FORCEINLINE void CKeyValuesSubKeysIterator::operator++()
{
	if ( currentIndex + 1 <= keyValues.size() )
	{
		++currentIndex;
	}
}

/*
==================
CKeyValuesSubKeysIterator::operator+=
==================
*/
FORCEINLINE void CKeyValuesSubKeysIterator::operator+=( uint32 offset )
{
	while ( offset-- )
	{
		operator++();
	}
}

/*
==================
CKeyValuesSubKeysIterator::operator*
==================
*/
FORCEINLINE CKeyValues* CKeyValuesSubKeysIterator::operator*() const
{
	return GetKeyValues();
}

/*
==================
CKeyValuesSubKeysIterator::operator->
==================
*/
FORCEINLINE CKeyValues* CKeyValuesSubKeysIterator::operator->() const
{
	return GetKeyValues();
}

/*
==================
CKeyValuesSubKeysIterator::operator bool
==================
*/
FORCEINLINE CKeyValuesSubKeysIterator::operator bool() const
{
	return currentIndex < keyValues.size();
}

/*
==================
CKeyValuesSubKeysIterator::operator !
==================
*/
FORCEINLINE bool CKeyValuesSubKeysIterator::operator!() const
{
	return !operator bool();
}

/*
==================
CKeyValuesSubKeysIterator::GetKeyValues
==================
*/
FORCEINLINE CKeyValues* CKeyValuesSubKeysIterator::GetKeyValues() const
{
	return !keyValues.empty() ? keyValues[currentIndex] : NULL;
}

/*
==================
CKeyValuesSubKeysReverseIterator::CKeyValuesSubKeysReverseIterator
==================
*/
FORCEINLINE CKeyValuesSubKeysReverseIterator::CKeyValuesSubKeysReverseIterator( CKeyValues* pKeyValues, bool bAllowValues /* = true */, bool bAllowSubKeys /* = false */, bool bAllowEmpty /* = false */ )
	: CKeyValuesSubKeysIterator( pKeyValues, bAllowValues, bAllowSubKeys, bAllowEmpty )
{
	Init();
}

/*
==================
CKeyValuesSubKeysReverseIterator::CKeyValuesSubKeysReverseIterator
==================
*/
FORCEINLINE CKeyValuesSubKeysReverseIterator::CKeyValuesSubKeysReverseIterator( CKeyValues* pKeyValues, const char* pKeyName, bool bAllowValues /* = true */, bool bAllowSubKeys /* = false */, bool bAllowEmpty /* = false */ )
	: CKeyValuesSubKeysIterator( pKeyValues, pKeyName, bAllowValues, bAllowSubKeys, bAllowEmpty )
{
	Init();
}

/*
==================
CKeyValuesSubKeysReverseIterator::Init
==================
*/
FORCEINLINE void CKeyValuesSubKeysReverseIterator::Init()
{
	eastl::reverse( eastl::begin( keyValues ), eastl::end( keyValues ) );
}
