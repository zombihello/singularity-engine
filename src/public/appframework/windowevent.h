#pragma once
#include "tier0/types.h"
#include "appframework/iwindow.h"

//-----------------------------------------------------------------------------
// Window events
//-----------------------------------------------------------------------------
enum windowEventType_t
{
	WINDOW_EVENT_TYPE_NONE,
	WINDOW_EVENT_TYPE_CLOSE,
	WINDOW_EVENT_TYPE_RESIZE,
	WINDOW_EVENT_TYPE_PIXEL_SIZE_CHANGED,
	WINDOW_EVENT_TYPE_FOCUS_GAINED,
	WINDOW_EVENT_TYPE_FOCUS_LOST,
	WINDOW_EVENT_TYPE_MOVE,
	WINDOW_EVENT_TYPE_MINIMIZED,
	WINDOW_EVENT_TYPE_MAXIMIZED,
	WINDOW_EVENT_TYPE_RESTORED,
	WINDOW_EVENT_TYPE_DISPLAY_CHANGED
};

struct windowDisplayChangedEvent_t
{
	uint32 displayId;
};

struct windowResizeEvent_t
{
	int32 width;
	int32 height;
};

struct windowPixelSizeChangedEvent_t
{
	int32 width;
	int32 height;
};

struct windowMoveEvent_t
{
	int32 x;
	int32 y;
};

struct windowEvent_t
{
	windowEvent_t();

	windowEventType_t type;
	windowId_t		  windowId;
	union
	{
		windowResizeEvent_t			  resize;
		windowPixelSizeChangedEvent_t pixelSizeChanged;
		windowMoveEvent_t			  move;
		windowDisplayChangedEvent_t	  displayChanged;
	};
};

#include "appframework/windowevent.inl"
