#pragma once
#include "tier0/types.h"
#include "inputsystem/buttoncode.h"

//-----------------------------------------------------------------------------
//  Struct for storage input event
//-----------------------------------------------------------------------------
struct inputEvent_t
{
	enum type_t
	{
		EVENT_NONE,
		EVENT_KEY_PRESSED,
		EVENT_KEY_RELEASED,
		EVENT_MOUSE_PRESSED,
		EVENT_MOUSE_RELEASED,
		EVENT_MOUSE_MOVE,
		EVENT_MOUSE_WHEEL,
		EVENT_TEXT_INPUT
	};

	struct keyEvent_t
	{
		buttonCode_t code;
		bool		 bAlt;
		bool		 bControl;
		bool		 bShift;
		bool		 bSuper;
		bool		 bCapsLock;
		bool		 bNumLock;
	};

	struct mouseButtonEvent_t
	{
		buttonCode_t code;
		int32		 x;
		int32		 y;
	};

	struct mouseMoveEvent_t
	{
		int32 x;
		int32 y;
		int32 xDirection;
		int32 yDirection;
	};

	struct mouseWheelEvent_t
	{
		int32 x;
		int32 y;
	};

	struct textInputEvent_t
	{
		char* pText;
	};

	union uevents_t
	{
		keyEvent_t		   key;
		mouseButtonEvent_t mouseButton;
		mouseMoveEvent_t   mouseMove;
		mouseWheelEvent_t  mouseWheel;
		textInputEvent_t   textInputEvent;
	};

	inputEvent_t()
		: type( EVENT_NONE )
	{
	}

	type_t	  type;
	uevents_t events;
};