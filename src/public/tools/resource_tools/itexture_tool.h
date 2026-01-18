#pragma once
#include "appframework/iappsystem.h"
#include "studiorender/studioapi/istudioapi_texture.h"

//-----------------------------------------------------------------------------
// Texture tool interface
//-----------------------------------------------------------------------------
struct resourceToolCompileTextureParams_t
{
	bool						  bGenerateMipMaps;
	studioAPITextureType_t		  type;
	studioAPISamplerAddressMode_t addressModeU;
	studioAPISamplerAddressMode_t addressModeV;
	studioAPISamplerAddressMode_t addressModeW;
	studioAPISamplerFilter_t	  filter;
	studioAPIPixelFormat_t		  pixelFormat;
	uint32						  maxAnisotropy;
	uint32						  numSrcPaths;
	const char**				  ppSrcPaths;
	const char*				  pDestPath;
};

#define TEXTURE_TOOL_INTERFACE_VERSION "STextureTool001"
class ITextureTool : public IAppSystem
{
public:
	virtual bool CompileTexture( const resourceToolCompileTextureParams_t& compileParams ) const = 0;
	virtual bool IsSupportPixelFormat( studioAPIPixelFormat_t pixelFormat ) const				 = 0;
};