#pragma once
#include "tier0/defines.h"

//-----------------------------------------------------------------------------
// Platform specific internal defines
//-----------------------------------------------------------------------------
#define PLATFORM_USE_SDL 0

#if PLATFORM_WINDOWS
	#include "appframework/platforms/windows/win_appframework.h"
#endif	// PLATFORM_WINDOWS
