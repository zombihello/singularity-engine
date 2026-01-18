#pragma once

/*
==================
CJsonObject::Clear
==================
*/
FORCEINLINE void CJsonObject::Clear()
{
	valuesDict.clear();
}

/*
==================
CJsonObject::Copy
==================
*/
FORCEINLINE void CJsonObject::Copy( const CJsonObject& copy )
{
	valuesDict = copy.valuesDict;
}

/*
==================
CJsonObject::operator=
==================
*/
FORCEINLINE CJsonObject& CJsonObject::operator=( const CJsonObject& copy )
{
	Copy( copy );
	return *this;
}

/*
==================
CJsonValue::IsValid
==================
*/
FORCEINLINE bool CJsonValue::IsValid() const
{
	return type != JSONVALUE_TYPE_UNKNOWN;
}

/*
==================
CJsonValue::IsA
==================
*/
FORCEINLINE bool CJsonValue::IsA( jsonValueType_t type ) const
{
	return CJsonValue::type == type;
}

/*
==================
CJsonValue::IsNumber
==================
*/
FORCEINLINE bool CJsonValue::IsNumber() const
{
	return type == JSONVALUE_TYPE_BOOL || type == JSONVALUE_TYPE_INT || type == JSONVALUE_TYPE_FLOAT;
}

/*
==================
CJsonValue::SetBool
==================
*/
FORCEINLINE void CJsonValue::SetBool( bool value )
{
	if ( type != JSONVALUE_TYPE_BOOL )
	{
		Clear();
	}

	if ( !CJsonValue::pValue )
	{
		CJsonValue::pValue = new bool;
	}

	( *(bool*)CJsonValue::pValue ) = value;
	type						   = JSONVALUE_TYPE_BOOL;
}

/*
==================
CJsonValue::SetInt
==================
*/
FORCEINLINE void CJsonValue::SetInt( int32 value )
{
	if ( type != JSONVALUE_TYPE_INT )
	{
		Clear();
	}

	if ( !CJsonValue::pValue )
	{
		CJsonValue::pValue = new int32;
	}

	( *(int32*)CJsonValue::pValue ) = value;
	type							= JSONVALUE_TYPE_INT;
}

/*
==================
CJsonValue::SetFloat
==================
*/
FORCEINLINE void CJsonValue::SetFloat( float value )
{
	if ( type != JSONVALUE_TYPE_FLOAT )
	{
		Clear();
	}

	if ( !CJsonValue::pValue )
	{
		CJsonValue::pValue = new float;
	}

	( *(float*)CJsonValue::pValue ) = value;
	type							= JSONVALUE_TYPE_FLOAT;
}

/*
==================
CJsonValue::SetString
==================
*/
FORCEINLINE void CJsonValue::SetString( const std::string& value )
{
	if ( type != JSONVALUE_TYPE_STRING )
	{
		Clear();
	}

	if ( !CJsonValue::pValue )
	{
		CJsonValue::pValue = new std::string();
	}

	( *(std::string*)CJsonValue::pValue ) = value;
	type								  = JSONVALUE_TYPE_STRING;
}

/*
==================
CJsonValue::SetObject
==================
*/
FORCEINLINE void CJsonValue::SetObject( const CJsonObject& value )
{
	if ( type != JSONVALUE_TYPE_OBJECT )
	{
		Clear();
	}

	if ( !CJsonValue::pValue )
	{
		CJsonValue::pValue = new CJsonObject();
	}

	( *(CJsonObject*)CJsonValue::pValue ) = value;
	type								  = JSONVALUE_TYPE_OBJECT;
}

/*
==================
CJsonValue::SetArray
==================
*/
FORCEINLINE void CJsonValue::SetArray( const std::vector<CJsonValue>& value )
{
	if ( type != JSONVALUE_TYPE_ARRAY )
	{
		Clear();
	}

	if ( !CJsonValue::pValue )
	{
		CJsonValue::pValue = new std::vector<CJsonValue>();
	}

	( *(std::vector<CJsonValue>*)CJsonValue::pValue ) = value;
	type											  = JSONVALUE_TYPE_ARRAY;
}

/*
==================
CJsonValue::GetType
==================
*/
FORCEINLINE jsonValueType_t CJsonValue::GetType() const
{
	return type;
}

/*
==================
CJsonValue::GetBool
==================
*/
FORCEINLINE bool CJsonValue::GetBool( bool defaultValue /*= false*/ ) const
{
	if ( type != JSONVALUE_TYPE_BOOL || !pValue )
	{
		return defaultValue;
	}

	return *(bool*)pValue;
}

/*
==================
CJsonValue::GetNumber
==================
*/
FORCEINLINE float CJsonValue::GetNumber( float defaultValue /*= 0.f*/ ) const
{
	if ( type != JSONVALUE_TYPE_BOOL && type != JSONVALUE_TYPE_INT && type != JSONVALUE_TYPE_FLOAT || !pValue )
	{
		return defaultValue;
	}

	if ( type == JSONVALUE_TYPE_BOOL )
	{
		return (float)GetBool();
	}
	else if ( type == JSONVALUE_TYPE_INT )
	{
		return (float)GetInt();
	}
	else
	{
		return GetFloat();
	}
}

/*
==================
CJsonValue::GetInt
==================
*/
FORCEINLINE int32 CJsonValue::GetInt( int32 defaultValue /*= 0*/ ) const
{
	if ( type != JSONVALUE_TYPE_INT || !pValue )
	{
		return defaultValue;
	}

	return *(int32*)pValue;
}

/*
==================
CJsonValue::GetFloat
==================
*/
FORCEINLINE float CJsonValue::GetFloat( float defaultValue /*= 0.f*/ ) const
{
	if ( type != JSONVALUE_TYPE_FLOAT || !pValue )
	{
		return defaultValue;
	}

	return *(float*)pValue;
}

/*
==================
CJsonValue::GetString
==================
*/
FORCEINLINE std::string CJsonValue::GetString( const std::string& defaultValue /*= ""*/ ) const
{
	if ( type != JSONVALUE_TYPE_STRING || !pValue )
	{
		return defaultValue;
	}

	return *(std::string*)pValue;
}

/*
==================
CJsonValue::GetObject
==================
*/
FORCEINLINE CJsonObject CJsonValue::GetObject( const CJsonObject& defaultValue /*= CJsonObject()*/ ) const
{
	if ( type != JSONVALUE_TYPE_OBJECT || !pValue )
	{
		return defaultValue;
	}

	return *(CJsonObject*)pValue;
}

/*
==================
CJsonValue::GetArray
==================
*/
FORCEINLINE std::vector<CJsonValue> CJsonValue::GetArray( const std::vector<CJsonValue>& defaultValue /*= std::vector<CJsonValue>()*/ ) const
{
	if ( type != JSONVALUE_TYPE_ARRAY || !pValue )
	{
		return defaultValue;
	}

	return *(std::vector<CJsonValue>*)pValue;
}

/*
==================
CJsonValue::operator=
==================
*/
FORCEINLINE CJsonValue& CJsonValue::operator=( const CJsonValue& copy )
{
	Copy( copy );
	return *this;
}

/*
==================
CJsonDoc::SetValue
==================
*/
FORCEINLINE void CJsonDoc::SetValue( const char* pName, const CJsonValue& value )
{
	valuesDict[pName] = value;
}

/*
==================
CJsonDoc::GetValue
==================
*/
FORCEINLINE CJsonValue CJsonDoc::GetValue( const char* pName ) const
{
	valuesDict_t::const_iterator itValues = valuesDict.find( pName );
	if ( itValues == valuesDict.end() )
	{
		return CJsonValue();
	}
	return itValues->second;
}
