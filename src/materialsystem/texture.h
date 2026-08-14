#pragma once
#include "tier1/threading.h"
#include "materialsystem/itexture.h"
#include "studiorender/istudio_renderresource.h"

//-----------------------------------------------------------------------------
// A texture resource which is owned by the render thread
//-----------------------------------------------------------------------------
class CTextureResource : public CDebugNamed<CRefCounted<ITextureResource>>, public CStudioRenderResource<IStudioRenderResource>
{
public:
	// ITextureResource interface
	virtual studioAPITextureType_t GetType() const override;
	virtual studioAPIPixelFormat_t GetPixelFormat() const override;
	virtual uint32				   GetSizeX() const override;
	virtual uint32				   GetSizeY() const override;
	virtual uint32				   GetSizeZ() const override;
	virtual uint32				   GetNumLayers() const override;
	virtual uint32				   GetUsageFlags() const override;
	virtual IStudioAPITexture*	   GetStudioAPITexture() const override;
	virtual IStudioAPISampler*	   GetStudioAPISampler() const override;

	CTextureResource( const char* pDebugName = "" );

	void				   Update( studioAPITextureType_t type, studioAPIPixelFormat_t pixelFormat, uint32 sizeX, uint32 sizeY, uint32 sizeZ, uint32 numLayers, uint32 numMipmaps, uint32 usageFlags, const studioAPISamplerCreateInfo_t& samplerInfo, const byte* pData = NULL, uint32 dataSize = 0 );
	void				   Clear();
	CStudioRenderCmdFence& GetRenderCmdFence();

protected:
	// IRefCounted interface
	virtual void FinalRelease() override;

private:
	// IStudioRenderResource interface
	virtual void InitStudioAPI() override;
	virtual void ReleaseStudioAPI() override;

	studioAPITextureType_t		 type;
	studioAPIPixelFormat_t		 pixelFormat;
	uint32						 usageFlags;
	studioAPISamplerCreateInfo_t studioAPISamplerCreateInfo;
	uint32						 sizeX;
	uint32						 sizeY;
	uint32						 sizeZ;
	uint32						 numLayers;
	uint32						 numMipmaps;
	CStudioRenderCmdFence		 renderCmdFence;
	CRefPtr<IStudioAPITexture>	 pStudioAPITexture;
	CRefPtr<IStudioAPISampler>	 pStudioAPISampler;
	eastl::vector<byte>			 data;
};

//-----------------------------------------------------------------------------
// A texture
//-----------------------------------------------------------------------------
class CTexture : public CResourceData<ITexture>
{
public:
	// ITexture interface
	virtual void Init( studioAPITextureType_t type, studioAPIPixelFormat_t pixelFormat, uint32 numLayers, const textureMipMap_t* pMipmaps, uint32 numMipmaps, uint32 usageFlags, const studioAPISamplerCreateInfo_t& samplerInfo, const byte* pData = NULL, uint32 dataSize = 0 ) override;
	virtual void Destroy() override;

	virtual studioAPITextureType_t GetType() const override;
	virtual studioAPIPixelFormat_t GetPixelFormat() const override;
	virtual uint32				   GetUsageFlags() const override;
	virtual uint32				   GetNumMips() const override;
	virtual const textureMipMap_t& GetMip( uint32 mipLevel ) const override;
	virtual uint32				   GetNumLayers() const override;
	virtual ITextureResource*	   GetStudioResource() const override;

	CTexture( IResource* pResource );
	~CTexture();

private:
	studioAPITextureType_t		   type;
	studioAPIPixelFormat_t		   pixelFormat;
	uint32						   usageFlags;
	uint32						   numLayers;
	CRefPtr<CTextureResource>	   pStudioResource;
	eastl::vector<textureMipMap_t> mipmaps;
};

DECLARE_RESOURCE_TYPE( CTexture, RESOURCE_TYPE_TEXTURE );
#include "materialsystem/texture.inl"
