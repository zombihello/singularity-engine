#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>

#include "tier1/stl.h"
#include "tier1/threading.h"

//-----------------------------------------------------------------------------
// String Pool
//-----------------------------------------------------------------------------
template<typename TIdType>
class TStringPool
{
public:
	~TStringPool();

	TIdType		Find( const char* pString, uint64 length ) const;
	TIdType		FindOrAdd( const char* pString, uint64 length );
	const char* GetString( TIdType id ) const;

private:
	eastl::unordered_map<eastl::string_view, TIdType, stlInsensitiveStringHash_t, stlInsensitiveCompareString_t> storageDict;
	eastl::vector<const char*>																					 strings;
	mutable CThreadMutex																						 mutex;
};

#include "tier1/stringpool.inl"
