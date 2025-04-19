#ifndef FASTHASH_H
#define FASTHASH_H

#include "core/types.h"
#include "core/platform.h"
#include "stdlib/strtools.h"

//-----------------------------------------------------------------------------
// Fast hashing function that doesn't require a table lookup for each element
//-----------------------------------------------------------------------------
hash_t FastHash( const void* pData, uint64 size, hash_t hash = 0 );
template<typename TType>
hash_t FastHash( const TType& value, hash_t hash = 0 );

hash_t FastHashFromString( const void* pData, uint64 size, hash_t hash = 0 );
hash_t FastHashFromString( const wchar* pString, hash_t hash = 0 );
hash_t FastHashFromString( const std::wstring& string, hash_t hash = 0 );
hash_t FastHashFromString( const achar* pString, hash_t hash = 0 );
hash_t FastHashFromString( const std::string& string, hash_t hash = 0 );

#include "stdlib/hashing/fasthash.inl"

#endif // !FASTHASH_H