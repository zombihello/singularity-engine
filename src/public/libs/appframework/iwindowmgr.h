#pragma once
#include "stdlib/delegate.h"
#include "appframework/iappsystem.h"
#include "appframework/windowevent.h"
#include "inputsystem/inputevent.h"

//-----------------------------------------------------------------------------
// Window manager delegate interfaces
//-----------------------------------------------------------------------------
DECLARE_MULTICAST_DELEGATE_INTERFACE( IOnProcessWindowEvent, const windowEvent_t& /* windowEvent */ );
DECLARE_MULTICAST_DELEGATE_INTERFACE( IOnProcessInputEvent, const inputEvent_t& /* inputEvent */ );


//-----------------------------------------------------------------------------
// Window styles
//-----------------------------------------------------------------------------
enum styleWindow_t
{
	WINDOW_STYLE_NONE			= 0,
	WINDOW_STYLE_RESIZABLE		= 1 << 0,
	WINDOW_STYLE_DECORATED		= 1 << 1,
	WINDOW_STYLE_FLOATING		= 1 << 2,
	WINDOW_STYLE_FULLSCREEN		= 1 << 3,
	WINDOW_STYLE_HIDDEN			= 1 << 4,
	WINDOW_STYLE_MINIMIZED		= 1 << 5,	// If set flag WINDOW_STYLE_MAXIMIZED or WINDOW_STYLE_FULLSCREEN will be ignore
	WINDOW_STYLE_MAXIMIZED		= 1 << 6,	// If set flag WINDOW_STYLE_MINIMIZED or WINDOW_STYLE_FULLSCREEN will be ignore

	WINDOW_STYLE_DEFAULT		= WINDOW_STYLE_DECORATED | WINDOW_STYLE_RESIZABLE
};


//-----------------------------------------------------------------------------
// Window manager interface
//-----------------------------------------------------------------------------
#define WINDOWMGR_INTERFACE_VERSION			"SWindowMgr001"
class IWindowMgr : public IAppSystem
{
public:
	virtual bool Create( const achar* pTitle, uint32 width, uint32 height, uint32 flags = WINDOW_STYLE_DEFAULT ) = 0;
	virtual void Close() = 0;

	virtual void ShowWindow( bool bShowWindow = true ) = 0;
	virtual void Maximize() = 0;
	virtual void Minimize() = 0;

	virtual void ProcessEvents() = 0;
	
	virtual void SetTitle( const achar* pTitle ) = 0;
	virtual void SetSize( uint32 width, uint32 height ) = 0;
	virtual void SetFullscreen( bool bFullscreen ) = 0;

	virtual IOnProcessWindowEvent* OnProcessWindowEvent() const = 0;
	virtual IOnProcessInputEvent* OnProcessInputEvent() const = 0;

	virtual bool IsOpen() const = 0;
	virtual bool IsFullscreen() const = 0;
	virtual void GetSize( uint32& width, uint32& height ) const = 0;
	virtual windowHandle_t GetHandle() const = 0;
	virtual uint32 GetID() const = 0;
};


//-----------------------------------------------------------------------------
// Create a singleton window manager
//-----------------------------------------------------------------------------
IWindowMgr* CreateWindowMgr();