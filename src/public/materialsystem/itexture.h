#ifndef ITEXTURE_H
#define ITEXTURE_H

#include "stdlib/refcount.h"
#include "studiorender/studioapi/istudioapi_texture.h"

//-----------------------------------------------------------------------------
// Texture interface
//-----------------------------------------------------------------------------
struct textureMipMap_t
{
	uint32		sizeX;
	uint32		sizeY;
	uint32		sizeZ;
};


class ITexture : public IRefCounted
{
public:
	virtual ~ITexture() {}

	virtual void SetData( studioAPITextureType_t type, studioAPIPixelFormat_t pixelFormat, uint32 numLayers, const textureMipMap_t* pMipmaps, uint32 numMipmaps, const studioAPISamplerCreateInfo_t& samplerInfo, const byte* pData = NULL, uint32 dataSize = 0 ) = 0;

	virtual studioAPIPixelFormat_t GetPixelFormat() const = 0;
	virtual uint32 GetNumLayers() const = 0;
	virtual uint32 GetNumMips() const = 0;
	virtual const textureMipMap_t& GetMip( uint32 mipLevel ) const = 0;
	virtual studioAPITextureType_t GetType() const = 0;
	virtual IStudioAPITexture* GetStudioAPITexture() const = 0;
	virtual IStudioAPISampler* GetStudioAPISampler() const = 0;
};

#endif // !ITEXTURE_H