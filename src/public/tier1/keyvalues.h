#pragma once
#include <EASTL/list.h>

#include "tier0/types.h"
#include "tier0/assert.h"
#include "tier0/ilogger.h"
#include "tier0/iprofiler.h"
#include "tier1/stringpool.h"
#include "tier1/streamdata_memory.h"
#include "filesystem/ifilesystem.h"
#include "utils/interfaces/interfaces.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CKeyValuesSubKeysIterator;

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
	friend CKeyValuesSubKeysIterator;

	CKeyValues( const char* pName, CKeyValues* pParentKey = NULL );
	~CKeyValues();

	// NOTE: For use Tier1 must be connected by ConnectTier1 (except LoadFromBuffer and SaveToBuffer)
	bool LoadFromFile( const char* pPath );
	bool LoadFromBuffer( const char* pBuffer, uint64 size );
	bool LoadFromStream( IStreamDataReader* pStreamReader );
	bool SaveToFile( const char* pPath ) const;
	void SaveToBuffer( eastl::vector<byte>& buffer ) const;
	void SaveToStream( IStreamDataWriter* pStreamWriter ) const;

	void AddSubKey( CKeyValues* pKeyValue );
	void RemoveSubKey( CKeyValues* pKeyValue, bool bDelete = true );
	void RemoveAllSubKeys( bool bDelete = true );
	void Clear();

	CKeyValues* FindKey( const char* pName, bool bCreate = false );

	void SetName( const char* pName );
	void SetSchema( const char* pSchema );
	void SetBool( const char* pKeyName, bool value, const char* pSchema = NULL );
	void SetInt( const char* pKeyName, int32 value, const char* pSchema = NULL );
	void SetInt64( const char* pKeyName, int64 value, const char* pSchema = NULL );
	void SetFloat( const char* pKeyName, float value, const char* pSchema = NULL );
	void SetDouble( const char* pKeyName, double value, const char* pSchema = NULL );
	void SetString( const char* pKeyName, const char* pValue, const char* pSchema = NULL );

	bool							IsEmpty() const;
	bool							HasData() const;
	bool							HasSubKeys() const;
	bool							HasSchema() const;
	const char*						GetName() const;
	const char*						GetSchema( const char* pKeyName ) const;
	bool							GetBool( const char* pKeyName, bool defaultValue = false, const char** pSchema = NULL, bool* pbGotDefaultValue = NULL ) const;
	int32							GetInt( const char* pKeyName, int32 defaultValue = 0, const char** pSchema = NULL, bool* pbGotDefaultValue = NULL ) const;
	int64							GetInt64( const char* pKeyName, int64 defaultValue = 0, const char** pSchema = NULL, bool* pbGotDefaultValue = NULL ) const;
	float							GetFloat( const char* pKeyName, float defaultValue = 0.f, const char** pSchema = NULL, bool* pbGotDefaultValue = NULL ) const;
	double							GetDouble( const char* pKeyName, double defaultValue = 0.0, const char** pSchema = NULL, bool* pbGotDefaultValue = NULL ) const;
	const char*						GetString( const char* pKeyName, const char* pDefaultValue = "", const char** pSchema = NULL, bool* pbGotDefaultValue = NULL ) const;
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
	eastl::string		valueSchema;
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

//-----------------------------------------------------------------------------
// Class for iterate over all subkeys in a key value
//-----------------------------------------------------------------------------
class CKeyValuesSubKeysIterator
{
public:
	CKeyValuesSubKeysIterator( CKeyValues* pKeyValues, bool bAllowValues = true, bool bAllowSubKeys = false, bool bAllowEmpty = false );
	CKeyValuesSubKeysIterator( CKeyValues* pKeyValues, const char* pKeyName, bool bAllowValues = true, bool bAllowSubKeys = false, bool bAllowEmpty = false );

	void		operator++();
	void		operator+=( uint32 offset );
	CKeyValues* operator*() const;
	CKeyValues* operator->() const;
				operator bool() const;
	bool		operator!() const;

protected:
	CKeyValues* GetKeyValues() const;

	uint32					   currentIndex;
	eastl::vector<CKeyValues*> keyValues;

private:
	void Init( CKeyValues* pKeyValues, const char* pKeyName, bool bAllowValues, bool bAllowSubKeys, bool bAllowEmpty );
};

//-----------------------------------------------------------------------------
// Class for reverse iterate over all subkeys in a key value
//-----------------------------------------------------------------------------
class CKeyValuesSubKeysReverseIterator : public CKeyValuesSubKeysIterator
{
public:
	CKeyValuesSubKeysReverseIterator( CKeyValues* pKeyValues, bool bAllowValues = true, bool bAllowSubKeys = false, bool bAllowEmpty = false );
	CKeyValuesSubKeysReverseIterator( CKeyValues* pKeyValues, const char* pKeyName, bool bAllowValues = true, bool bAllowSubKeys = false, bool bAllowEmpty = false );

private:
	void Init();
};

#include "tier1/keyvalues.inl"
