#pragma once
#include "tier0/defines.h"
#include "tier0/types.h"

//-----------------------------------------------------------------------------
// Platform specific internal defines
//-----------------------------------------------------------------------------
#define PLATFORM_USE_SDL 0

//-----------------------------------------------------------------------------
// Platform specific functions
//-----------------------------------------------------------------------------
// While the mouse is in relative mode, the cursor is hidden, and the
// driver will try to report continuous motion in the current window.
// Only relative motion events will be delivered, the mouse position
// will not change.
void Plat_SetRelativeMouseMode( bool bEnabled );

#if PLATFORM_WINDOWS
	#include "inputsystem/platforms/windows/win_inputsystem.h"
#endif	// PLATFORM_WINDOWS
