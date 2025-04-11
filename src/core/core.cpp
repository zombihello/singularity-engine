#include "pch_core.h"
#include <SDL2/SDL.h>
#include "core/core_private.h"
#include "core/crashdump_private.h"

bool		g_bRequestingExit = false;
double		g_SecondsPerCycle = Sys_GetSecondsPerCycle();
bool		g_bConsoleIOInited = false;

/*
==================
Sys_Error
==================
*/
void Sys_Error( const achar* pFormat, ... )
{
	// Don't show message if we already shutdown the game by a critical error
	static bool		bAlreadyHasError = false;
	if ( bAlreadyHasError )
	{
		return;
	}
	bAlreadyHasError = true;

	// Get formated string
	va_list			params;
	va_start( params, pFormat );
	std::string		message = S_Vsprintf( pFormat, params );
	va_end( params );

	// Print message and show message box
	Error( message.c_str() );
	if ( Sys_IsDebuggerPresent() )
	{
		Sys_DebugBreak();
	}
	Sys_ShowMessageBox( "Singularity Error", message.c_str(), MESSAGE_BOX_ERROR );

	// Set crash dump message
	CrashDump_SetMessage( message.c_str() );

	// Shutdown application
	Sys_RequestExit( true );
}

/*
==================
Sys_ShowMessageBox
==================
*/
void Sys_ShowMessageBox( const achar* pTitle, const achar* pMessage, messageBox_t type )
{
	uint32 		sdlFlags = 0;
	switch ( type )
	{
	case MESSAGE_BOX_INFO: 		sdlFlags = SDL_MESSAGEBOX_INFORMATION; 	break;
	case MESSAGE_BOX_WARNING: 	sdlFlags = SDL_MESSAGEBOX_WARNING; 		break;
	case MESSAGE_BOX_ERROR: 	sdlFlags = SDL_MESSAGEBOX_ERROR; 		break;
	default:
		AssertMsg( false, "Unknown message type" );
		break;
	}

	SDL_ShowSimpleMessageBox( sdlFlags, pTitle, pMessage, nullptr );
}

/*
==================
Sys_IsRequestingExit
==================
*/
bool Sys_IsRequestingExit()
{
	return g_bRequestingExit;
}

/*
==================
Sys_IsInitedConsoleIO
==================
*/
bool Sys_IsInitedConsoleIO()
{
	return g_bConsoleIOInited;
}