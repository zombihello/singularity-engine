#pragma once
#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier1/strtools.h"

//-----------------------------------------------------------------------------
// Fast hashing function that doesn't require a table lookup for each element
//-----------------------------------------------------------------------------
hash_t FastHash( const void* pData, uint64 size, hash_t hash = 0 );
template<typename TType>
hash_t FastHash( const TType& value, hash_t hash = 0 );

hash_t FastHashFromString( const void* pData, uint64 size, hash_t hash = 0 );
hash_t FastHashFromString( const wchar_t* pString, hash_t hash = 0 );
hash_t FastHashFromString( const eastl::wstring& string, hash_t hash = 0 );
hash_t FastHashFromString( const char* pString, hash_t hash = 0 );
hash_t FastHashFromString( const eastl::string& string, hash_t hash = 0 );

#include "tier1/hashing/fasthash.inl"
