#pragma once
#include "stdlib/defines.h"
#include "stdlib/types.h"
#include "stdlib/strtools.h"
#include "stdlib/hashing/fasthash.h"

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

#include "stdlib/stl.inl"
