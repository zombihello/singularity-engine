#include "pch_inputsystem.h"
#include "inputsystem/inputsystem_internal.h"

#if PLATFORM_USE_SDL
	#include <SDL3/SDL.h>

/*
==================
Plat_SetRelativeMouseMode
==================
*/
void Plat_SetRelativeMouseMode( bool bEnabled )
{
	#if 0
	SDL_SetRelativeMouseMode( bEnabled ? SDL_TRUE : SDL_FALSE );
	#else
	AssertUnimplemented();
	#endif	// 0
}

#endif	// PLATFORM_USE_SDL
