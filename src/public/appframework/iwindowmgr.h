#pragma once
#include "tier1/delegate.h"
#include "tier1/math/rect.h"
#include "appframework/iappsystem.h"
#include "appframework/iwindow.h"
#include "appframework/windowevent.h"
#include "inputsystem/inputevent.h"

//-----------------------------------------------------------------------------
// Constants and types
//-----------------------------------------------------------------------------
enum displayOrientation_t
{
	DISPLAY_ORIENTATION_UNKNOWN,			// The display orientation can't be determined
	DISPLAY_ORIENTATION_LANDSCAPE,			// The display is in landscape mode, with the right side up, relative to portrait mode
	DISPLAY_ORIENTATION_LANDSCAPE_FLIPPED,	// The display is in landscape mode, with the left side up, relative to portrait mode
	DISPLAY_ORIENTATION_PORTRAIT,			// The display is in portrait mode
	DISPLAY_ORIENTATION_PORTRAIT_FLIPPED	// The display is in portrait mode, upside down
};

struct displayMode_t
{
	uint32 width;
	uint32 height;
	float  refreshRate;
};

struct display_t
{
	uint32				 id;
	const char*			 pName;
	rect_t<uint32>		 bounds;
	rect_t<uint32>		 usableBounds;
	displayOrientation_t orientation;
};

//-----------------------------------------------------------------------------
// The window manager interface
//-----------------------------------------------------------------------------
#define WINDOWMGR_INTERFACE_VERSION "SWindowMgr001"
class IWindowMgr : public IAppSystem
{
public:
	DECLARE_MULTICAST_DELEGATE_INTERFACE( IOnChangedMainWindow, windowId_t /* newMainWindowId */ );
	DECLARE_MULTICAST_DELEGATE_INTERFACE( IOnWindowEvent, const windowEvent_t& /* windowEvent */ );
	DECLARE_MULTICAST_DELEGATE_INTERFACE( IOnInputEvent, const inputEvent_t& /* inputEvent */ );

	// Functions to work with windows
	virtual IWindow*   CreateWindow()								  = 0;
	virtual void	   DestroyWindow( windowId_t windowId )			  = 0;
	virtual IWindow*   GetWindow( windowId_t windowId ) const		  = 0;
	virtual void	   ChangeMainWindow( windowId_t newMainWindowId ) = 0;
	virtual IWindow*   GetOrCreateMainWindow()						  = 0;
	virtual windowId_t GetMainWindowId() const						  = 0;

	// Functions to process events
	virtual void				  ProcessEvents()			  = 0;
	virtual IOnChangedMainWindow* OnChangedMainWindow() const = 0;
	virtual IOnWindowEvent*		  OnWindowEvent() const		  = 0;
	virtual IOnInputEvent*		  OnInputEvent() const		  = 0;

	// Functions to get information about displays
	virtual uint32 GetDisplays( display_t* pDisplays, uint32 maxNumDisplays ) const																									  = 0;
	virtual bool   GetPrimaryDisplay( display_t& display ) const																													  = 0;
	virtual uint32 GetDisplayModes( const display_t& display, displayMode_t* pDisplayModes, uint32 maxNumDisplayModes ) const														  = 0;
	virtual bool   GetDesktopDisplayMode( const display_t& display, displayMode_t& displayMode ) const																				  = 0;
	virtual bool   GetCurrentDisplayMode( const display_t& display, displayMode_t& displayMode ) const																				  = 0;
	virtual bool   FindClosestDisplayMode( const display_t& display, int32 width, int32 height, float refreshRate, bool bIncludeHightDensityModes, displayMode_t& displayMode ) const = 0;
};

//-----------------------------------------------------------------------------
// Create a singleton window manager
//-----------------------------------------------------------------------------
IWindowMgr* CreateWindowMgr();
