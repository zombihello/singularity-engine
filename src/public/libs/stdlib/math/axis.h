#ifndef AXIS_H
#define AXIS_H

//-----------------------------------------------------------------------------
// Axis types
//-----------------------------------------------------------------------------
enum axis_t
{
	AXIS_X			= 1 << 0,
	AXIS_Y			= 1 << 1,
	AXIS_Z			= 1 << 2,

	AXIS_RIGHT		= AXIS_X,
	AXIS_UP			= AXIS_Y,
	AXIS_FORWARD	= AXIS_Z,
};


enum axisUp_t
{
	AXIS_UP_PLUS_X,
	AXIS_UP_MINUS_X,
	AXIS_UP_PLUS_Y,
	AXIS_UP_MINUS_Y,
	AXIS_UP_PLUS_Z,
	AXIS_UP_MINUS_Z,
	AXIS_UP_NUM,
	AXIS_UP_DEFAULT	= AXIS_UP_PLUS_Y
};

#endif // !AXIS_H