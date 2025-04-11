#include "pch_materialsystem.h"
#include "materialsystem/texture.h"
/*
==================
CTexture::CTexture
==================
*/
CTexture::CTexture()
	: type( STUDIOAPI_TEXTURE_TYPE_1D )
	, pixelFormat( STUDIOAPI_PIXEL_FORMAT_UNKNOWN )
	, numLayers( 0 )
{
	Mem_Memzero( &studioAPISamplerCreateInfo, sizeof( studioAPISamplerCreateInfo_t ) );
}

/*
==================
CTexture::InitStudioAPI
==================
*/
void CTexture::InitStudioAPI()
{
	// Create a StudioAPI texture
	Assert( !mipmaps.empty() );
	const textureMipMap_t&		mipmap0 = mipmaps[0];
	pStudioAPITexture = g_pStudioAPI->CreateTexture( type, mipmap0.sizeX, mipmap0.sizeY, mipmap0.sizeZ, numLayers, ( uint32 )mipmaps.size(), STUDIOAPI_TEXTURE_USAGE_FLAG_TEXTURE, pixelFormat, data.data() );
	data.clear();

	// Create a StudioAPI sampler
	pStudioAPISampler = g_pStudioAPI->CreateSampler( studioAPISamplerCreateInfo );
}

/*
==================
CTexture::ReleaseStudioAPI
==================
*/
void CTexture::ReleaseStudioAPI()
{
	pStudioAPITexture = NULL;
	pStudioAPISampler = NULL;
}

/*
==================
CTexture::Clear
==================
*/
void CTexture::Clear()
{
	// Clear some fields
	type		= STUDIOAPI_TEXTURE_TYPE_1D;
	pixelFormat = STUDIOAPI_PIXEL_FORMAT_UNKNOWN;
	numLayers	= 0;
	mipmaps.clear();

	// Release StudioAPI resources
	Studio_BeginReleaseResourceSafe<CTexture>( this );
}

/*
==================
CTexture::SetData
==================
*/
void CTexture::SetData( studioAPITextureType_t type, studioAPIPixelFormat_t pixelFormat, uint32 numLayers, const textureMipMap_t* pMipmaps, uint32 numMipmaps, const studioAPISamplerCreateInfo_t& samplerInfo, const byte* pData /* = NULL */, uint32 dataSize /* = 0 */ )
{
	// Copy mipmaps information
	mipmaps.clear();
	mipmaps.resize( numMipmaps );
	Mem_Memcpy( mipmaps.data(), pMipmaps, numMipmaps * sizeof( textureMipMap_t ) );

	// Copy a new texture parameters
	CTexture::type				= type;
	CTexture::pixelFormat		= pixelFormat;
	CTexture::numLayers			= numLayers;
	studioAPISamplerCreateInfo	= samplerInfo;

	// Copy a texture data
	data.clear();
	data.resize( dataSize );
	Mem_Memcpy( data.data(), pData, dataSize );

	// Initialize StudioAPI resources
	Studio_BeginUpdateResourceSafe<CTexture>( this );
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
CTexture::GetNumLayers
==================
*/
uint32 CTexture::GetNumLayers() const
{
	return numLayers;
}

/*
==================
CTexture::GetNumMips
==================
*/
uint32 CTexture::GetNumMips() const
{
	return ( uint32 )mipmaps.size();
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
CTexture::GetType
==================
*/
studioAPITextureType_t CTexture::GetType() const
{
	return type;
}

/*
==================
CTexture::GetStudioAPITexture
==================
*/
IStudioAPITexture* CTexture::GetStudioAPITexture() const
{
	return pStudioAPITexture;
}

/*
==================
CTexture::GetStudioAPISampler
==================
*/
IStudioAPISampler* CTexture::GetStudioAPISampler() const
{
	return pStudioAPISampler;
}