#ifndef PLATFORM_PRIVATE_H
#define PLATFORM_PRIVATE_H

#include "core/platform.h"

// By default all defines PLATFORM_XXX and PLATFORM_USE_XXX is 0
#define PLATFORM_USE__ALIGNED_MALLOC        0
#define PLATFORM_IS_ANSI_MALLOC_THREADSAFE  0
#define PLATFORM_SUPPORTS_MIMALLOC          0

// Platform specific definitions
#if PLATFORM_WINDOWS				// Windows platform
	#include "core/platforms/windows/win_platform_private.h"
#else								// Unknown platform
	#error Unknown platform
#endif // _WIN32 || _WIN64

#endif // !PLATFORM_PRIVATE_H