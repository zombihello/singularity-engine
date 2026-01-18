#pragma once
#include "stdlib/defines.h"

//-----------------------------------------------------------------------------
// Windows
//-----------------------------------------------------------------------------
#if PLATFORM_WINDOWS
	#include "studiorender/studioapi/vk/platforms/windows/win_vk_platform.h"
#else
	#error Unknown platform
#endif	// PLATFORM_WINDOWS