#include "pch_materialsystem.h"
#include "stdlib/filetools.h"
#include "stexdoc/stex_compiled_doc.h"
#include "resourcesystem/iresourcesystem.h"
#include "materialsystem/texture.h"
#include "materialsystem/texture_factory.h"

/*
==================
CTextureFactory::Init
==================
*/
void CTextureFactory::Init()
{
	// Register the factory in the resource system
	Assert( g_pResourceSystem );
	flags = GetDefaultFlags();
	g_pResourceSystem->RegisterResourceFactory( RESOURCE_TYPE_TEXTURE, this );

	// Initialize a default texture
	pDefaultTexture = g_pResourceSystem->FindOrLoadResource( "//CORE/materials/default", RESOURCE_TYPE_TEXTURE );
	if ( !pDefaultTexture )
	{
		pDefaultTexture	   = g_pResourceSystem->CreateProceduralResource( "default", RESOURCE_TYPE_TEXTURE );
		CTexture* pTexture = (CTexture*)pDefaultTexture->GetData();

		uint8			data[4] = { 0xFF, 0x00, 0xFF, 0xFF };
		textureMipMap_t mipmap0 = {};
		mipmap0.sizeX			= 1;
		mipmap0.sizeY			= 1;
		mipmap0.sizeZ			= 1;

		studioAPISamplerCreateInfo_t studioAPISamplerInfo = {};
		studioAPISamplerInfo.filer						  = STUDIOAPI_SAMPLER_FILTER_POINT;
		studioAPISamplerInfo.addressModeU				  = STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
		studioAPISamplerInfo.addressModeV				  = STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
		studioAPISamplerInfo.addressModeW				  = STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
		studioAPISamplerInfo.minLod						  = -FLT_MAX;
		studioAPISamplerInfo.maxLod						  = FLT_MAX;
		pTexture->SetData( STUDIOAPI_TEXTURE_TYPE_2D, STUDIOAPI_PIXEL_FORMAT_RGBA8, 1, &mipmap0, 1, studioAPISamplerInfo, data, ARRAYSIZE( data ) );
	}
}

/*
==================
CTextureFactory::Shutdown
==================
*/
void CTextureFactory::Shutdown()
{
	// Unregister the factory from the resource system
	flags |= RESOURCE_FACTORY_FLAG_NOT_USED;
	g_pResourceSystem->UnRegisterResourceFactory( RESOURCE_TYPE_TEXTURE );

	// Remove the default texture
	pDefaultTexture = NULL;
}

/*
==================
CTextureFactory::CreateProceduralResource
==================
*/
TRefPtr<IRefCounted> CTextureFactory::CreateProceduralResource() const
{
	return new CTexture();
}

/*
==================
CTextureFactory::LoadResource
==================
*/
TRefPtr<IRefCounted> CTextureFactory::LoadResource( const char* pPath, uint32 loadFlags /* = RESOURCE_LOAD_FLAG_NONE */ ) const
{
	CSTEXCompiledTextureDoc stexCompiledTextureDoc;
	if ( !stexCompiledTextureDoc.LoadFromFile( S_GetFileExtension( pPath ) ? pPath : S_Sprintf( "%s.stex_c", pPath ).c_str() ) )
	{
		return false;
	}

	// Convert STX mipmap information to the engine format
	eastl::vector<textureMipMap_t> mipmaps;
	const eastl::vector<byte>&	 data = stexCompiledTextureDoc.GetData();
	mipmaps.resize( stexCompiledTextureDoc.GetNumMips() );
	for ( uint32 mipIdx = 0, numMips = stexCompiledTextureDoc.GetNumMips(); mipIdx < numMips; ++mipIdx )
	{
		const stexTextureMipMap_t srcMipmap	 = stexCompiledTextureDoc.GetMip( mipIdx );
		textureMipMap_t&		  destMipmap = mipmaps[mipIdx];
		destMipmap.sizeX					 = srcMipmap.sizeX;
		destMipmap.sizeY					 = srcMipmap.sizeY;
		destMipmap.sizeZ					 = srcMipmap.sizeZ;
	}

	// Initialize a sampler information
	studioAPISamplerCreateInfo_t studioAPISamplerInfo = {};
	studioAPISamplerInfo.filer						  = stexCompiledTextureDoc.GetFilter();
	studioAPISamplerInfo.addressModeU				  = stexCompiledTextureDoc.GetAddressModeU();
	studioAPISamplerInfo.addressModeV				  = stexCompiledTextureDoc.GetAddressModeV();
	studioAPISamplerInfo.addressModeW				  = stexCompiledTextureDoc.GetAddressModeW();
	studioAPISamplerInfo.maxAnisotropy				  = stexCompiledTextureDoc.GetMaxAnisotropy();
	studioAPISamplerInfo.minLod						  = -FLT_MAX;
	studioAPISamplerInfo.maxLod						  = FLT_MAX;

	// Create a new texture and initialize it
	TRefPtr<CTexture> pTexture = new CTexture();
	pTexture->SetData( stexCompiledTextureDoc.GetType(), stexCompiledTextureDoc.GetPixelFormat(), stexCompiledTextureDoc.GetNumLayers(), mipmaps.data(), stexCompiledTextureDoc.GetNumMips(), studioAPISamplerInfo, data.data(), (uint32)data.size() );
	return pTexture;
}

/*
==================
CTextureFactory::UnloadResource
==================
*/
void CTextureFactory::UnloadResource( IRefCounted* pResoruce ) const
{
	CTexture* pTexture = (CTexture*)pResoruce;
	pTexture->Clear();
}

/*
==================
CTextureFactory::GetDefaultResource
==================
*/
TRefPtr<IResource> CTextureFactory::GetDefaultResource() const
{
	return pDefaultTexture;
}

/*
==================
CTextureFactory::GetFormatType
==================
*/
const char* CTextureFactory::GetFormatType() const
{
	return "STEX";
}