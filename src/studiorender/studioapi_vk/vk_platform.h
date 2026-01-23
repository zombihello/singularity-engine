#pragma once
#include "tier0/defines.h"

//-----------------------------------------------------------------------------
// Windows
//-----------------------------------------------------------------------------
#if PLATFORM_WINDOWS
	#include "studiorender/studioapi_vk/platforms/windows/win_vk_platform.h"
#else
	#error Unknown platform
#endif	// PLATFORM_WINDOWS