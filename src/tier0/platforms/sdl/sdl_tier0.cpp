#include "pch_tier0.h"

#if PLATFORM_USE_SDL
	#include <SDL3/SDL.h>

/*
==================
Sys_ShowMessageBox
==================
*/
void Sys_ShowMessageBox( const char* pTitle, const char* pMessage, messageBox_t type )
{
	uint32 sdlFlags = 0;
	switch ( type )
	{
	case MESSAGE_BOX_INFO: sdlFlags = SDL_MESSAGEBOX_INFORMATION; break;
	case MESSAGE_BOX_WARNING: sdlFlags = SDL_MESSAGEBOX_WARNING; break;
	case MESSAGE_BOX_ERROR: sdlFlags = SDL_MESSAGEBOX_ERROR; break;
	default:
		AssertMsg( false, "Unknown message type 0x%X", type );
		break;
	}

	SDL_ShowSimpleMessageBox( sdlFlags, pTitle, pMessage, NULL );
}
#endif	// PLATFORM_USE_SDL
