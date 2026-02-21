#include "pch_tier0.h"
#include <SDL2/SDL.h>
#include "tier0/tier0_internal.h"
#include "tier0/crashdump_internal.h"
#include "tier0/debug.h"
#include "tier1/threading.h"

bool			  g_bRequestingExit = false;
double			  g_SecondsPerCycle = Sys_GetSecondsPerCycle();
static threadId_t s_MainThreadId	= INVALID_THREAD_ID;

/*
==================
Sys_Error
==================
*/
void Sys_Error( const char* pFormat, ... )
{
	// Don't show message if we already shutdown the game by a critical error
	static bool s_bAlreadyHasError = false;
	if ( s_bAlreadyHasError )
	{
		return;
	}
	s_bAlreadyHasError = true;

	// Get formated string
	va_list params;
	va_start( params, pFormat );
	eastl::string message = S_Vsprintf( pFormat, params );
	va_end( params );

	// Print message and show message box
	Error( message.c_str() );
	Sys_DebugBreak();
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
void Sys_ShowMessageBox( const char* pTitle, const char* pMessage, messageBox_t type )
{
	uint32 sdlFlags = 0;
	switch ( type )
	{
	case MESSAGE_BOX_INFO: sdlFlags = SDL_MESSAGEBOX_INFORMATION; break;
	case MESSAGE_BOX_WARNING: sdlFlags = SDL_MESSAGEBOX_WARNING; break;
	case MESSAGE_BOX_ERROR: sdlFlags = SDL_MESSAGEBOX_ERROR; break;
	default:
		AssertMsg( false, "Unknown message type" );
		break;
	}

	SDL_ShowSimpleMessageBox( sdlFlags, pTitle, pMessage, NULL );
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
 Sys_InitMainThread
 ==================
 */
void Sys_InitMainThread()
{
	CrashDump_OnThreadRun();
	CrashDump_SetupExceptionHandler();
	Thread_SetName( Thread_GetCurrentThreadHandle(), "Main Thread" );
	s_MainThreadId = Thread_GetCurrentThreadId();
}

/*
 ==================
 Sys_IsInMainThread
 ==================
 */
bool Sys_IsInMainThread()
{
	return s_MainThreadId == INVALID_THREAD_ID || s_MainThreadId == Thread_GetCurrentThreadId();
}
