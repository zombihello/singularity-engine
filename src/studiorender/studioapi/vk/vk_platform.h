#ifndef VK_PLATFORM_H
#define VK_PLATFORM_H

#include "core/platform.h"

//-----------------------------------------------------------------------------
// Windows
//-----------------------------------------------------------------------------
#if PLATFORM_WINDOWS
	#include "studiorender/studioapi/vk/platforms/windows/win_vk_platform.h"
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS

#endif // !VK_PLATFORM_H