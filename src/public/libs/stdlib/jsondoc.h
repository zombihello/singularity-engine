#ifndef JSONDOC_H
#define JSONDOC_H

#include <string>
#include <unordered_map>
#include <vector>
#include <rapidjson/document.h>

#include "core/types.h"
#include "core/platform.h"

//-----------------------------------------------------------------------------
// Forward delcarations
//-----------------------------------------------------------------------------
class CJsonValue;


//-----------------------------------------------------------------------------
// JSON object
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


class CJsonObject
{
public:
	CJsonObject()
	{}
	CJsonObject( const CJsonObject& copy )
		: valuesDict( copy.valuesDict )
	{}
	~CJsonObject()
	{
		Clear();
	}

	void Clear();
	void Copy( const CJsonObject& copy );

	void Set( const rapidjson::Value& value );
	void SetValue( const achar* pName, const CJsonValue& value );
	std::string AsJson( uint32 countTabs = 0 ) const;
	CJsonValue GetValue( const achar* pName ) const;

	CJsonObject& operator=( const CJsonObject& copy );

private:
	std::unordered_map<std::string, class CJsonValue>		valuesDict;
};


//-----------------------------------------------------------------------------
// JSON value
//-----------------------------------------------------------------------------
class CJsonValue
{
public:
	CJsonValue()
		: type( JSONVALUE_TYPE_UNKNOWN )
		, pValue( nullptr )
	{}
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
	void SetString( const std::string& value );
	void SetObject( const CJsonObject& value );
	void SetArray( const std::vector<CJsonValue>& value );
	
	bool IsValid() const;
	bool IsA( jsonValueType_t type ) const;
	bool IsNumber() const;
	std::string	AsJson( uint32 countTabs = 0 ) const;
	jsonValueType_t GetType() const;
	bool GetBool( bool defaultValue = false ) const;
	float GetNumber( float defaultValue = 0.f ) const;
	int32 GetInt( int32 defaultValue = 0 ) const;
	float GetFloat( float defaultValue = 0.f ) const;
	std::string GetString( const std::string& defaultValue = "" ) const;
	CJsonObject GetObject( const CJsonObject& defaultValue = CJsonObject() ) const;
	std::vector<CJsonValue>	GetArray( const std::vector<CJsonValue>& defaultValue = std::vector<CJsonValue>() ) const;

	CJsonValue& operator=( const CJsonValue& copy );

private:
	jsonValueType_t		type;
	void*				pValue;
};


//-----------------------------------------------------------------------------
// JSON document
//-----------------------------------------------------------------------------
class CJsonDoc
{
public:
	// Load and save a JSON document
	// NOTE: For use StdLib must be connected by ConnectStdLib (except LoadFromBuffer and SaveToBuffer)
	bool LoadFromFile( const achar* pPath );
	bool LoadFromBuffer( const achar* pBuffer );
	bool SaveToFile( const achar* pPath );
	bool SaveToBuffer( std::string& buffer );

	void SetValue( const achar* pName, const CJsonValue& value );
	CJsonValue GetValue( const achar* pName ) const;

private:
	typedef std::unordered_map<std::string, CJsonValue>		valuesDict_t;
	valuesDict_t	valuesDict;
};

#include "stdlib/jsondoc.inl"

#endif // !JSONDOC_H