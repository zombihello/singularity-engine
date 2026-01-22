#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>
#include <rapidjson/document.h>

#include "stdlib/defines.h"
#include "stdlib/types.h"

//-----------------------------------------------------------------------------
// Forward delcarations
//-----------------------------------------------------------------------------
class CJsonObject;

//-----------------------------------------------------------------------------
// JSON value
//-----------------------------------------------------------------------------
enum jsonValueType_t
{
	JSONVALUE_TYPE_UNKNOWN,
	JSONVALUE_TYPE_BOOL,
	JSONVALUE_TYPE_INT,
	JSONVALUE_TYPE_FLOAT,
	JSONVALUE_TYPE_STRING,
	JSONVALUE_TYPE_OBJECT,
	JSONVALUE_TYPE_ARRAY
};

class CJsonValue
{
public:
	CJsonValue()
		: type( JSONVALUE_TYPE_UNKNOWN )
		, pValue( nullptr )
	{
	}
	CJsonValue( const CJsonValue& copy )
		: type( JSONVALUE_TYPE_UNKNOWN )
		, pValue( nullptr )
	{
		Copy( copy );
	}
	~CJsonValue()
	{
		Clear();
	}

	void Clear();
	void Copy( const CJsonValue& copy );

	void Set( const rapidjson::Value& value );
	void SetBool( bool value );
	void SetInt( int32 value );
	void SetFloat( float value );
	void SetString( const eastl::string& value );
	void SetObject( const CJsonObject& value );
	void SetArray( const eastl::vector<CJsonValue>& value );

	bool					  IsValid() const;
	bool					  IsA( jsonValueType_t type ) const;
	bool					  IsNumber() const;
	eastl::string			  AsJson( uint32 countTabs = 0 ) const;
	jsonValueType_t			  GetType() const;
	bool					  GetBool() const;
	float					  GetNumber() const;
	int32					  GetInt() const;
	float					  GetFloat() const;
	eastl::string			  GetString() const;
	CJsonObject				  GetObject() const;
	eastl::vector<CJsonValue> GetArray() const;

	CJsonValue& operator=( const CJsonValue& copy );

private:
	jsonValueType_t type;
	void*			pValue;
};

//-----------------------------------------------------------------------------
// JSON object
//-----------------------------------------------------------------------------
class CJsonObject
{
public:
	CJsonObject()
	{
	}
	CJsonObject( const CJsonObject& copy )
		: valuesDict( copy.valuesDict )
	{
	}
	~CJsonObject()
	{
		Clear();
	}

	void Clear();
	void Copy( const CJsonObject& copy );

	void		  Set( const rapidjson::Value& value );
	void		  SetValue( const char* pName, const CJsonValue& value );
	eastl::string AsJson( uint32 countTabs = 0 ) const;
	CJsonValue	  GetValue( const char* pName ) const;

	CJsonObject& operator=( const CJsonObject& copy );

private:
	eastl::unordered_map<eastl::string, CJsonValue> valuesDict;
};

//-----------------------------------------------------------------------------
// JSON document
//-----------------------------------------------------------------------------
class CJsonDoc
{
public:
	// Load and save a JSON document
	// NOTE: For use StdLib must be connected by ConnectStdLib (except LoadFromBuffer and SaveToBuffer)
	bool LoadFromFile( const char* pPath );
	bool LoadFromBuffer( const char* pBuffer );
	bool SaveToFile( const char* pPath );
	bool SaveToBuffer( eastl::string& buffer );

	void	   SetValue( const char* pName, const CJsonValue& value );
	CJsonValue GetValue( const char* pName ) const;

private:
	typedef eastl::unordered_map<eastl::string, CJsonValue> valuesDict_t;
	valuesDict_t											valuesDict;
};

#include "stdlib/jsondoc.inl"
