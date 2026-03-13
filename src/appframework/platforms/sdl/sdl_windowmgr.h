#pragma once
#include "appframework/appframework_internal.h"

#if PLATFORM_USE_SDL
	#include "appframework/windowpool.h"
	#include "appframework/platforms/sdl/sdl_window.h"

//-----------------------------------------------------------------------------
// SDL window manager
//-----------------------------------------------------------------------------
class CWindowMgrSDL : public CBaseAppSystem<IWindowMgr>
{
public:
	DECLARE_EVENT( COnChangedMainWindow, windowId_t /* newMainWindowId */ );
	DECLARE_EVENT( COnWindowEvent, const windowEvent_t& /* windowEvent */ );
	DECLARE_EVENT( COnInputEvent, const inputEvent_t& /* inputEvent */ );

	// IAppSystem interface
	virtual bool Init() override;
	virtual void Shutdown() override;

	// IWindowMgr interface
	// Functions to work with windows
	virtual IWindow*   CreateWindow() override;
	virtual void	   DestroyWindow( windowId_t windowId ) override;
	virtual IWindow*   GetWindow( windowId_t windowId ) const override;
	virtual void	   ChangeMainWindow( windowId_t newMainWindowId ) override;
	virtual IWindow*   GetOrCreateMainWindow() override;
	virtual windowId_t GetMainWindowId() const override;

	// Functions to process events
	virtual void				  ProcessEvents() override;
	virtual IOnChangedMainWindow* OnChangedMainWindow() const override;
	virtual IOnWindowEvent*		  OnWindowEvent() const override;
	virtual IOnInputEvent*		  OnInputEvent() const override;

	// Functions to get information about displays
	virtual uint32 GetDisplays( display_t* pDisplays, uint32 maxNumDisplays ) const override;
	virtual bool   GetDisplayById( uint32 index, display_t& display ) const override;
	virtual bool   GetDisplayByHandle( displayHandle_t displayHandle, display_t& display ) const override;
	virtual bool   GetPrimaryDisplay( display_t& display ) const override;
	virtual uint32 GetDisplayModes( displayHandle_t displayHandle, displayMode_t* pDisplayModes, uint32 maxNumDisplayModes ) const override;
	virtual bool   GetDesktopDisplayMode( displayHandle_t displayHandle, displayMode_t& displayMode ) const override;
	virtual bool   GetCurrentDisplayMode( displayHandle_t displayHandle, displayMode_t& displayMode ) const override;
	virtual bool   FindClosestDisplayMode( displayHandle_t displayHandle, int32 width, int32 height, float refreshRate, bool bIncludeHightDensityModes, displayMode_t& displayMode ) const override;

	CWindowMgrSDL();

private:
	windowId_t					 mainWindowId;
	CWindowPool<CWindowSDL>		 windowPool;
	eastl::vector<windowId_t>	 windowIds;
	mutable COnChangedMainWindow onChangedMainWindow;
	mutable COnWindowEvent		 onWindowEvent;
	mutable COnInputEvent		 onInputEvent;
};

CWindowMgrSDL& GetWindowMgrSDL();

	#include "appframework/platforms/sdl/sdl_windowmgr.inl"
#endif	// 0
