/**
 * @file
 * @addtogroup pixelformatinfos pixelformatinfos
 */

#ifndef PIXELFORMATINFOS_H
#define PIXELFORMATINFOS_H

#include "core/types.h"
#include "studiorender/studioapi/istudioapi_texture.h"

/**
 * @ingroup pixelformatinfos
 * @brief Information about pixel format
 */
struct pixelFormatInfo_t
{
	const achar*	pName;			/**< Pixel format name */
	uint32			blockSizeX;		/**< Block size by X */
	uint32			blockSizeY;		/**< Block size by Y */
	uint32			blockSizeZ;		/**< Block size by Z */
	uint32			blockBytes;		/**< Count bytes in one block */
	uint32			numComponents;	/**< Number components in a block */
};

/**
 * @ingroup pixelformatinfos
 * @brief Pixel format informations
 */
extern pixelFormatInfo_t		g_PixelFormatInfos[STUDIOAPI_PIXEL_NUM_FORMATS];

#endif // !PIXELFORMATINFOS_H