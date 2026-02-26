#include "pch_appframework.h"
#include "appframework/appframework_internal.h"

#if PLATFORM_USE_SDL
	#include <EASTL/unordered_map.h>
	#include "appframework/platforms/sdl/sdl_window.h"

static eastl::unordered_map<SDL_WindowID, windowId_t> s_sdlWindowIdDict;

/*
==================
CWindowSDL::CWindowSDL
==================
*/
CWindowSDL::CWindowSDL( windowId_t id )
	: refreshRate( 0.f )
	, id( id )
	, pSDLWindow( NULL )
	, handle( INVALID_WINDOW_HANDLE )
	, mode( WINDOW_MODE_HIDDEN )
{
}

/*
==================
CWindowSDL::~CWindowSDL
==================
*/
CWindowSDL::~CWindowSDL()
{
	Close();
}

/*
==================
CWindowSDL::Create
==================
*/
bool CWindowSDL::Create( const windowCreateInfo_t& createInfo )
{
	// Do nothing if we already create the window
	if ( pSDLWindow )
	{
		Warning( "WindowMgrSDL: Window already created" );
		return true;
	}

	uint32 sdlFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
	switch ( createInfo.mode )
	{
	case WINDOW_MODE_HIDDEN:
		sdlFlags |= SDL_WINDOW_HIDDEN;
		break;

	case WINDOW_MODE_BORDERLESS_FULLSCREEN:
	case WINDOW_MODE_EXCLUSIVE_FULLSCREEN:
		sdlFlags |= SDL_WINDOW_FULLSCREEN;
		break;
	}

	// Create the window
	pSDLWindow = SDL_CreateWindow( createInfo.pTitle, createInfo.width, createInfo.height, sdlFlags );
	if ( !pSDLWindow )
	{
		Warning( "WindowMgrSDL: Failed to create window (%ix%i) with title '%s' in mode 0x%X. SDL error: %s", createInfo.width, createInfo.height, createInfo.pTitle, createInfo.mode, SDL_GetError() );
		return false;
	}

	refreshRate = createInfo.refreshRate;
	if ( createInfo.pDisplay )
	{
		SetDisplay( *createInfo.pDisplay );
	}
	SetMode( createInfo.mode );

	// Get OS handle on the window
	#if PLATFORM_WINDOWS
	handle = (windowHandle_t)SDL_GetPointerProperty( SDL_GetWindowProperties( pSDLWindow ), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL );
	#else
		#error Unknown platform
	#endif	// PLATFORM_WINDOWS

	Assert( handle );
	Msg( "WindowMgrSDL: Window created (%ix%i) with title '%s' in mode 0x%X, handle 0x%p", createInfo.width, createInfo.height, createInfo.pTitle, createInfo.mode, handle );

	SDL_WindowID sdlWindowId = SDL_GetWindowID( pSDLWindow );
	Assert( s_sdlWindowIdDict.find( sdlWindowId ) == s_sdlWindowIdDict.end() );
	s_sdlWindowIdDict[sdlWindowId] = id;
	return true;
}

/*
==================
CWindowSDL::Close
==================
*/
void CWindowSDL::Close()
{
	if ( pSDLWindow )
	{
		s_sdlWindowIdDict.erase( SDL_GetWindowID( pSDLWindow ) );
		SDL_DestroyWindow( pSDLWindow );
		Msg( "WindowMgrSDL: Window with handle 0x%p closed", handle );

		refreshRate = 0.f;
		pSDLWindow	= NULL;
		handle		= INVALID_WINDOW_HANDLE;
		mode		= WINDOW_MODE_HIDDEN;
	}
}

/*
==================
CWindowSDL::SetMode
==================
*/
void CWindowSDL::SetMode( windowMode_t mode )
{
	Assert( pSDLWindow );
	CWindowSDL::mode = mode;
	switch ( mode )
	{
	case WINDOW_MODE_WINDOWED:
		SDL_SetWindowFullscreen( pSDLWindow, false );
		break;

	case WINDOW_MODE_BORDERLESS_FULLSCREEN:
	case WINDOW_MODE_EXCLUSIVE_FULLSCREEN:
		UpdateDisplayMode();
		SDL_SetWindowFullscreen( pSDLWindow, true );
		break;
	}

	Show( mode != WINDOW_MODE_HIDDEN );
}

/*
==================
CWindowSDL::UpdateDisplayMode
==================
*/
void CWindowSDL::UpdateDisplayMode()
{
	Assert( pSDLWindow );
	switch ( mode )
	{
	case WINDOW_MODE_EXCLUSIVE_FULLSCREEN:
	{
		SDL_DisplayMode sdlDisplayMode;
		ivec2_t			size = IWindow::GetSizeInPixels();
		if ( SDL_GetClosestFullscreenDisplayMode( SDL_GetDisplayForWindow( pSDLWindow ), size.x, size.y, refreshRate, true, &sdlDisplayMode ) )
		{
			SDL_SetWindowFullscreenMode( pSDLWindow, &sdlDisplayMode );
			refreshRate = sdlDisplayMode.refresh_rate;
		}
		else
		{
			SDL_SetWindowFullscreenMode( pSDLWindow, NULL );
		}
		break;
	}

	case WINDOW_MODE_BORDERLESS_FULLSCREEN:
		SDL_SetWindowFullscreenMode( pSDLWindow, NULL );
		break;
	}
}

/*
==================
CWindowSDL::GetIdBySDLWindowId
==================
*/
windowId_t CWindowSDL::GetIdBySDLWindowId( SDL_WindowID sdlWindowId )
{
	auto it = s_sdlWindowIdDict.find( sdlWindowId );
	return it != s_sdlWindowIdDict.end() ? it->second : INVALID_INDEX;
}
#endif	// PLATFORM_USE_SDL
