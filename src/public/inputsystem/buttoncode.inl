#pragma once

/*
==================
Input_IsKeyCode
==================
*/
FORCEINLINE bool Input_IsKeyCode( buttonCode_t code )
{
	return code >= KEY_FIRST && code <= KEY_LAST;
}

/*
==================
Input_IsMouseCode
==================
*/
FORCEINLINE bool Input_IsMouseCode( buttonCode_t code )
{
	return code >= MOUSE_BUTTON_FIRST && code <= MOUSE_BUTTON_LAST;
}

/*
==================
Input_IsMouseWheel
==================
*/
FORCEINLINE bool Input_IsMouseWheel( buttonCode_t code )
{
	return code >= MOUSE_WHEEL_FIRST && code <= MOUSE_WHEEL_LAST;
}

/*
==================
Input_IsMouseAxis
==================
*/
FORCEINLINE bool Input_IsMouseAxis( buttonCode_t code )
{
	return code >= MOUSE_AXIS_FIRST && code <= MOUSE_AXIS_LAST;
}

/*
==================
Input_IsValidCode
==================
*/
FORCEINLINE bool Input_IsValidCode( buttonCode_t code )
{
	return code >= BUTTON_CODE_FIRST && code <= BUTTON_CODE_LAST;
}
