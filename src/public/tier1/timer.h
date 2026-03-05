#pragma once
#include "tier0/defines.h"

//-----------------------------------------------------------------------------
// Timer
//-----------------------------------------------------------------------------
#if PLATFORM_WINDOWS
	#include "tier1/platforms/windows/win_timer.h"
typedef CTimerWindows CTimer;
#else
	#error Unknown platform
#endif	// PLATFORM_WINDOWS
