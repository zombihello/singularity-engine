#ifndef PIXELFORMATINFOS_H
#define PIXELFORMATINFOS_H

#include "core/types.h"
#include "studiorender/studioapi/istudioapi_texture.h"

//-----------------------------------------------------------------------------
// Information about pixel format
//-----------------------------------------------------------------------------
struct pixelFormatInfo_t
{
	const achar*				pName;
	uint32						blockSizeX;
	uint32						blockSizeY;
	uint32						blockSizeZ;
	uint32						blockBytes;
	uint32						numComponents;
};
extern pixelFormatInfo_t		g_PixelFormatInfos[STUDIOAPI_PIXEL_NUM_FORMATS];

#endif // !PIXELFORMATINFOS_H