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
	size_t operator()( const char* pString ) const;
};

//-----------------------------------------------------------------------------
// Comparator for eastl::unordered_map to insensitive compre strings
//-----------------------------------------------------------------------------
struct stlInsensitiveCompareString_t
{
	bool operator()( const char* pLeft, const char* pRight ) const;
};

#include "tier1/stl.inl"
