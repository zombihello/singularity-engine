#include "pch_appframework.h"
#include "appframework/appframework_internal.h"

#if PLATFORM_USE_SDL
	#include <EASTL/unordered_map.h>

	#include "appframework/platforms/sdl/sdl_windowmgr.h"
	#include "appframework/platforms/sdl/sdl_window.h"

	#define DEFAULT_WINDOW_WIDTH  1280
	#define DEFAULT_WINDOW_HEIGHT 720

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
		Warning( "WindowMgrSDL: Window already created (ptr: 0x%p)", this );
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

	// Get a display and resolve window size if it need
	uint32 width			= createInfo.width;
	uint32 height			= createInfo.height;
	bool   bSizeFromDisplay = width == WINDOW_SIZE_FROM_DISPLAY || height == WINDOW_SIZE_FROM_DISPLAY;
	if ( bSizeFromDisplay )
	{
		display_t	   display;
		CWindowMgrSDL& windowMgrSDL	 = GetWindowMgrSDL();
		bool		   bValidDisplay = ( createInfo.displayHandle != INVALID_DISPLAY_HANDLE && windowMgrSDL.GetDisplayByHandle( createInfo.displayHandle, display ) ) || windowMgrSDL.GetPrimaryDisplay( display );
		if ( !bValidDisplay )
		{
			Warning( "WindowMgrSDL: Couldn't resolve display, using default window size %ix%i (displayHandle: 0x%X, ptr: 0x%p)", DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, createInfo.displayHandle, this );
		}

		if ( width == WINDOW_SIZE_FROM_DISPLAY )
		{
			width = bValidDisplay ? display.bounds.width : DEFAULT_WINDOW_WIDTH;
		}
		if ( height == WINDOW_SIZE_FROM_DISPLAY )
		{
			height = bValidDisplay ? display.bounds.height : DEFAULT_WINDOW_HEIGHT;
		}
	}

	// Create the window
	pSDLWindow = SDL_CreateWindow( createInfo.pTitle, width, height, sdlFlags );
	if ( !pSDLWindow )
	{
		Warning( "WindowMgrSDL: Couldn't create window (width: %i, height: %i, title: %s, mode: 0x%X, ptr: 0x%p): %s", width, height, createInfo.pTitle, createInfo.mode, this, SDL_GetError() );
		return false;
	}

	refreshRate = createInfo.refreshRate;
	SetDisplay( createInfo.displayHandle );
	SetMode( createInfo.mode );

	// Get OS handle on the window
	#if PLATFORM_WINDOWS
	handle = (windowHandle_t)SDL_GetPointerProperty( SDL_GetWindowProperties( pSDLWindow ), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL );
	#else
		#error Unknown platform
	#endif	// PLATFORM_WINDOWS

	Assert( handle );
	Msg( "WindowMgrSDL: Window created (width: %i, height: %i, title: %s, mode: 0x%X, ptr: 0x%p)", width, height, createInfo.pTitle, createInfo.mode, this );

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
		Msg( "WindowMgrSDL: Window closed (ptr: 0x%p)", this );

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
			refreshRate = sdlDisplayMode.refresh_rate;
			SDL_SetWindowFullscreenMode( pSDLWindow, &sdlDisplayMode );
			Msg( "WindowMgrSDL: Set display mode %ix%i@%fHz (ptr: 0x%p)", size.x, size.y, refreshRate, this );
			break;
		}
		else
		{
			Warning( "WindowMgrSDL: Couldn't find closest display mode to %ix%i@%fHz (ptr: 0x%p)", size.x, size.y, refreshRate, this );
		}
	}

	case WINDOW_MODE_BORDERLESS_FULLSCREEN:
		Msg( "WindowMgrSDL: Set borderless fullscreen mode (ptr: 0x%p)", this );
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
