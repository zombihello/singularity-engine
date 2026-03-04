#pragma once
#include "tier0/types.h"
#include "tier1/math/rect.h"

//-----------------------------------------------------------------------------
// Constants and types
//-----------------------------------------------------------------------------
typedef uint32 displayHandle_t;
typedef uint32 windowId_t;

enum
{
	INVALID_DISPLAY_HANDLE	 = INVALID_INDEX,
	INVALID_WINDOW_ID		 = INVALID_INDEX,
	WINDOW_SIZE_FROM_DISPLAY = -1
};

enum displayOrientation_t
{
	DISPLAY_ORIENTATION_UNKNOWN,			// The display orientation can't be determined
	DISPLAY_ORIENTATION_LANDSCAPE,			// The display is in landscape mode, with the right side up, relative to portrait mode
	DISPLAY_ORIENTATION_LANDSCAPE_FLIPPED,	// The display is in landscape mode, with the left side up, relative to portrait mode
	DISPLAY_ORIENTATION_PORTRAIT,			// The display is in portrait mode
	DISPLAY_ORIENTATION_PORTRAIT_FLIPPED	// The display is in portrait mode, upside down
};

enum windowMode_t
{
	WINDOW_MODE_HIDDEN,
	WINDOW_MODE_WINDOWED,
	WINDOW_MODE_BORDERLESS_FULLSCREEN,
	WINDOW_MODE_EXCLUSIVE_FULLSCREEN
};

struct displayMode_t
{
	uint32 width;
	uint32 height;
	float  refreshRate;
};

struct display_t
{
	displayHandle_t		 handle;
	const char*			 pName;
	rect_t<uint32>		 bounds;
	rect_t<uint32>		 usableBounds;
	displayOrientation_t orientation;
};

struct windowCreateInfo_t
{
	const char*		pTitle;
	windowMode_t	mode;
	displayHandle_t displayHandle;
	uint32			width;
	uint32			height;
	float			refreshRate;
};
