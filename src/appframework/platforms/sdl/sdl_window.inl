#pragma once

/*
==================
CWindowSDL::Show
==================
*/
FORCEINLINE void CWindowSDL::Show( bool bShow /* = true */ )
{
	Assert( pSDLWindow );
	if ( bShow )
	{
		SDL_ShowWindow( pSDLWindow );
	}
	else
	{
		SDL_HideWindow( pSDLWindow );
	}
}

/*
==================
CWindowSDL::Maximize
==================
*/
FORCEINLINE void CWindowSDL::Maximize()
{
	Assert( pSDLWindow );
	SDL_MaximizeWindow( pSDLWindow );
}

/*
==================
CWindowSDL::Minimize
==================
*/
FORCEINLINE void CWindowSDL::Minimize()
{
	Assert( pSDLWindow );
	SDL_MinimizeWindow( pSDLWindow );
}

/*
==================
CWindowSDL::SetTitle
==================
*/
FORCEINLINE void CWindowSDL::SetTitle( const char* pTitle )
{
	Assert( pSDLWindow );
	SDL_SetWindowTitle( pSDLWindow, pTitle );
}

/*
==================
CWindowSDL::SetSize
==================
*/
FORCEINLINE void CWindowSDL::SetSize( uint32 width, uint32 height )
{
	Assert( pSDLWindow );
	SDL_SetWindowSize( pSDLWindow, width, height );
	SDL_SetWindowPosition( pSDLWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED );
}

/*
==================
CWindowSDL::SetRefreshRate
==================
*/
FORCEINLINE void CWindowSDL::SetRefreshRate( float refreshRate )
{
	// Change the refresh rate and update display mode if we in exclusive fullscreen mode
	CWindowSDL::refreshRate = refreshRate;
	if ( mode == WINDOW_MODE_EXCLUSIVE_FULLSCREEN )
	{
		UpdateDisplayMode();
	}
}

/*
==================
CWindowSDL::SetDisplay
==================
*/
FORCEINLINE void CWindowSDL::SetDisplay( displayHandle_t displayHandle )
{
	Assert( pSDLWindow );
	if ( displayHandle != INVALID_DISPLAY_HANDLE )
	{
		// Change the window position and update display mode if we in exclusive fullscreen mode
		SDL_SetWindowPosition( pSDLWindow, SDL_WINDOWPOS_CENTERED_DISPLAY( displayHandle ), SDL_WINDOWPOS_CENTERED_DISPLAY( displayHandle ) );
		if ( mode == WINDOW_MODE_EXCLUSIVE_FULLSCREEN )
		{
			UpdateDisplayMode();
		}
	}
}

/*
==================
CWindowSDL::SetCursorVisible
==================
*/
FORCEINLINE void CWindowSDL::SetCursorVisible( bool bVisible )
{
	Assert( pSDLWindow );
	SDL_SetWindowRelativeMouseMode( pSDLWindow, !bVisible );
}

/*
==================
CWindowSDL::IsOpen
==================
*/
FORCEINLINE bool CWindowSDL::IsOpen() const
{
	return !!pSDLWindow;
}

/*
==================
CWindowSDL::IsCursorVisible
==================
*/
FORCEINLINE bool CWindowSDL::IsCursorVisible() const
{
	Assert( pSDLWindow );
	return !SDL_GetWindowRelativeMouseMode( pSDLWindow );
}

/*
==================
CWindowSDL::GetMode
==================
*/
FORCEINLINE windowMode_t CWindowSDL::GetMode() const
{
	return mode;
}

/*
==================
CWindowSDL::GetRefreshRate
==================
*/
FORCEINLINE float CWindowSDL::GetRefreshRate() const
{
	return refreshRate;
}

/*
==================
CWindowSDL::GetPixelDensity
==================
*/
FORCEINLINE float CWindowSDL::GetPixelDensity() const
{
	Assert( pSDLWindow );
	return SDL_GetWindowPixelDensity( pSDLWindow );
}

/*
==================
CWindowSDL::GetDisplayScale
==================
*/
FORCEINLINE float CWindowSDL::GetDisplayScale() const
{
	Assert( pSDLWindow );
	return SDL_GetWindowDisplayScale( pSDLWindow );
}

/*
==================
CWindowSDL::GetSize
==================
*/
FORCEINLINE void CWindowSDL::GetSize( ivec2_t& size ) const
{
	Assert( pSDLWindow );
	SDL_GetWindowSize( pSDLWindow, (int32*)&size.x, (int32*)&size.y );
}

/*
==================
CWindowSDL::GetSizeInPixels
==================
*/
FORCEINLINE void CWindowSDL::GetSizeInPixels( ivec2_t& size ) const
{
	Assert( pSDLWindow );
	SDL_GetWindowSizeInPixels( pSDLWindow, (int32*)&size.x, (int32*)&size.y );
}

/*
==================
CWindowSDL::GetHandle
==================
*/
FORCEINLINE windowHandle_t CWindowSDL::GetHandle() const
{
	return handle;
}

/*
==================
CWindowSDL::GetId
==================
*/
FORCEINLINE windowId_t CWindowSDL::GetId() const
{
	return id;
}
