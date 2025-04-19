#ifndef PLATFORM_H
#define PLATFORM_H

//-----------------------------------------------------------------------------
// Platforms
//-----------------------------------------------------------------------------
// By default all defines PLATFORM_XXX and PLATFORM_USE_XXX is 0
#define PLATFORM_WINDOWS		            0

// Platform specific definitions
#if _WIN32 || _WIN64						// Windows platform
	#include "core/platforms/windows/win_platform.h"
#else										// Unknown platform
	#error Unknown platform
#endif // _WIN32 || _WIN64

#endif // !PLATFORM_H