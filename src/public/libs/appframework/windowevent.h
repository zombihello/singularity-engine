#pragma once
#include "stdlib/types.h"

//-----------------------------------------------------------------------------
// Window events
//-----------------------------------------------------------------------------
struct windowEvent_t
{
	enum type_t
	{
		EVENT_NONE,
		EVENT_WINDOW_CLOSE,
		EVENT_WINDOW_RESIZE,
		EVENT_WINDOW_FOCUS_GAINED,
		EVENT_WINDOW_FOCUS_LOST,
		EVENT_WINDOW_MOVE,
		EVENT_WINDOW_MINIMIZED,
		EVENT_WINDOW_MAXIMIZED,
		EVENT_WINDOW_RESTORED,
		EVENT_WINDOW_DISPLAY_CHANGED
	};


	struct windowDisplayChangedEvent_t
	{	
		uint32		displayId;
	};


	struct windowResizeEvent_t
	{
		int32		width;
		int32		height;
	};


	struct windowMoveEvent_t
	{
		int32		x;
		int32		y;
	};


	union uevents_t
	{
		windowResizeEvent_t			windowResize;
		windowMoveEvent_t			windowMove;
		windowDisplayChangedEvent_t	windowDisplayChanged;
	};


	windowEvent_t() 
		: type( EVENT_NONE )
		, windowId( 0 )
	{}

	type_t			type;
	uint32			windowId;
	uevents_t		events;
};