#include "pch_appframework.h"
#include "appframework/appframework_internal.h"

#if PLATFORM_USE_SDL
	#include <SDL3/SDL.h>

	#include "tier0/profile.h"
	#include "appframework/platforms/sdl/sdl_windowmgr.h"

// Table for convert SDL scancode to engine button code
static SDL_Scancode s_scanCodeTable[] = {
	SDL_SCANCODE_UNKNOWN,  // BUTTON_CODE_NONE
	SDL_SCANCODE_0,		   // KEY_0
	SDL_SCANCODE_1,		   // KEY_1
	SDL_SCANCODE_2,		   // KEY_2
	SDL_SCANCODE_3,		   // KEY_3
	SDL_SCANCODE_4,		   // KEY_4
	SDL_SCANCODE_5,		   // KEY_5
	SDL_SCANCODE_6,		   // KEY_6
	SDL_SCANCODE_7,		   // KEY_7
	SDL_SCANCODE_8,		   // KEY_8
	SDL_SCANCODE_9,		   // KEY_9

	SDL_SCANCODE_A,	 // KEY_A
	SDL_SCANCODE_B,	 // KEY_B
	SDL_SCANCODE_C,	 // KEY_C
	SDL_SCANCODE_D,	 // KEY_D
	SDL_SCANCODE_E,	 // KEY_E
	SDL_SCANCODE_F,	 // KEY_F
	SDL_SCANCODE_G,	 // KEY_G
	SDL_SCANCODE_H,	 // KEY_H
	SDL_SCANCODE_I,	 // KEY_I
	SDL_SCANCODE_J,	 // KEY_J
	SDL_SCANCODE_K,	 // KEY_K
	SDL_SCANCODE_L,	 // KEY_L
	SDL_SCANCODE_M,	 // KEY_M
	SDL_SCANCODE_N,	 // KEY_N
	SDL_SCANCODE_O,	 // KEY_O
	SDL_SCANCODE_P,	 // KEY_P
	SDL_SCANCODE_Q,	 // KEY_Q
	SDL_SCANCODE_R,	 // KEY_R
	SDL_SCANCODE_S,	 // KEY_S
	SDL_SCANCODE_T,	 // KEY_T
	SDL_SCANCODE_U,	 // KEY_U
	SDL_SCANCODE_V,	 // KEY_V
	SDL_SCANCODE_W,	 // KEY_W
	SDL_SCANCODE_X,	 // KEY_X
	SDL_SCANCODE_Y,	 // KEY_Y
	SDL_SCANCODE_Z,	 // KEY_Z

	SDL_SCANCODE_KP_0,		   // KEYPAD_0
	SDL_SCANCODE_KP_1,		   // KEYPAD_1
	SDL_SCANCODE_KP_2,		   // KEYPAD_2
	SDL_SCANCODE_KP_3,		   // KEYPAD_3
	SDL_SCANCODE_KP_4,		   // KEYPAD_4
	SDL_SCANCODE_KP_5,		   // KEYPAD_5
	SDL_SCANCODE_KP_6,		   // KEYPAD_6
	SDL_SCANCODE_KP_7,		   // KEYPAD_7
	SDL_SCANCODE_KP_8,		   // KEYPAD_8
	SDL_SCANCODE_KP_9,		   // KEYPAD_9
	SDL_SCANCODE_KP_DIVIDE,	   // KEYPAD_DIVIDE
	SDL_SCANCODE_KP_MULTIPLY,  // KEYPAD_MULTIPLY
	SDL_SCANCODE_KP_MINUS,	   // KEYPAD_MINUS
	SDL_SCANCODE_KP_PLUS,	   // KEYPAD_PLUS
	SDL_SCANCODE_KP_ENTER,	   // KEYPAD_ENTER
	SDL_SCANCODE_KP_DECIMAL,   // KEYPAD_DECIMAL

	SDL_SCANCODE_LEFTBRACKET,	// KEY_LBRACKET
	SDL_SCANCODE_RIGHTBRACKET,	// KEY_RBRACKET
	SDL_SCANCODE_SEMICOLON,		// KEY_SEMICOLON
	SDL_SCANCODE_APOSTROPHE,	// KEY_APOSTROPHE
	SDL_SCANCODE_GRAVE,			// KEY_BACKQUOTE
	SDL_SCANCODE_COMMA,			// KEY_COMMA
	SDL_SCANCODE_PERIOD,		// KEY_PERIOD
	SDL_SCANCODE_SLASH,			// KEY_SLASH
	SDL_SCANCODE_BACKSLASH,		// KEY_BACKSLASH
	SDL_SCANCODE_MINUS,			// KEY_MINUS
	SDL_SCANCODE_EQUALS,		// KEY_EQUAL
	SDL_SCANCODE_RETURN,		// KEY_ENTER
	SDL_SCANCODE_SPACE,			// KEY_SPACE
	SDL_SCANCODE_BACKSPACE,		// KEY_BACKSPACE
	SDL_SCANCODE_TAB,			// KEY_TAB
	SDL_SCANCODE_CAPSLOCK,		// KEY_CAPSLOCK
	SDL_SCANCODE_NUMLOCKCLEAR,	// KEY_NUMLOCK
	SDL_SCANCODE_ESCAPE,		// KEY_ESCAPE
	SDL_SCANCODE_SCROLLLOCK,	// KEY_SCROLLLOCK
	SDL_SCANCODE_INSERT,		// KEY_INSERT
	SDL_SCANCODE_DELETE,		// KEY_DELETE
	SDL_SCANCODE_HOME,			// KEY_HOME
	SDL_SCANCODE_END,			// KEY_END
	SDL_SCANCODE_PAGEUP,		// KEY_PAGEUP
	SDL_SCANCODE_PAGEDOWN,		// KEY_PAGEDOWN
	SDL_SCANCODE_PAUSE,			// KEY_BREAK
	SDL_SCANCODE_LSHIFT,		// KEY_LSHIFT
	SDL_SCANCODE_RSHIFT,		// KEY_RSHIFT
	SDL_SCANCODE_LALT,			// KEY_LALT
	SDL_SCANCODE_RALT,			// KEY_RALT
	SDL_SCANCODE_LCTRL,			// KEY_LCONTROL
	SDL_SCANCODE_RCTRL,			// KEY_RCONTROL
	SDL_SCANCODE_LGUI,			// KEY_LWIN
	SDL_SCANCODE_RGUI,			// KEY_RWIN
	SDL_SCANCODE_APPLICATION,	// KEY_APP
	SDL_SCANCODE_UP,			// KEY_UP
	SDL_SCANCODE_LEFT,			// KEY_LEFT
	SDL_SCANCODE_DOWN,			// KEY_DOWN
	SDL_SCANCODE_RIGHT,			// KEY_RIGHT

	SDL_SCANCODE_F1,   // KEY_F1
	SDL_SCANCODE_F2,   // KEY_F2
	SDL_SCANCODE_F3,   // KEY_F3
	SDL_SCANCODE_F4,   // KEY_F4
	SDL_SCANCODE_F5,   // KEY_F5
	SDL_SCANCODE_F6,   // KEY_F6
	SDL_SCANCODE_F7,   // KEY_F7
	SDL_SCANCODE_F8,   // KEY_F8
	SDL_SCANCODE_F9,   // KEY_F9
	SDL_SCANCODE_F10,  // KEY_F10
	SDL_SCANCODE_F11,  // KEY_F11
	SDL_SCANCODE_F12   // KEY_F12
};

/*
==================
TranslateSDLDisplayOrientation
==================
*/
FORCEINLINE static displayOrientation_t TranslateSDLDisplayOrientation( SDL_DisplayOrientation sdlDisplayOrientation )
{
	switch ( sdlDisplayOrientation )
	{
	case SDL_ORIENTATION_UNKNOWN: return DISPLAY_ORIENTATION_UNKNOWN;
	case SDL_ORIENTATION_LANDSCAPE: return DISPLAY_ORIENTATION_LANDSCAPE;
	case SDL_ORIENTATION_LANDSCAPE_FLIPPED: return DISPLAY_ORIENTATION_LANDSCAPE_FLIPPED;
	case SDL_ORIENTATION_PORTRAIT: return DISPLAY_ORIENTATION_PORTRAIT;
	case SDL_ORIENTATION_PORTRAIT_FLIPPED: return DISPLAY_ORIENTATION_PORTRAIT_FLIPPED;
	default:
		AssertMsg( false, "Unkown SDL display orientation 0x%X", sdlDisplayOrientation );
		return DISPLAY_ORIENTATION_UNKNOWN;
	}
}

/*
==================
TranslateSDLScanCode
==================
*/
FORCEINLINE static buttonCode_t TranslateSDLScanCode( uint32 scancode )
{
	for ( uint32 index = 0; index < BUTTON_CODE_COUNT; ++index )
	{
		if ( s_scanCodeTable[index] == (SDL_Scancode)scancode )
		{
			return (buttonCode_t)index;
		}
	}
	return BUTTON_CODE_NONE;
}

/*
==================
TranslateSDLMouseButton
==================
*/
FORCEINLINE static buttonCode_t TranslateSDLMouseButton( uint8 buttonIndex )
{
	switch ( buttonIndex )
	{
	case 1: return MOUSE_1;
	case 2: return MOUSE_2;
	case 3: return MOUSE_3;
	case 4: return MOUSE_4;
	case 5: return MOUSE_5;
	case 6: return MOUSE_6;
	case 7: return MOUSE_7;
	case 8: return MOUSE_8;
	default: return BUTTON_CODE_NONE;
	}
}

/*
==================
TranslateSDLKeymod
==================
*/
FORCEINLINE static keyModifierMask_t TranslateSDLKeymod( SDL_Keymod sdlKeymod )
{
	keyModifierMask_t result;
	result.bAlt		 = sdlKeymod & SDL_KMOD_ALT;
	result.bCapsLock = sdlKeymod & SDL_KMOD_CAPS;
	result.bControl	 = sdlKeymod & SDL_KMOD_CTRL;
	result.bNumLock	 = sdlKeymod & SDL_KMOD_NUM;
	result.bShift	 = sdlKeymod & SDL_KMOD_SHIFT;
	result.bSuper	 = sdlKeymod & SDL_KMOD_GUI;
	return result;
}

/*
==================
CWindowMgrSDL::CWindowMgrSDL
==================
*/
CWindowMgrSDL::CWindowMgrSDL()
	: mainWindowId( INVALID_INDEX )
{
}

/*
==================
CWindowMgrSDL::Init
==================
*/
bool CWindowMgrSDL::Init()
{
	if ( !SDL_WasInit( SDL_INIT_VIDEO ) && !SDL_Init( SDL_INIT_VIDEO ) )
	{
		Error( "WindowMgrSDL: Failed to initialize SDL. SDL error: %s", SDL_GetError() );
		return false;
	}
	return true;
}

/*
==================
CWindowMgrSDL::Shutdown
==================
*/
void CWindowMgrSDL::Shutdown()
{
	for ( size index = 0, count = windowIds.size(); index < count; ++index )
	{
		windowPool.Free( windowIds[index] );
	}

	onWindowEvent.RemoveAll();
	onInputEvent.RemoveAll();
	SDL_Quit();

	windowIds.clear();
	mainWindowId = INVALID_INDEX;
}

/*
==================
CWindowMgrSDL::CreateWindow
==================
*/
IWindow* CWindowMgrSDL::CreateWindow()
{
	IWindow*   pWindow	= windowPool.Create();
	windowId_t windowId = pWindow->GetId();
	windowIds.emplace_back( windowId );

	if ( mainWindowId == INVALID_INDEX )
	{
		mainWindowId = windowId;
		onChangedMainWindow.Broadcast( mainWindowId );
	}
	return pWindow;
}

/*
==================
CWindowMgrSDL::CreateWindow
==================
*/
void CWindowMgrSDL::DestroyWindow( windowId_t windowId )
{
	if ( windowId == INVALID_INDEX )
	{
		return;
	}

	windowPool.Free( windowId );
	for ( size index = 0, count = windowIds.size(); index < count; ++index )
	{
		if ( windowId == windowIds[index] )
		{
			windowIds.erase( windowIds.begin() + index );
			break;
		}
	}

	if ( mainWindowId == windowId )
	{
		mainWindowId = !windowIds.empty() ? windowIds[0] : INVALID_INDEX;
		onChangedMainWindow.Broadcast( mainWindowId );
	}
}

/*
==================
CWindowMgrSDL::ProcessEvents
==================
*/
void CWindowMgrSDL::ProcessEvents()
{
	// Polls for currently pending SDL3 events
	PROFILE_SCOPE();
	SDL_Event sdlEvent;
	while ( SDL_PollEvent( &sdlEvent ) )
	{
		windowEvent_t windowEvent;
		inputEvent_t  inputEvent;
		switch ( sdlEvent.type )
		{
			// Text input event
		case SDL_EVENT_TEXT_INPUT:
			inputEvent.type					  = INPUT_EVENT_TYPE_TEXT_INPUT;
			inputEvent.windowId				  = CWindowSDL::GetIdBySDLWindowId( sdlEvent.text.windowID );
			inputEvent.textInput.pText		  = sdlEvent.text.text;
			inputEvent.textInput.modifierMask = TranslateSDLKeymod( SDL_GetModState() );
			break;

			// Key pressed and released button events
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP:
			if ( sdlEvent.type == SDL_EVENT_KEY_UP )
			{
				inputEvent.type = INPUT_EVENT_TYPE_KEY_RELEASED;
			}
			else
			{
				inputEvent.type = !sdlEvent.key.repeat ? INPUT_EVENT_TYPE_KEY_PRESSED : INPUT_EVENT_TYPE_KEY_REPEAT;
			}

			inputEvent.windowId			= CWindowSDL::GetIdBySDLWindowId( sdlEvent.key.windowID );
			inputEvent.key.code			= TranslateSDLScanCode( sdlEvent.key.scancode );
			inputEvent.key.modifierMask = TranslateSDLKeymod( sdlEvent.key.mod );
			break;

			// Mouse button pressed and released events
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP:
			if ( sdlEvent.type == SDL_EVENT_MOUSE_BUTTON_UP )
			{
				inputEvent.type = INPUT_EVENT_TYPE_MOUSE_RELEASED;
			}
			else
			{
				inputEvent.type = INPUT_EVENT_TYPE_MOUSE_PRESSED;
			}

			inputEvent.windowId					= CWindowSDL::GetIdBySDLWindowId( sdlEvent.button.windowID );
			inputEvent.mouseButton.code			= TranslateSDLMouseButton( sdlEvent.button.button );
			inputEvent.mouseButton.modifierMask = TranslateSDLKeymod( SDL_GetModState() );
			break;

			// Moving wheel mouse event
		case SDL_EVENT_MOUSE_WHEEL:
			inputEvent.type					   = INPUT_EVENT_TYPE_MOUSE_WHEEL;
			inputEvent.windowId				   = CWindowSDL::GetIdBySDLWindowId( sdlEvent.wheel.windowID );
			inputEvent.mouseWheel.x			   = sdlEvent.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? sdlEvent.wheel.x * -1 : sdlEvent.wheel.x;
			inputEvent.mouseWheel.y			   = sdlEvent.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? sdlEvent.wheel.y * -1 : sdlEvent.wheel.y;
			inputEvent.mouseWheel.modifierMask = TranslateSDLKeymod( SDL_GetModState() );
			break;

			// Mouse moving event
		case SDL_EVENT_MOUSE_MOTION:
			inputEvent.type					= INPUT_EVENT_TYPE_MOUSE_MOVE;
			inputEvent.windowId				= CWindowSDL::GetIdBySDLWindowId( sdlEvent.motion.windowID );
			inputEvent.mouseMove.x			= sdlEvent.motion.x;
			inputEvent.mouseMove.y			= sdlEvent.motion.y;
			inputEvent.mouseMove.xDirection = sdlEvent.motion.xrel;
			inputEvent.mouseMove.yDirection = sdlEvent.motion.yrel;
			break;

			// Close window event
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			windowEvent.type	 = WINDOW_EVENT_TYPE_CLOSE;
			windowEvent.windowId = CWindowSDL::GetIdBySDLWindowId( sdlEvent.window.windowID );
			break;

			// Pixel size has been changed event
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			windowEvent.type		  = WINDOW_EVENT_TYPE_PIXEL_SIZE_CHANGED;
			windowEvent.windowId	  = CWindowSDL::GetIdBySDLWindowId( sdlEvent.window.windowID );
			windowEvent.resize.width  = sdlEvent.window.data1;
			windowEvent.resize.height = sdlEvent.window.data2;
			break;

			// Resize window event
		case SDL_EVENT_WINDOW_RESIZED:
			windowEvent.type		  = WINDOW_EVENT_TYPE_RESIZE;
			windowEvent.windowId	  = CWindowSDL::GetIdBySDLWindowId( sdlEvent.window.windowID );
			windowEvent.resize.width  = sdlEvent.window.data1;
			windowEvent.resize.height = sdlEvent.window.data2;
			break;

			// Gained focus event
		case SDL_EVENT_WINDOW_FOCUS_GAINED:
			windowEvent.type	 = WINDOW_EVENT_TYPE_FOCUS_GAINED;
			windowEvent.windowId = CWindowSDL::GetIdBySDLWindowId( sdlEvent.window.windowID );
			break;

			// Lost focus event
		case SDL_EVENT_WINDOW_FOCUS_LOST:
			windowEvent.type	 = WINDOW_EVENT_TYPE_FOCUS_LOST;
			windowEvent.windowId = CWindowSDL::GetIdBySDLWindowId( sdlEvent.window.windowID );
			break;

			// Move window event
		case SDL_EVENT_WINDOW_MOVED:
			windowEvent.type	 = WINDOW_EVENT_TYPE_MOVE;
			windowEvent.windowId = CWindowSDL::GetIdBySDLWindowId( sdlEvent.window.windowID );
			windowEvent.move.x	 = sdlEvent.window.data1;
			windowEvent.move.y	 = sdlEvent.window.data2;
			break;

			// Window has been minimized
		case SDL_EVENT_WINDOW_MINIMIZED:
			windowEvent.type	 = WINDOW_EVENT_TYPE_MINIMIZED;
			windowEvent.windowId = CWindowSDL::GetIdBySDLWindowId( sdlEvent.window.windowID );
			break;

			// Window has been maximized
		case SDL_EVENT_WINDOW_MAXIMIZED:
			windowEvent.type	 = WINDOW_EVENT_TYPE_MAXIMIZED;
			windowEvent.windowId = CWindowSDL::GetIdBySDLWindowId( sdlEvent.window.windowID );
			break;

			// Window has been restored to normal size and position
		case SDL_EVENT_WINDOW_RESTORED:
			windowEvent.type	 = WINDOW_EVENT_TYPE_RESTORED;
			windowEvent.windowId = CWindowSDL::GetIdBySDLWindowId( sdlEvent.window.windowID );
			break;

			// Window has been moved to display data1
		case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
			windowEvent.type					 = WINDOW_EVENT_TYPE_DISPLAY_CHANGED;
			windowEvent.windowId				 = CWindowSDL::GetIdBySDLWindowId( sdlEvent.window.windowID );
			windowEvent.displayChanged.displayId = sdlEvent.window.data1;
			break;

			// Unknown event
		default:
			inputEvent.type	 = INPUT_EVENT_TYPE_NONE;
			windowEvent.type = WINDOW_EVENT_TYPE_NONE;
			break;
		}

		// Broadcast process the window event
		if ( windowEvent.type != WINDOW_EVENT_TYPE_NONE )
		{
			onWindowEvent.Broadcast( windowEvent );
		}

		// Broadcast process the input event
		if ( inputEvent.type != INPUT_EVENT_TYPE_NONE )
		{
			onInputEvent.Broadcast( inputEvent );
		}
	}
}

/*
==================
CWindowMgrSDL::GetDisplays
==================
*/
uint32 CWindowMgrSDL::GetDisplays( display_t* pDisplays, uint32 maxNumDisplays ) const
{
	uint32		   numDisplays	  = 0;
	SDL_DisplayID* pSDLDisplayIds = SDL_GetDisplays( (int32*)&numDisplays );
	if ( numDisplays > 0 )
	{
		SDL_Rect sdlRect;
		uint32	 numDisplaysToWrite = Min( numDisplays, pDisplays ? maxNumDisplays : 0 );
		for ( uint32 index = 0; index < numDisplaysToWrite; ++index )
		{
			SDL_DisplayID sdlDisplayId = pSDLDisplayIds[index];
			display_t&	  display	   = pDisplays[index];
			display.handle			   = sdlDisplayId;
			display.pName			   = SDL_GetDisplayName( sdlDisplayId );
			display.orientation		   = TranslateSDLDisplayOrientation( SDL_GetCurrentDisplayOrientation( sdlDisplayId ) );

			SDL_GetDisplayBounds( sdlDisplayId, &sdlRect );
			display.bounds.x	  = sdlRect.x;
			display.bounds.y	  = sdlRect.y;
			display.bounds.width  = sdlRect.w;
			display.bounds.height = sdlRect.h;

			SDL_GetDisplayUsableBounds( sdlDisplayId, &sdlRect );
			display.usableBounds.x		= sdlRect.x;
			display.usableBounds.y		= sdlRect.y;
			display.usableBounds.width	= sdlRect.w;
			display.usableBounds.height = sdlRect.h;
		}
	}

	SDL_free( pSDLDisplayIds );
	return numDisplays;
}

/*
==================
CWindowMgrSDL::GetDisplayById
==================
*/
bool CWindowMgrSDL::GetDisplayById( uint32 index, display_t& display ) const
{
	uint32		   numDisplays	  = 0;
	SDL_DisplayID* pSDLDisplayIds = SDL_GetDisplays( (int32*)&numDisplays );
	if ( index < numDisplays )
	{
		SDL_Rect	  sdlRect;
		SDL_DisplayID sdlDisplayId = pSDLDisplayIds[index];
		display.handle			   = sdlDisplayId;
		display.pName			   = SDL_GetDisplayName( sdlDisplayId );
		display.orientation		   = TranslateSDLDisplayOrientation( SDL_GetCurrentDisplayOrientation( sdlDisplayId ) );

		SDL_GetDisplayBounds( sdlDisplayId, &sdlRect );
		display.bounds.x	  = sdlRect.x;
		display.bounds.y	  = sdlRect.y;
		display.bounds.width  = sdlRect.w;
		display.bounds.height = sdlRect.h;

		SDL_GetDisplayUsableBounds( sdlDisplayId, &sdlRect );
		display.usableBounds.x		= sdlRect.x;
		display.usableBounds.y		= sdlRect.y;
		display.usableBounds.width	= sdlRect.w;
		display.usableBounds.height = sdlRect.h;
	}

	SDL_free( pSDLDisplayIds );
	return index < numDisplays;
}

/*
==================
CWindowMgrSDL::GetDisplayById
==================
*/
bool CWindowMgrSDL::GetDisplayByHandle( displayHandle_t displayHandle, display_t& display ) const
{
	if ( displayHandle != INVALID_DISPLAY_HANDLE )
	{
		SDL_Rect sdlRect;
		display.handle		= displayHandle;
		display.pName		= SDL_GetDisplayName( displayHandle );
		display.orientation = TranslateSDLDisplayOrientation( SDL_GetCurrentDisplayOrientation( displayHandle ) );

		SDL_GetDisplayBounds( displayHandle, &sdlRect );
		display.bounds.x	  = sdlRect.x;
		display.bounds.y	  = sdlRect.y;
		display.bounds.width  = sdlRect.w;
		display.bounds.height = sdlRect.h;

		SDL_GetDisplayUsableBounds( displayHandle, &sdlRect );
		display.usableBounds.x		= sdlRect.x;
		display.usableBounds.y		= sdlRect.y;
		display.usableBounds.width	= sdlRect.w;
		display.usableBounds.height = sdlRect.h;
	}

	return displayHandle != INVALID_DISPLAY_HANDLE;
}

/*
==================
CWindowMgrSDL::GetPrimaryDisplay
==================
*/
bool CWindowMgrSDL::GetPrimaryDisplay( display_t& display ) const
{
	SDL_DisplayID sdlDisplayId = SDL_GetPrimaryDisplay();
	if ( sdlDisplayId != 0 )
	{
		SDL_Rect sdlRect;
		display.handle		= sdlDisplayId;
		display.pName		= SDL_GetDisplayName( sdlDisplayId );
		display.orientation = TranslateSDLDisplayOrientation( SDL_GetCurrentDisplayOrientation( sdlDisplayId ) );

		SDL_GetDisplayBounds( sdlDisplayId, &sdlRect );
		display.bounds.x	  = sdlRect.x;
		display.bounds.y	  = sdlRect.y;
		display.bounds.width  = sdlRect.w;
		display.bounds.height = sdlRect.h;

		SDL_GetDisplayUsableBounds( sdlDisplayId, &sdlRect );
		display.usableBounds.x		= sdlRect.x;
		display.usableBounds.y		= sdlRect.y;
		display.usableBounds.width	= sdlRect.w;
		display.usableBounds.height = sdlRect.h;
	}

	return sdlDisplayId != 0;
}

/*
==================
CWindowMgrSDL::GetDisplayModes
==================
*/
uint32 CWindowMgrSDL::GetDisplayModes( displayHandle_t displayHandle, displayMode_t* pDisplayModes, uint32 maxNumDisplayModes ) const
{
	uint32			  sdlNumDisplayModes = 0;
	uint32			  numDisplayModes	 = 0;
	SDL_DisplayMode** ppSDLDisplayModes	 = SDL_GetFullscreenDisplayModes( displayHandle, (int32*)&sdlNumDisplayModes );
	if ( numDisplayModes > 0 )
	{
		uint32* pSDLDisplayModeIds = (uint32*)Mem_Alloca( sdlNumDisplayModes * sizeof( uint32 ) );
		for ( uint32 index = 0; index < sdlNumDisplayModes; ++index )
		{
			SDL_DisplayMode* pSDLDisplayMode   = ppSDLDisplayModes[index];
			bool			 bFoundDisplayMode = false;
			for ( uint32 searchIndex = 0; searchIndex < numDisplayModes && !bFoundDisplayMode; ++searchIndex )
			{
				SDL_DisplayMode* pSDLDisplayModeSearch = ppSDLDisplayModes[pSDLDisplayModeIds[searchIndex]];
				if ( pSDLDisplayMode->w == pSDLDisplayModeSearch->w && pSDLDisplayMode->h == pSDLDisplayModeSearch->h
					 && pSDLDisplayMode->refresh_rate == pSDLDisplayModeSearch->refresh_rate )
				{
					bFoundDisplayMode = true;
				}
			}

			if ( !bFoundDisplayMode )
			{
				pSDLDisplayModeIds[numDisplayModes++] = index;
			}
		}

		uint32 numDisplayModesToWrite = Min( numDisplayModes, pDisplayModes ? maxNumDisplayModes : 0 );
		for ( uint32 index = 0; index < numDisplayModesToWrite; ++index )
		{
			SDL_DisplayMode* pSDLDisplayMode = ppSDLDisplayModes[pSDLDisplayModeIds[index]];
			displayMode_t&	 displayMode	 = pDisplayModes[index];
			displayMode.width				 = pSDLDisplayMode->w;
			displayMode.height				 = pSDLDisplayMode->h;
			displayMode.refreshRate			 = pSDLDisplayMode->refresh_rate;
		}
	}

	SDL_free( ppSDLDisplayModes );
	return numDisplayModes;
}

/*
==================
CWindowMgrSDL::GetDesktopDisplayMode
==================
*/
bool CWindowMgrSDL::GetDesktopDisplayMode( displayHandle_t displayHandle, displayMode_t& displayMode ) const
{
	const SDL_DisplayMode* pSDLDisplayMode = SDL_GetDesktopDisplayMode( displayHandle );
	if ( pSDLDisplayMode )
	{
		displayMode.width		= pSDLDisplayMode->w;
		displayMode.height		= pSDLDisplayMode->h;
		displayMode.refreshRate = pSDLDisplayMode->refresh_rate;
	}
	return !!pSDLDisplayMode;
}

/*
==================
CWindowMgrSDL::GetCurrentDisplayMode
==================
*/
bool CWindowMgrSDL::GetCurrentDisplayMode( displayHandle_t displayHandle, displayMode_t& displayMode ) const
{
	const SDL_DisplayMode* pSDLDisplayMode = SDL_GetCurrentDisplayMode( displayHandle );
	if ( pSDLDisplayMode )
	{
		displayMode.width		= pSDLDisplayMode->w;
		displayMode.height		= pSDLDisplayMode->h;
		displayMode.refreshRate = pSDLDisplayMode->refresh_rate;
	}
	return !!pSDLDisplayMode;
}

/*
==================
CWindowMgrSDL::FindClosestDisplayMode
==================
*/
bool CWindowMgrSDL::FindClosestDisplayMode( displayHandle_t displayHandle, int32 width, int32 height, float refreshRate, bool bIncludeHightDensityModes, displayMode_t& displayMode ) const
{
	SDL_DisplayMode sdlDisplayMode;
	if ( !SDL_GetClosestFullscreenDisplayMode( displayHandle, width, height, refreshRate, bIncludeHightDensityModes, &sdlDisplayMode ) )
	{
		Warning( "WindowMgrSDL: Couldn't find closest display mode to %ix%i@%fHz", width, height, refreshRate );
		return false;
	}

	displayMode.width		= sdlDisplayMode.w;
	displayMode.height		= sdlDisplayMode.h;
	displayMode.refreshRate = sdlDisplayMode.refresh_rate;
	return true;
}

/*
==================
GetWindowMgrSDL
==================
*/
CWindowMgrSDL& GetWindowMgrSDL()
{
	static CWindowMgrSDL s_windowMgrSDL;
	return s_windowMgrSDL;
}

/*
==================
CreateWindowMgr
==================
*/
IWindowMgr* CreateWindowMgr()
{
	return &GetWindowMgrSDL();
}
#endif	// PLATFORM_USE_SDL
