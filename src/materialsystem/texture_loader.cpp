#include "pch_materialsystem.h"
#include "tier1/filetools.h"
#include "utils/stexdoc/stex_compiled_doc.h"
#include "materialsystem/texture.h"
#include "materialsystem/texture_loader.h"

/*
==================
CTextureLoader::Load
==================
*/
bool CTextureLoader::Load( const char* pPath, IResourceData* pData ) const
{
	Assert( pData );
	CSTEXCompiledTextureDoc stexCompiledTextureDoc;
	if ( !stexCompiledTextureDoc.LoadFromFile( S_GetFileExtension( pPath ) ? pPath : S_Sprintf( "%s.stex_c", pPath ).c_str() ) )
	{
		return false;
	}

	// Convert STX mipmap information to the engine format
	eastl::vector<textureMipMap_t> mipmaps;
	const eastl::vector<byte>&	   data = stexCompiledTextureDoc.GetData();
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
	studioAPISamplerInfo.minLod						  = S_MinValue<float>();
	studioAPISamplerInfo.maxLod						  = S_MaxValue<float>();

	// Initialize the texture
	CTexture* pTexture = (CTexture*)pData;
	pTexture->Init( stexCompiledTextureDoc.GetType(), stexCompiledTextureDoc.GetPixelFormat(), stexCompiledTextureDoc.GetNumLayers(), mipmaps.data(), stexCompiledTextureDoc.GetNumMips(), studioAPISamplerInfo, data.data(), (uint32)data.size() );
	return true;
}

/*
==================
CTextureLoader::GetFormatName
==================
*/
const char* CTextureLoader::GetFormatName() const
{
	return "Singularity Texture";
}
