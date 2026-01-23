#pragma once
#include "tier1/builddefines.h"

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------
typedef signed char		   int8;
typedef short			   int16;
typedef int				   int32;
typedef long long		   int64;
typedef unsigned char	   uint8;
typedef unsigned short	   uint16;
typedef unsigned int	   uint32;
typedef unsigned long long uint64;
typedef uint8			   byte;
typedef uint64			   hash_t;

#if PLATFORM_64BIT
typedef int64  ptrint;
typedef uint64 uptrint;
#elif PLATFORM_32BIT
typedef int32  ptrint;
typedef uint32 uptrint;
#else
	#error Unknown platform bit
#endif	// PLATFORM_64BIT || PLATFORM_32BIT

enum
{
	INVALID_HASH  = (hash_t)-1,
	INVALID_INDEX = (uint32)-1
};
