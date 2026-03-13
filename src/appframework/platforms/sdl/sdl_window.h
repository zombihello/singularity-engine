#pragma once
#include "appframework/appframework_internal.h"

#if PLATFORM_USE_SDL
	#include <SDL3/SDL.h>

	#include "appframework/iwindow.h"
	#include "appframework/iwindowmgr.h"

//-----------------------------------------------------------------------------
// SDL window
//-----------------------------------------------------------------------------
class CWindowSDL : public IWindow
{
public:
	// IWindow interface
	virtual bool Create( const windowCreateInfo_t& createInfo ) override;
	virtual void Close() override;

	virtual void Show( bool bShow = true ) override;
	virtual void Maximize() override;
	virtual void Minimize() override;

	virtual void SetTitle( const char* pTitle ) override;
	virtual void SetSize( uint32 width, uint32 height ) override;
	virtual void SetRefreshRate( float refreshRate ) override;
	virtual void SetDisplay( displayHandle_t displayHandle ) override;
	virtual void SetMode( windowMode_t mode ) override;
	virtual void SetCursorVisible( bool bVisible ) override;

	virtual bool		   IsOpen() const override;
	virtual bool		   IsCursorVisible() const override;
	virtual windowMode_t   GetMode() const override;
	virtual float		   GetRefreshRate() const override;
	virtual float		   GetPixelDensity() const override;
	virtual float		   GetDisplayScale() const override;
	virtual void		   GetSize( vector2i_t& size ) const override;
	virtual void		   GetSizeInPixels( vector2i_t& size ) const override;
	virtual windowHandle_t GetHandle() const override;
	virtual windowId_t	   GetId() const override;

	CWindowSDL( windowId_t id );
	~CWindowSDL();

	static windowId_t GetIdBySDLWindowId( SDL_WindowID sdlWindowId );

private:
	void UpdateDisplayMode();

	float		   refreshRate;
	windowId_t	   id;
	SDL_Window*	   pSDLWindow;
	windowHandle_t handle;
	windowMode_t   mode;
};

	#include "appframework/platforms/sdl/sdl_window.inl"
#endif	// PLATFORM_USE_SDL
