#include "pch_materialsystem.h"
#include "materialsystem/texture.h"

/*
==================
CTextureResource::CTextureResource
==================
*/
CTextureResource::CTextureResource( studioAPITextureType_t type, studioAPIPixelFormat_t pixelFormat, uint32 sizeX, uint32 sizeY, uint32 sizeZ, uint32 numLayers, uint32 numMipmaps, const studioAPISamplerCreateInfo_t& samplerInfo, const byte* pData /* = NULL */, uint32 dataSize /* = 0 */ )
	: type( type )
	, pixelFormat( pixelFormat )
	, studioAPISamplerCreateInfo( samplerInfo )
	, sizeX( sizeX )
	, sizeY( sizeY )
	, sizeZ( sizeZ )
	, numLayers( numLayers )
	, numMipmaps( numMipmaps )
{
	data.resize( dataSize );
	Mem_Memcpy( data.data(), pData, dataSize );
}

/*
==================
CTextureResource::InitStudioAPI
==================
*/
void CTextureResource::InitStudioAPI()
{
	Assert( !data.empty() );
	pStudioAPITexture = g_pStudioAPI->CreateTexture( type, sizeX, sizeY, sizeZ, numLayers, numMipmaps, STUDIOAPI_TEXTURE_USAGE_FLAG_TEXTURE, pixelFormat, data.data() );
	pStudioAPISampler = g_pStudioAPI->CreateSampler( studioAPISamplerCreateInfo );
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
	, numLayers( 0 )
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
void CTexture::Init( studioAPITextureType_t type, studioAPIPixelFormat_t pixelFormat, uint32 numLayers, const textureMipMap_t* pMipmaps, uint32 numMipmaps, const studioAPISamplerCreateInfo_t& samplerInfo, const byte* pData /* = NULL */, uint32 dataSize /* = 0 */ )
{
	// Copy a new texture parameters
	CTexture::type		  = type;
	CTexture::pixelFormat = pixelFormat;
	CTexture::numLayers	  = numLayers;

	// Copy mipmaps information
	mipmaps.clear();
	mipmaps.resize( numMipmaps );
	Mem_Memcpy( mipmaps.data(), pMipmaps, numMipmaps * sizeof( textureMipMap_t ) );

	// Create a studio resource
	Assert( !mipmaps.empty() );
	const textureMipMap_t& mipmap0 = mipmaps[0];
	{
		CScopeLock scopeLock( resourceCreationMutex );
		pStudioResource = new CTextureResource( type, pixelFormat, mipmap0.sizeX, mipmap0.sizeY, mipmap0.sizeZ, numLayers, (uint32)mipmaps.size(), samplerInfo, pData, dataSize );
		Studio_BeginInitResource( pStudioResource );
	}

	// Trigger event that the studio resource has been changed
	onStudioResourceChanged.Invoke( this );
}

/*
==================
CTexture::Destroy
==================
*/
void CTexture::Destroy()
{
	// Clear some fields
	type		= STUDIOAPI_TEXTURE_TYPE_1D;
	pixelFormat = STUDIOAPI_PIXEL_FORMAT_UNKNOWN;
	numLayers	= 0;
	mipmaps.clear();

	// Release the studio resource
	{
		CScopeLock scopeLock( resourceCreationMutex );
		if ( pStudioResource )
		{
			Studio_BeginReleaseResource( pStudioResource );
			pStudioResource = NULL;
		}
	}

	// Trigger event that the studio resource has been changed
	onStudioResourceChanged.Invoke( this );
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
	CScopeLock scopeLock( resourceCreationMutex );
	return pStudioResource;
}

/*
==================
CTexture::OnStudioResourceChanged
==================
*/
ITexture::IOnStudioResourceChanged* CTexture::OnStudioResourceChanged() const
{
	return &onStudioResourceChanged;
}
