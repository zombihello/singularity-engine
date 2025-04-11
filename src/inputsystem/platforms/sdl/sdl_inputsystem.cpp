#include "pch_inputsystem.h"
#include "inputsystem/inputsystem_private.h"

#include <SDL2/SDL.h>

/*
==================
Sys_SetRelativeMouseMode
==================
*/
void Sys_SetRelativeMouseMode( bool bEnabled )
{
	SDL_SetRelativeMouseMode( bEnabled ? SDL_TRUE : SDL_FALSE );
}