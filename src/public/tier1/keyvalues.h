#pragma once
#include <EASTL/list.h>

#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier1/stringpool.h"

//-----------------------------------------------------------------------------
// Key values
//-----------------------------------------------------------------------------
enum keyValuesDataType_t
{
	KEYVALUES_DATA_TYPE_NONE,
	KEYVALUES_DATA_TYPE_INT,
	KEYVALUES_DATA_TYPE_INT64,
	KEYVALUES_DATA_TYPE_FLOAT,
	KEYVALUES_DATA_TYPE_DOUBLE,
	KEYVALUES_DATA_TYPE_STRING
};

class CKeyValues
{
public:
	CKeyValues( const char* pName, CKeyValues* pParentKey = NULL );
	CKeyValues( const char* pName, uint32 length, CKeyValues* pParentKey = NULL );
	~CKeyValues();

	// NOTE: For use Tier1 must be connected by ConnectTier1 (except LoadFromBuffer and SaveToBuffer)
	bool LoadFromFile( const char* pPath );
	bool LoadFromBuffer( const char* pBuffer, uint64 size );
	bool SaveToFile( const char* pPath ) const;
	bool SaveToBuffer( eastl::string& buffer ) const;

	void AddSubKey( CKeyValues* pKeyValue );
	void RemoveSubKey( CKeyValues* pKeyValue, bool bDelete = true );
	void RemoveAllSubKeys( bool bDelete = true );
	void Clear();

	CKeyValues* FindKey( const char* pName, bool bCreate = false );

	void SetName( const char* pName );
	void SetName( const char* pName, uint32 length );
	void SetBool( bool value );
	void SetInt( int32 value );
	void SetInt64( int64 value );
	void SetFloat( float value );
	void SetDouble( double value );
	void SetString( const char* pValue );
	void SetString( const char* pValue, uint32 length );

	const char*						GetName() const;
	bool							GetBool( bool defaultValue = false ) const;
	int32							GetInt( int32 defaultValue = 0 ) const;
	int64							GetInt64( int64 defaultValue = 0 ) const;
	float							GetFloat( float defaultValue = 0.f ) const;
	double							GetDouble( double defaultValue = 0.0 ) const;
	const char*						GetString( const char* pDefaultValue = "" ) const;
	keyValuesDataType_t				GetDataType() const;
	CKeyValues*						GetParentKey() const;
	const eastl::list<CKeyValues*>& GetSubKeys() const;

private:
	typedef uint16				  nameID_t;
	typedef TStringPool<nameID_t> namePool_t;

	void DetachFromParent();
	void MigrateNamePool( namePool_t* pNamePool );
	void SetParent( CKeyValues* pParentKey );

	nameID_t			nameID;
	bool				bOwnNamePool;
	keyValuesDataType_t dataType;
	eastl::string		valueString;
	union
	{
		int32  valueInt32;
		int64  valueInt64;
		float  valueFloat;
		double valueDouble;
	};
	CKeyValues*				 pParentKey;
	namePool_t*				 pNamePool;
	eastl::list<CKeyValues*> subKeys;
};

#include "tier1/keyvalues.inl"
