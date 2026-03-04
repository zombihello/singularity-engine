#pragma once
#include "tier0/types.h"
#include "appframework/iwindowmgr.h"
#include "inputsystem/keymodifiermask.h"
#include "inputsystem/buttoncode.h"

//-----------------------------------------------------------------------------
// Input events
//-----------------------------------------------------------------------------
enum inputEventType_t
{
	INPUT_EVENT_TYPE_NONE,
	INPUT_EVENT_TYPE_KEY_PRESSED,
	INPUT_EVENT_TYPE_KEY_RELEASED,
	INPUT_EVENT_TYPE_MOUSE_PRESSED,
	INPUT_EVENT_TYPE_MOUSE_RELEASED,
	INPUT_EVENT_TYPE_MOUSE_MOVE,
	INPUT_EVENT_TYPE_MOUSE_WHEEL,
	INPUT_EVENT_TYPE_TEXT_INPUT
};

struct keyEvent_t
{
	buttonCode_t	  code;
	keyModifierMask_t modifierMask;
};

struct mouseButtonEvent_t
{
	buttonCode_t	  code;
	keyModifierMask_t modifierMask;
};

struct mouseMoveEvent_t
{
	float x;
	float y;
	float xDirection;
	float yDirection;
};

struct mouseWheelEvent_t
{
	float			  x;
	float			  y;
	keyModifierMask_t modifierMask;
};

struct textInputEvent_t
{
	const char*		  pText;
	keyModifierMask_t modifierMask;
};

struct inputEvent_t
{
	inputEvent_t();

	inputEventType_t type;
	windowId_t		 windowId;
	union
	{
		keyEvent_t		   key;
		mouseButtonEvent_t mouseButton;
		mouseMoveEvent_t   mouseMove;
		mouseWheelEvent_t  mouseWheel;
		textInputEvent_t   textInput;
	};
};

#include "inputsystem/inputevent.inl"
