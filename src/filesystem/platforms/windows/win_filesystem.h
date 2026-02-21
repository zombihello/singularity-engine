#pragma once
#undef PLATFORM_USE_STD_FILE
#define PLATFORM_USE_STD_FILE 1

#if PLATFORM_WINDOWS
	#include "filesystem/platforms/windows/win_filesystem.inl"
#endif	// PLATFORM_WINDOWS
