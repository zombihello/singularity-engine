/**
 * @file
 * @addtogroup resource_tools resource_tools
 */

#ifndef ITEXTURE_TOOL_H
#define ITEXTURE_TOOL_H

#include "appframework/iappsystem.h"
#include "studiorender/studioapi/istudioapi_texture.h"

/**
 * @ingroup resource_tools
 * @brief Texture tool interface version
 */
#define TEXTURE_TOOL_INTERFACE_VERSION "STextureTool001"

/**
 * @ingroup resource_tools
 * @brief Compile texture parameters
 */
struct resourceToolCompileTextureParams_t
{
	bool								bGenerateMipMaps;	/**< Is need generate mipmaps */
	studioAPITextureType_t				type;				/**< Texture type */
	studioAPISamplerAddressMode_t		addressModeU;		/**< Address mode for U coord */
	studioAPISamplerAddressMode_t		addressModeV;		/**< Address mode for V coord */
	studioAPISamplerAddressMode_t		addressModeW;		/**< Address mode for W coord */
	studioAPISamplerFilter_t			filter;				/**< Sampler filter */
	studioAPIPixelFormat_t				pixelFormat;		/**< Pixel format */
	uint32								maxAnisotropy;		/**< Anisotropy value clamp */
	uint32								numSrcPaths;		/**< Source paths number */
	const achar**						ppSrcPaths;			/**< Paths to source textures */
	const achar*						pDestPath;			/**< Path to a destination texture (without file extension) */
};

/**
 * @ingroup resource_tools
 * @brief Texture tool interface
 */
class ITextureTool : public IAppSystem
{
public:
	/**
	 * @brief Compile a texture and save in the file system
	 * @param compileParams		Compile parameters
	 * @return Return TRUE if the texture successfully compiled, otherwise FALSE
	 */
	virtual bool CompileTexture( const resourceToolCompileTextureParams_t& compileParams ) const = 0;

	/**
	 * @brief Is a pixel format support by the tool
	 * @param pixelFormat	Pixel format to check
	 * @return Return TRUE if the pixel format is support by the tool, otherwise FALSE
	 */
	virtual bool IsSupportPixelFormat( studioAPIPixelFormat_t pixelFormat ) const = 0;
};

#endif // !ITEXTURE_TOOL_H