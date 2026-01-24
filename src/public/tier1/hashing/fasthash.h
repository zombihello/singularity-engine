#pragma once
#include <EASTL/string.h>

#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier1/strtools.h"

//-----------------------------------------------------------------------------
// Fast hashing function that doesn't require a table lookup for each element
//-----------------------------------------------------------------------------
hash_t FastHash( const void* pData, uint64 size, hash_t hash = 0 );
template<typename TType>
hash_t FastHash( const TType& data, hash_t hash = 0 );

template<typename TType>
hash_t FastHashString( const TType* pString, uint64 length, hash_t hash = 0 );
template<typename TType>
hash_t FastHashString( const TType* pString, hash_t hash = 0 );
template<typename TType>
hash_t FastHashString( const eastl::basic_string<TType>& string, hash_t hash = 0 );
template<typename TType>
hash_t FastHashString( const eastl::basic_string_view<TType>& string, hash_t hash = 0 );

template<typename TType>
hash_t FastHashStringInsensitive( const TType* pString, uint64 length, hash_t hash = 0 );
template<typename TType>
hash_t FastHashStringInsensitive( const TType* pString, hash_t hash = 0 );
template<typename TType>
hash_t FastHashStringInsensitive( const eastl::basic_string<TType>& string, hash_t hash = 0 );
template<typename TType>
hash_t FastHashStringInsensitive( const eastl::basic_string_view<TType>& string, hash_t hash = 0 );

template<typename TType>
hash_t FastHashItem( const TType& data, hash_t hash = 0 );

#include "tier1/hashing/fasthash.inl"
