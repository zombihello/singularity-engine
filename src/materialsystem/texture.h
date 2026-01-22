#pragma once
#include "materialsystem/itexture.h"
#include "studiorender/istudio_renderresource.h"

//-----------------------------------------------------------------------------
// Texture
//-----------------------------------------------------------------------------
class CTexture : public TRefCounted<ITexture>, public TStudioRenderResource<IStudioRenderResource>
{
public:
	CTexture();

	// ITexture interface
	virtual void SetData( studioAPITextureType_t type, studioAPIPixelFormat_t pixelFormat, uint32 numLayers, const textureMipMap_t* pMipmaps, uint32 numMipmaps, const studioAPISamplerCreateInfo_t& samplerInfo, const byte* pData = NULL, uint32 dataSize = 0 ) override;

	virtual studioAPIPixelFormat_t GetPixelFormat() const override;
	virtual uint32				   GetNumLayers() const override;
	virtual uint32				   GetNumMips() const override;
	virtual const textureMipMap_t& GetMip( uint32 mipLevel ) const override;
	virtual studioAPITextureType_t GetType() const override;
	virtual IStudioAPITexture*	   GetStudioAPITexture() const override;
	virtual IStudioAPISampler*	   GetStudioAPISampler() const override;

	void Clear();

private:
	// IStudioRenderResource interface
	virtual void InitStudioAPI() override;
	virtual void ReleaseStudioAPI() override;

	studioAPITextureType_t		 type;
	studioAPIPixelFormat_t		 pixelFormat;
	studioAPISamplerCreateInfo_t studioAPISamplerCreateInfo;
	uint32						 numLayers;
	TRefPtr<IStudioAPITexture>	 pStudioAPITexture;
	TRefPtr<IStudioAPISampler>	 pStudioAPISampler;
	eastl::vector<textureMipMap_t> mipmaps;
	eastl::vector<byte>			 data;
};