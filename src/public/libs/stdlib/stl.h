#ifndef STL_H
#define STL_H

#include "core/types.h"
#include "core/platform.h"
#include "stdlib/strtools.h"
#include "stdlib/hashing/fasthash.h"

//-----------------------------------------------------------------------------
// Calculate a hash for a string to use it in std::unordered_map
//-----------------------------------------------------------------------------
struct stlInsensitiveStringHash_t
{
	std::size_t operator()( const achar* pString ) const;
};


//-----------------------------------------------------------------------------
// Comparator for std::unordered_map to insensitive compre strings
//-----------------------------------------------------------------------------
struct stlInsensitiveCompareString_t
{
	bool operator()( const achar* pLeft, const achar* pRight ) const;
};

#include "stdlib/stl.inl"

#endif // !STL_H
