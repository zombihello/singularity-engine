#pragma once
#include "tier1/refcount.h"
#include "studiorender/studioapi/istudioapi_texture.h"
#include "studiorender/istudio_renderresource.h"
#include "resourcesystem/iresource.h"

//-----------------------------------------------------------------------------
// Texture mip map info
//-----------------------------------------------------------------------------
struct textureMipMap_t
{
	uint32 sizeX;
	uint32 sizeY;
	uint32 sizeZ;
};

//-----------------------------------------------------------------------------
// A texture resource interface which is owned by the render thread
//-----------------------------------------------------------------------------
class ITextureResource : public IRefCounted
{
public:
	virtual ~ITextureResource() {}

	virtual studioAPITextureType_t GetType() const			   = 0;
	virtual studioAPIPixelFormat_t GetPixelFormat() const	   = 0;
	virtual uint32				   GetSizeX() const			   = 0;
	virtual uint32				   GetSizeY() const			   = 0;
	virtual uint32				   GetSizeZ() const			   = 0;
	virtual uint32				   GetNumLayers() const		   = 0;
	virtual IStudioAPITexture*	   GetStudioAPITexture() const = 0;
	virtual IStudioAPISampler*	   GetStudioAPISampler() const = 0;
};

//-----------------------------------------------------------------------------
// A texture interface
//-----------------------------------------------------------------------------
class ITexture : public IResourceData
{
public:
	virtual ~ITexture() {}

	virtual void Init( studioAPITextureType_t type, studioAPIPixelFormat_t pixelFormat, uint32 numLayers, const textureMipMap_t* pMipmaps, uint32 numMipmaps, const studioAPISamplerCreateInfo_t& samplerInfo, const byte* pData = NULL, uint32 dataSize = 0 ) = 0;
	virtual void Destroy()																																																									   = 0;

	virtual studioAPITextureType_t GetType() const				   = 0;
	virtual studioAPIPixelFormat_t GetPixelFormat() const		   = 0;
	virtual uint32				   GetNumMips() const			   = 0;
	virtual const textureMipMap_t& GetMip( uint32 mipLevel ) const = 0;
	virtual uint32				   GetNumLayers() const			   = 0;
	virtual ITextureResource*	   GetStudioResource() const	   = 0;
};

DECLARE_RESOURCE_TYPE( ITexture, RESOURCE_TYPE_TEXTURE );
