#pragma once

//-----------------------------------------------------------------------------
// Button codes
//-----------------------------------------------------------------------------
enum buttonCode_t
{
	//
	// Keyboard buttons
	//

	KEY_FIRST = 0,

	KEY_0 = KEY_FIRST,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEYPAD_0,
	KEYPAD_1,
	KEYPAD_2,
	KEYPAD_3,
	KEYPAD_4,
	KEYPAD_5,
	KEYPAD_6,
	KEYPAD_7,
	KEYPAD_8,
	KEYPAD_9,
	KEYPAD_DIVIDE,
	KEYPAD_MULTIPLY,
	KEYPAD_MINUS,
	KEYPAD_PLUS,
	KEYPAD_ENTER,
	KEYPAD_DECIMAL,
	KEY_LBRACKET,
	KEY_RBRACKET,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_BACKQUOTE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_BACKSLASH,
	KEY_MINUS,
	KEY_EQUAL,
	KEY_ENTER,
	KEY_SPACE,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_CAPSLOCK,
	KEY_NUMLOCK,
	KEY_ESCAPE,
	KEY_SCROLLLOCK,
	KEY_INSERT,
	KEY_DELETE,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_BREAK,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LALT,
	KEY_RALT,
	KEY_LCONTROL,
	KEY_RCONTROL,
	KEY_LWIN,
	KEY_RWIN,
	KEY_APP,
	KEY_UP,
	KEY_LEFT,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,

	KEY_LAST  = KEY_F12,
	KEY_COUNT = KEY_LAST - KEY_FIRST + 1,

	//
	// Mouse buttons
	//

	MOUSE_FIRST = KEY_LAST + 1,

	MOUSE_1 = MOUSE_FIRST,
	MOUSE_2,
	MOUSE_3,
	MOUSE_4,
	MOUSE_5,
	MOUSE_6,
	MOUSE_7,
	MOUSE_8,
	MOUSE_WHEELUP,
	MOUSE_WHEELDOWN,
	MOUSE_X,
	MOUSE_Y,

	MOUSE_LEFT		   = MOUSE_1,
	MOUSE_MIDDLE	   = MOUSE_2,
	MOUSE_RIGHT		   = MOUSE_3,
	MOUSE_LAST		   = MOUSE_Y,
	MOUSE_BUTTON_FIRST = MOUSE_1,
	MOUSE_BUTTON_LAST  = MOUSE_8,
	MOUSE_WHEEL_FIRST  = MOUSE_WHEELUP,
	MOUSE_WHEEL_LAST   = MOUSE_WHEELDOWN,
	MOUSE_AXIS_FIRST   = MOUSE_X,
	MOUSE_AXIS_LAST	   = MOUSE_Y,
	MOUSE_COUNT		   = MOUSE_LAST - MOUSE_FIRST + 1,

	//
	// Range markers
	//

	BUTTON_CODE_FIRST = KEY_FIRST,
	BUTTON_CODE_LAST  = MOUSE_LAST,
	BUTTON_CODE_COUNT = BUTTON_CODE_LAST - BUTTON_CODE_FIRST + 1,
	BUTTON_CODE_NONE  = BUTTON_CODE_COUNT
};

//-----------------------------------------------------------------------------
// Button code range checks
//-----------------------------------------------------------------------------
bool Input_IsKeyCode( buttonCode_t code );
bool Input_IsMouseCode( buttonCode_t code );
bool Input_IsMouseWheel( buttonCode_t code );
bool Input_IsMouseAxis( buttonCode_t code );
bool Input_IsValidCode( buttonCode_t code );

#include "inputsystem/buttoncode.inl"
