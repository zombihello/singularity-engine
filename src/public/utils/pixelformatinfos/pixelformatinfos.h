#pragma once
#include "tier1/types.h"
#include "studiorender/studioapi/istudioapi_texture.h"

//-----------------------------------------------------------------------------
// Information about pixel format
//-----------------------------------------------------------------------------
struct pixelFormatInfo_t
{
	const char* pName;
	uint32		 blockSizeX;
	uint32		 blockSizeY;
	uint32		 blockSizeZ;
	uint32		 blockBytes;
	uint32		 numComponents;
};
extern pixelFormatInfo_t g_PixelFormatInfos[STUDIOAPI_PIXEL_NUM_FORMATS];