#pragma once
#include <EASTL/string.h>

#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier1/strtools.h"

//-----------------------------------------------------------------------------
// Fast hashing function that doesn't require a table lookup for each element
//-----------------------------------------------------------------------------
hash FastHash( const void* pData, uint64 size, hash hash = 0 );
template<typename TType>
hash FastHash( const TType& data, hash hash = 0 );

template<typename TType>
hash FastHashString( const TType* pString, uint64 length, hash hash = 0 );
template<typename TType>
hash FastHashString( const TType* pString, hash hash = 0 );
template<typename TType>
hash FastHashString( const eastl::basic_string<TType>& string, hash hash = 0 );
template<typename TType>
hash FastHashString( const eastl::basic_string_view<TType>& string, hash hash = 0 );

template<typename TType>
hash FastHashStringInsensitive( const TType* pString, uint64 length, hash hash = 0 );
template<typename TType>
hash FastHashStringInsensitive( const TType* pString, hash hash = 0 );
template<typename TType>
hash FastHashStringInsensitive( const eastl::basic_string<TType>& string, hash hash = 0 );
template<typename TType>
hash FastHashStringInsensitive( const eastl::basic_string_view<TType>& string, hash hash = 0 );

template<typename TType>
hash FastHashItem( const TType& data, hash hash = 0 );

#include "tier1/fasthash.inl"
