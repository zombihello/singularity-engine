#include "pch_materialsystem.h"
#include "materialsystem/texture.h"

/*
==================
CTextureResource::CTextureResource
==================
*/
CTextureResource::CTextureResource()
	: type( STUDIOAPI_TEXTURE_TYPE_1D )
	, pixelFormat( STUDIOAPI_PIXEL_FORMAT_UNKNOWN )
	, usageFlags( 0 )
	, sizeX( 0 )
	, sizeY( 0 )
	, sizeZ( 0 )
	, numLayers( 0 )
	, numMipmaps( 0 )
{
	Mem_Memzero( &studioAPISamplerCreateInfo, sizeof( studioAPISamplerCreateInfo_t ) );
}

/*
==================
CTextureResource::Update
==================
*/
void CTextureResource::Update( studioAPITextureType_t type, studioAPIPixelFormat_t pixelFormat, uint32 sizeX, uint32 sizeY, uint32 sizeZ, uint32 numLayers, uint32 numMipmaps, uint32 usageFlags, const studioAPISamplerCreateInfo_t& samplerInfo, const byte* pData, uint32 dataSize )
{
	// Copy new parameters
	PROFILER_SCOPE_FUNC();
	CTextureResource::type		  = type;
	CTextureResource::pixelFormat = pixelFormat;
	CTextureResource::usageFlags  = usageFlags;
	CTextureResource::sizeX		  = sizeX;
	CTextureResource::sizeY		  = sizeY;
	CTextureResource::sizeZ		  = sizeZ;
	CTextureResource::numLayers	  = numLayers;
	CTextureResource::numMipmaps  = numMipmaps;
	studioAPISamplerCreateInfo	  = samplerInfo;

	// Copy the new resource data
	data.clear();
	data.resize( dataSize );
	Mem_Memcpy( data.data(), pData, dataSize );

	// Begin update the resource in the render thread
	Studio_BeginUpdateResource( this );
}

/*
==================
CTextureResource::Clear
==================
*/
void CTextureResource::Clear()
{
	// Reset all fields and data
	PROFILER_SCOPE_FUNC();
	type		= STUDIOAPI_TEXTURE_TYPE_1D;
	pixelFormat = STUDIOAPI_PIXEL_FORMAT_UNKNOWN;
	usageFlags	= 0;
	sizeX		= 0;
	sizeY		= 0;
	sizeZ		= 0;
	numLayers	= 0;
	numMipmaps	= 0;
	Mem_Memzero( &studioAPISamplerCreateInfo, sizeof( studioAPISamplerCreateInfo_t ) );
	data.clear();

	// Begin release the resource in the render thread
	Studio_BeginReleaseResource( this );
}

/*
==================
CTextureResource::InitStudioAPI
==================
*/
void CTextureResource::InitStudioAPI()
{
	// Create a StudioAPI texture and sample (if we have flag `STUDIOAPI_TEXTURE_USAGE_FLAG_TEXTURE`)
	pStudioAPITexture = g_pStudioAPI->CreateTexture( type, sizeX, sizeY, sizeZ, numLayers, numMipmaps, usageFlags, pixelFormat, !data.empty() ? data.data() : NULL );
	if ( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_TEXTURE )
	{
		pStudioAPISampler = g_pStudioAPI->CreateSampler( studioAPISamplerCreateInfo );
	}
	data.clear();
}

/*
==================
CTextureResource::ReleaseStudioAPI
==================
*/
void CTextureResource::ReleaseStudioAPI()
{
	pStudioAPITexture = NULL;
	pStudioAPISampler = NULL;
}

/*
==================
CTextureResource::FinalRelease
==================
*/
void CTextureResource::FinalRelease()
{
	if ( IsNeedDeferredDestroy() )
	{
		Studio_BeginDeleteResource( this );
	}
	else
	{
		delete this;
	}
}

/*
==================
CTextureResource::GetType
==================
*/
studioAPITextureType_t CTextureResource::GetType() const
{
	return type;
}

/*
==================
CTextureResource::GetPixelFormat
==================
*/
studioAPIPixelFormat_t CTextureResource::GetPixelFormat() const
{
	return pixelFormat;
}

/*
==================
CTextureResource::GetSizeX
==================
*/
uint32 CTextureResource::GetSizeX() const
{
	return sizeX;
}

/*
==================
CTextureResource::GetSizeY
==================
*/
uint32 CTextureResource::GetSizeY() const
{
	return sizeY;
}

/*
==================
CTextureResource::GetSizeZ
==================
*/
uint32 CTextureResource::GetSizeZ() const
{
	return sizeZ;
}

/*
==================
CTextureResource::GetNumLayers
==================
*/
uint32 CTextureResource::GetNumLayers() const
{
	return numLayers;
}

/*
==================
CTextureResource::GetUsageFlags
==================
*/
uint32 CTextureResource::GetUsageFlags() const
{
	return usageFlags;
}

/*
==================
CTextureResource::GetStudioAPITexture
==================
*/
IStudioAPITexture* CTextureResource::GetStudioAPITexture() const
{
	return pStudioAPITexture;
}

/*
==================
CTextureResource::GetStudioAPISampler
==================
*/
IStudioAPISampler* CTextureResource::GetStudioAPISampler() const
{
	return pStudioAPISampler;
}

/*
==================
CTexture::CTexture
==================
*/
CTexture::CTexture( IResource* pResource )
	: CResourceData<ITexture>( pResource )
	, type( STUDIOAPI_TEXTURE_TYPE_1D )
	, pixelFormat( STUDIOAPI_PIXEL_FORMAT_UNKNOWN )
	, usageFlags( 0 )
	, numLayers( 0 )
	, pStudioResource( new CTextureResource() )
{
}

/*
==================
CTexture::~CTexture
==================
*/
CTexture::~CTexture()
{
	Destroy();
}

/*
==================
CTexture::Init
==================
*/
void CTexture::Init( studioAPITextureType_t type, studioAPIPixelFormat_t pixelFormat, uint32 numLayers, const textureMipMap_t* pMipmaps, uint32 numMipmaps, uint32 usageFlags, const studioAPISamplerCreateInfo_t& samplerInfo, const byte* pData /* = NULL */, uint32 dataSize /* = 0 */ )
{
	// Insert a fence to make sure that the render thread not using the studio resource
	PROFILER_SCOPE_FUNC();
	CStudioRenderCmdFence& renderCmdFence = pStudioResource->GetRenderCmdFence();
	renderCmdFence.InsertFence();

	// Copy a new texture parameters
	CTexture::type		  = type;
	CTexture::pixelFormat = pixelFormat;
	CTexture::usageFlags  = usageFlags;
	CTexture::numLayers	  = numLayers;

	// Copy mipmaps information
	mipmaps.clear();
	mipmaps.resize( numMipmaps );
	Mem_Memcpy( mipmaps.data(), pMipmaps, numMipmaps * sizeof( textureMipMap_t ) );

	// Wait the render command fence
	renderCmdFence.Wait();

	// Update the studio resource
	Assert( !mipmaps.empty() );
	const textureMipMap_t& mipmap0 = mipmaps[0];
	pStudioResource->Update( type, pixelFormat, mipmap0.sizeX, mipmap0.sizeY, mipmap0.sizeZ, numLayers, (uint32)mipmaps.size(), usageFlags, samplerInfo, pData, dataSize );
}

/*
==================
CTexture::Destroy
==================
*/
void CTexture::Destroy()
{
	// Insert a fence to make sure that the render thread not using the studio resource
	PROFILER_SCOPE_FUNC();
	CStudioRenderCmdFence& renderCmdFence = pStudioResource->GetRenderCmdFence();
	renderCmdFence.InsertFence();

	// Clear some fields
	type		= STUDIOAPI_TEXTURE_TYPE_1D;
	pixelFormat = STUDIOAPI_PIXEL_FORMAT_UNKNOWN;
	usageFlags	= 0;
	numLayers	= 0;
	mipmaps.clear();

	// Wait the render command fence
	renderCmdFence.Wait();

	// Clear the studio resource
	pStudioResource->Clear();
}

/*
==================
CTexture::GetType
==================
*/
studioAPITextureType_t CTexture::GetType() const
{
	return type;
}

/*
==================
CTexture::GetPixelFormat
==================
*/
studioAPIPixelFormat_t CTexture::GetPixelFormat() const
{
	return pixelFormat;
}

/*
==================
CTexture::GetUsageFlags
==================
*/
uint32 CTexture::GetUsageFlags() const
{
	return usageFlags;
}

/*
==================
CTexture::GetNumMips
==================
*/
uint32 CTexture::GetNumMips() const
{
	return (uint32)mipmaps.size();
}

/*
==================
CTexture::GetMip
==================
*/
const textureMipMap_t& CTexture::GetMip( uint32 mipLevel ) const
{
	Assert( mipLevel < mipmaps.size() );
	return mipmaps[mipLevel];
}

/*
==================
CTexture::GetNumLayers
==================
*/
uint32 CTexture::GetNumLayers() const
{
	return numLayers;
}

/*
==================
CTexture::GetStudioResource
==================
*/
ITextureResource* CTexture::GetStudioResource() const
{
	return pStudioResource;
}
