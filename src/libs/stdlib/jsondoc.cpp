#include "pch_stdlib.h"
#include <sstream>

#include "stdlib/stdlib.h"
#include "stdlib/jsondoc.h"
#include "filesystem/ifilesystem.h"
#include "interfaces/interfaces.h"

/*
==================
CJsonObject::AsJson
==================
*/
std::string CJsonObject::AsJson( uint32 countTabs /* = 0 */ ) const
{
	// Print tabs
	std::stringstream strStream;
	std::string		  tabs;
	for ( uint32 index = 0; index < countTabs; ++index )
	{
		tabs += "\t";
	}

	// Print content
	strStream << "{" << LINE_TERMINATOR;
	for ( auto itValue = valuesDict.begin(), itValueEnd = valuesDict.end(); itValue != itValueEnd; ++itValue )
	{
		strStream << tabs << "\t\"" << itValue->first << "\": " << itValue->second.AsJson( countTabs );
		if ( std::next( itValue ) != itValueEnd )
		{
			strStream << "," << LINE_TERMINATOR;
		}
		else
		{
			strStream << LINE_TERMINATOR;
		}
	}

	strStream << tabs << "}";
	return strStream.str();
}

/*
==================
CJsonObject::Set
==================
*/
void CJsonObject::Set( const rapidjson::Value& value )
{
	// Set values from rapidjson::Value
	for ( auto itValue = value.MemberBegin(), itValueEnd = value.MemberEnd(); itValue != itValueEnd; ++itValue )
	{
		const char* pValueName = itValue->name.GetString();
		CJsonValue	 value;

		value.Set( itValue->value );
		valuesDict[pValueName] = value;
	}
}

/*
==================
CJsonObject::SetValue
==================
*/
void CJsonObject::SetValue( const char* pName, const class CJsonValue& value )
{
	valuesDict[pName] = value;
}

/*
==================
CJsonObject::GetValue
==================
*/
CJsonValue CJsonObject::GetValue( const char* pName ) const
{
	auto itValue = valuesDict.find( pName );
	if ( itValue == valuesDict.end() )
	{
		return CJsonValue();
	}

	return itValue->second;
}

/*
==================
CJsonValue::Clear
==================
*/
void CJsonValue::Clear()
{
	// Do nothing if pValue isn't valid
	if ( !pValue )
	{
		return;
	}

	// Free allocate memory
	switch ( type )
	{
	case JSONVALUE_TYPE_BOOL: delete ( (bool*)pValue ); break;
	case JSONVALUE_TYPE_INT: delete ( (int32*)pValue ); break;
	case JSONVALUE_TYPE_FLOAT: delete ( (float*)pValue ); break;
	case JSONVALUE_TYPE_STRING: delete ( (std::string*)pValue ); break;
	case JSONVALUE_TYPE_OBJECT: delete ( (CJsonObject*)pValue ); break;
	case JSONVALUE_TYPE_ARRAY: delete ( (std::vector<CJsonValue>*)pValue ); break;
	}

	pValue = nullptr;
	type   = JSONVALUE_TYPE_UNKNOWN;
}

/*
==================
CJsonValue::Copy
==================
*/
void CJsonValue::Copy( const CJsonValue& copy )
{
	type = copy.type;
	switch ( type )
	{
	case JSONVALUE_TYPE_BOOL: SetBool( copy.GetBool() ); break;
	case JSONVALUE_TYPE_INT: SetInt( copy.GetInt() ); break;
	case JSONVALUE_TYPE_FLOAT: SetFloat( copy.GetFloat() ); break;
	case JSONVALUE_TYPE_STRING: SetString( copy.GetString() ); break;
	case JSONVALUE_TYPE_OBJECT: SetObject( copy.GetObject() ); break;
	case JSONVALUE_TYPE_ARRAY: SetArray( copy.GetArray() ); break;
	default:
		pValue = nullptr;
		type   = JSONVALUE_TYPE_UNKNOWN;
		break;
	}
}

/*
==================
CJsonValue::AsJson
==================
*/
std::string CJsonValue::AsJson( uint32 countTabs /* = 0 */ ) const
{
	// Convert JSON value to string
	std::stringstream strStream;
	switch ( type )
	{
	case JSONVALUE_TYPE_BOOL: strStream << GetBool() ? "true" : "false"; break;
	case JSONVALUE_TYPE_FLOAT: strStream << GetFloat(); break;
	case JSONVALUE_TYPE_INT: strStream << GetInt(); break;
	case JSONVALUE_TYPE_STRING: strStream << "\"" << GetString() << "\""; break;
	case JSONVALUE_TYPE_OBJECT: strStream << GetObject().AsJson( countTabs + 1 ); break;
	default: strStream << "null"; break;
	}

	return strStream.str();
}

/*
==================
CJsonValue::Set
==================
*/
void CJsonValue::Set( const rapidjson::Value& value )
{
	// Null JSON value
	if ( value.IsNull() )
	{
		return;
	}
	// Bool value
	else if ( value.IsBool() )
	{
		SetBool( value.GetBool() );
	}
	// Integer value
	else if ( value.IsInt() )
	{
		SetInt( value.GetInt() );
	}
	// Float value
	else if ( value.IsFloat() )
	{
		SetFloat( value.GetFloat() );
	}
	// String value
	else if ( value.IsString() )
	{
		SetString( value.GetString() );
	}
	// Object value
	else if ( value.IsObject() )
	{
		CJsonObject object;
		object.Set( value );
		SetObject( object );
	}
	// Array value
	else if ( value.IsArray() )
	{
		auto					array = value.GetArray();
		std::vector<CJsonValue> jsonValues;
		for ( uint32 index = 0, count = array.Size(); index < count; ++index )
		{
			jsonValues.emplace_back().Set( array[index] );
		}

		SetArray( jsonValues );
	}
	else
	{
		AssertMsg( false, "Unknown JSON value type" );
	}
}

/*
==================
CJsonDoc::LoadFromFile
==================
*/
bool CJsonDoc::LoadFromFile( const char* pPath )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Do nothing if file system isn't valid
	Assert( g_pFileSystem );
	if ( !g_pFileSystem )
	{
		return false;
	}

	// Try open file
	TRefPtr<IStreamDataReader> file = g_pFileSystem->CreateFileReader( pPath );
	if ( !file )
	{
		return false;
	}

	// Allocate memory for buffer
	uint64 fileSize = file->GetSize() + 1;
	byte*  pBuffer	= (byte*)Mem_MallocZero( fileSize );

	// Serialize data to string buffer
	file->Read( pBuffer, fileSize );

	// Load data from buffer
	bool bResult = LoadFromBuffer( (const char*)pBuffer );

	// Free allocated memory
	Mem_Free( pBuffer );
	return bResult;
}

/*
==================
CJsonDoc::LoadFromBuffer
==================
*/
bool CJsonDoc::LoadFromBuffer( const char* pBuffer )
{
	// Do nothing if buffer isn't valid
	Assert( pBuffer && pBuffer[0] );
	if ( !pBuffer || !pBuffer[0] )
	{
		return false;
	}

	// Parse JSON with help RapidJSON
	rapidjson::Document jsonDocument;
	jsonDocument.Parse<rapidjson::kParseDefaultFlags | rapidjson::kParseCommentsFlag>( pBuffer );
	if ( jsonDocument.HasParseError() )
	{
		Warning( "CJsonDoc::LoadFromBuffer: Failed to parse JSON document, not correct JSON syntax. RapidJson code error 0x%X", jsonDocument.GetParseError() );
		return false;
	}

	// Read all JSON values
	for ( auto itValue = jsonDocument.MemberBegin(), itValueEnd = jsonDocument.MemberEnd(); itValue != itValueEnd; ++itValue )
	{
		const char* pValueName = itValue->name.GetString();
		CJsonValue*	 pJsonValue = &valuesDict.insert( std::make_pair( pValueName, CJsonValue() ) ).first->second;
		pJsonValue->Set( itValue->value );
	}

	return true;
}

/*
==================
CJsonDoc::SaveToFile
==================
*/
bool CJsonDoc::SaveToFile( const char* path )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Do nothing if file system isn't valid
	Assert( g_pFileSystem );
	if ( !g_pFileSystem )
	{
		return false;
	}

	// Save data to buffer
	std::string buffer;
	if ( !SaveToBuffer( buffer ) )
	{
		Warning( "StdLib: Failed to save JSON document into buffer" );
		return false;
	}

	// Try open file for save
	TRefPtr<IStreamDataWriter> file = g_pFileSystem->CreateFileWriter( path );
	if ( !file )
	{
		Warning( "StdLib: Failed to create a file" );
		return false;
	}

	// Serialize buffer to the file
	file->Write( buffer.data(), buffer.size() * sizeof( char ) );
	return true;
}

/*
==================
CJsonDoc::SaveToBuffer
==================
*/
bool CJsonDoc::SaveToBuffer( std::string& buffer )
{
	buffer += "{";
	buffer += LINE_TERMINATOR;
	for ( valuesDict_t::const_iterator itValue = valuesDict.begin(), itValueEnd = valuesDict.end(); itValue != itValueEnd; ++itValue )
	{
		buffer += S_Sprintf( "\t\"%s\": %s", itValue->first.c_str(), itValue->second.AsJson( 0 ).c_str() );
		if ( std::next( itValue ) != itValueEnd )
		{
			buffer += ",";
			buffer += LINE_TERMINATOR;
		}
		else
		{
			buffer += LINE_TERMINATOR;
		}
	}

	buffer += "}";
	return true;
}