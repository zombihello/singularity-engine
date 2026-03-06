#pragma once
#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier1/strtools.h"
#include "tier1/hashing/fasthash.h"

//-----------------------------------------------------------------------------
// Calculate a hash for a string to use it in eastl::unordered_map
//-----------------------------------------------------------------------------
struct stlInsensitiveStringHash_t
{
	template<typename TType>
	size operator()( const TType* pString ) const;
	template<typename TType>
	size operator()( const eastl::basic_string<TType>& string ) const;
	template<typename TType>
	size operator()( const eastl::basic_string_view<TType>& string ) const;
};

//-----------------------------------------------------------------------------
// Comparator for eastl::unordered_map to insensitive compre strings
//-----------------------------------------------------------------------------
struct stlInsensitiveCompareString_t
{
	template<typename TType>
	bool operator()( const TType* pLeft, const TType* pRight ) const;
	template<typename TType>
	bool operator()( const eastl::basic_string<TType>& left, const eastl::basic_string<TType>& right ) const;
	template<typename TType>
	bool operator()( const eastl::basic_string_view<TType>& left, const eastl::basic_string_view<TType>& right ) const;
};

#include "tier1/stl.inl"
