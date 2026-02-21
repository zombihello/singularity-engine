#pragma once
#include "tier0/defines.h"

//-----------------------------------------------------------------------------
// Platform specific types
//-----------------------------------------------------------------------------
#if PLATFORM_WINDOWS
	#include "tier0/platforms/windows/win_types.h"
#else
	#error Unknown platform
#endif	// PLATFORM_WINDOWS

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------
typedef uint8	  byte;
typedef uint64	  hash;
typedef size_t	  size;
typedef ptrdiff_t ptrdiff;
typedef intptr_t  ptrint;
typedef uintptr_t uptrint;

enum
{
	INVALID_HASH   = -1,
	INVALID_INDEX  = -1,
	INVALID_HANDLE = -1
};
