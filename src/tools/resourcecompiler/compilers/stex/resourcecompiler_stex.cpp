#include <cstdlib>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <compressonator.h>

#include "tier1/tier1.h"
#include "tier1/filetools.h"
#include "utils/stexdoc/stex_source_doc.h"
#include "utils/stexdoc/stex_compiled_doc.h"
#include "tools/resourcecompiler/iresourcecompiler.h"

/*
==================
STex_ConvPixelFormatToCmpFormat
==================
*/
static CMP_FORMAT STex_ConvPixelFormatToCmpFormat( studioAPIPixelFormat_t pixelFormat )
{
	switch ( pixelFormat )
	{
		// Channel component formats
	case STUDIOAPI_PIXEL_FORMAT_RGBA8: return CMP_FORMAT_RGBA_8888;
	case STUDIOAPI_PIXEL_FORMAT_RG8: return CMP_FORMAT_RG_8;
	case STUDIOAPI_PIXEL_FORMAT_R8: return CMP_FORMAT_R_8;
	case STUDIOAPI_PIXEL_FORMAT_RGBA16: return CMP_FORMAT_RGBA_16;
	case STUDIOAPI_PIXEL_FORMAT_RG16: return CMP_FORMAT_RG_16;
	case STUDIOAPI_PIXEL_FORMAT_R16: return CMP_FORMAT_R_16;
	case STUDIOAPI_PIXEL_FORMAT_RGBA32F: return CMP_FORMAT_RGBA_32F;
	case STUDIOAPI_PIXEL_FORMAT_RG32F: return CMP_FORMAT_RG_32F;
	case STUDIOAPI_PIXEL_FORMAT_R32F: return CMP_FORMAT_R_32F;
	case STUDIOAPI_PIXEL_FORMAT_RGBA16F: return CMP_FORMAT_RGBA_16F;
	case STUDIOAPI_PIXEL_FORMAT_RG16F: return CMP_FORMAT_RG_16F;
	case STUDIOAPI_PIXEL_FORMAT_R16F:
		return CMP_FORMAT_R_16F;

		// Compression formats
	case STUDIOAPI_PIXEL_FORMAT_BC1: return CMP_FORMAT_BC1;
	case STUDIOAPI_PIXEL_FORMAT_BC2: return CMP_FORMAT_BC2;
	case STUDIOAPI_PIXEL_FORMAT_BC3: return CMP_FORMAT_BC3;
	case STUDIOAPI_PIXEL_FORMAT_BC4: return CMP_FORMAT_BC4;
	case STUDIOAPI_PIXEL_FORMAT_BC5: return CMP_FORMAT_BC5;
	case STUDIOAPI_PIXEL_FORMAT_BC6H: return CMP_FORMAT_BC6H;
	case STUDIOAPI_PIXEL_FORMAT_BC7:
		return CMP_FORMAT_BC7;

		// Other formats
	case STUDIOAPI_PIXEL_FORMAT_DEPTH16:
	case STUDIOAPI_PIXEL_FORMAT_DEPTH32F:
	case STUDIOAPI_PIXEL_FORMAT_DEPTH16_STENCIL8:
	case STUDIOAPI_PIXEL_FORMAT_DEPTH24_STENCIL8:
	case STUDIOAPI_PIXEL_FORMAT_DEPTH32F_STENCIL8:
	default:
		Error( "ResourceCompilerSTex: Unsupported pixel format 0x%X", pixelFormat );
		Assert( false );
		return CMP_FORMAT_Unknown;
	}
}

/*
==================
STex_GetNumChannels
==================
*/
static uint32 STex_GetNumChannels( CMP_FORMAT cmpFormat )
{
	switch ( cmpFormat )
	{
		// 4 channels
	case CMP_FORMAT_RGBA_8888_S:
	case CMP_FORMAT_ARGB_8888_S:
	case CMP_FORMAT_ARGB_8888:
	case CMP_FORMAT_ABGR_8888:
	case CMP_FORMAT_RGBA_8888:
	case CMP_FORMAT_BGRA_8888:
	case CMP_FORMAT_ARGB_2101010:
	case CMP_FORMAT_RGBA_1010102:
	case CMP_FORMAT_ARGB_16:
	case CMP_FORMAT_ABGR_16:
	case CMP_FORMAT_RGBA_16:
	case CMP_FORMAT_BGRA_16:
	case CMP_FORMAT_RGBE_32F:
	case CMP_FORMAT_ARGB_16F:
	case CMP_FORMAT_ABGR_16F:
	case CMP_FORMAT_RGBA_16F:
	case CMP_FORMAT_BGRA_16F:
	case CMP_FORMAT_ARGB_32F:
	case CMP_FORMAT_ABGR_32F:
	case CMP_FORMAT_RGBA_32F:
	case CMP_FORMAT_BGRA_32F:
		return 4;

		// 3 channels
	case CMP_FORMAT_RGB_888:
	case CMP_FORMAT_RGB_888_S:
	case CMP_FORMAT_BGR_888:
	case CMP_FORMAT_RGB_32F:
	case CMP_FORMAT_BGR_32F:
		return 3;

		// 2 channels
	case CMP_FORMAT_RG_8_S:
	case CMP_FORMAT_RG_8:
	case CMP_FORMAT_RG_16:
	case CMP_FORMAT_RG_16F:
	case CMP_FORMAT_RG_32F:
		return 2;

		// 1 channel
	case CMP_FORMAT_R_8_S:
	case CMP_FORMAT_R_8:
	case CMP_FORMAT_R_16:
	case CMP_FORMAT_R_16F:
	case CMP_FORMAT_R_32F:
		return 1;

	default:
		Error( "ResourceCompilerSTex: Unknown or unsupported Compressonator format 0x%X", cmpFormat );
		Assert( false );
		return 0;
	}
}

/*
==================
STex_GetNumBytesPerChannel
==================
*/
static uint32 STex_GetNumBytesPerChannel( CMP_FORMAT cmpFormat )
{
	switch ( cmpFormat )
	{
		// 8 bit
	case CMP_FORMAT_RGBA_8888_S:
	case CMP_FORMAT_ARGB_8888_S:
	case CMP_FORMAT_ARGB_8888:
	case CMP_FORMAT_ABGR_8888:
	case CMP_FORMAT_RGBA_8888:
	case CMP_FORMAT_BGRA_8888:
	case CMP_FORMAT_RGB_888:
	case CMP_FORMAT_RGB_888_S:
	case CMP_FORMAT_BGR_888:
	case CMP_FORMAT_RG_8_S:
	case CMP_FORMAT_RG_8:
	case CMP_FORMAT_R_8_S:
	case CMP_FORMAT_R_8:
		return 8;

		// 16 bit
	case CMP_FORMAT_ARGB_16:
	case CMP_FORMAT_ABGR_16:
	case CMP_FORMAT_RGBA_16:
	case CMP_FORMAT_BGRA_16:
	case CMP_FORMAT_ARGB_16F:
	case CMP_FORMAT_ABGR_16F:
	case CMP_FORMAT_RGBA_16F:
	case CMP_FORMAT_BGRA_16F:
	case CMP_FORMAT_RG_16:
	case CMP_FORMAT_RG_16F:
	case CMP_FORMAT_R_16:
	case CMP_FORMAT_R_16F:
		return 16;

		// 32 bit
	case CMP_FORMAT_RGBE_32F:
	case CMP_FORMAT_ARGB_32F:
	case CMP_FORMAT_ABGR_32F:
	case CMP_FORMAT_RGBA_32F:
	case CMP_FORMAT_BGRA_32F:
	case CMP_FORMAT_RGB_32F:
	case CMP_FORMAT_BGR_32F:
	case CMP_FORMAT_RG_32F:
	case CMP_FORMAT_R_32F:
		return 32;

	default:
		Error( "ResourceCompilerSTex: Unknown or unsupported Compressonator format 0x%X", cmpFormat );
		Assert( false );
		return 0;
	}
}

//-----------------------------------------------------------------------------
// Resource compiler for 'stex'
//-----------------------------------------------------------------------------
class CResourceCompilerSTex : public IResourceCompiler
{
public:
	// IResourceCompiler interface
	// Initialize and shutdown
	virtual bool Init( createInterfaceFn_t pFactory ) override;
	virtual void Shutdown() override;

	// Compile a resource
	// pSrcPath		Path to a source file (with file extension)
	// pDestPath	Path to a destination file (without file extension)
	virtual bool Compile( const char* pSrcPath, const char* pDestPath ) const override;

	// Get source formats and resource type name
	virtual uint32		GetNumSrcFormats() const override;
	virtual const char* GetSrcFormat( uint32 index ) const override;
	virtual const char* GetName() const override;

private:
	bool	  IsSupportPixelFormat( studioAPIPixelFormat_t pixelFormat ) const;
	CMP_ERROR LoadImage( const char* pPath, CMP_MipSet& cmpMipSet ) const;
	void	  GenerateMipmaps( CMP_MipSet& cmpMipSet, stexTextureMipMaps_t& mipmaps ) const;

	// The function convert the texture data into a new pixel format and copy it into output array
	CMP_ERROR ConvertMipsData( CMP_MipSet& cmpMipSet, CMP_FORMAT cmpNewFormat, const stexTextureMipMaps_t& mipmaps, eastl::vector<byte>& data, uint32 dataOffset = 0 ) const;

	// Copy a texture data as is
	void CopyMipsData( CMP_MipSet& cmpMipSet, eastl::vector<byte>& data, uint32 dataOffset = 0 ) const;
};

EXPOSE_SINGLE_INTERFACE( CResourceCompilerSTex, IResourceCompiler, RESOURCECOMPILER_INTERFACE_VERSION );

/*
==================
CResourceCompilerSTex::Init
==================
*/
bool CResourceCompilerSTex::Init( createInterfaceFn_t pFactory )
{
	// Connect the tier1
	if ( !ConnectTier1( pFactory ) )
	{
		return false;
	}

	// Initialize frameworks plugin and IO interfaces for Compressonator
	CMP_InitFramework();
	BC_ERROR bcError = CMP_InitializeBCLibrary();
	if ( bcError != BC_ERROR_NONE && bcError != BC_ERROR_LIBRARY_ALREADY_INITIALIZED )
	{
		Assert( bcError );
		return false;
	}

	// Set a default pixel format
	CSTEXSourceTextureDoc::SetDefaultPixelFormat( STUDIOAPI_PIXEL_FORMAT_RGBA8 );
	return true;
}

/*
==================
CResourceCompilerSTex::Shutdown
==================
*/
void CResourceCompilerSTex::Shutdown()
{
	CMP_ShutdownBCLibrary();
	DisconnectTier1();
}

/*
==================
CResourceCompilerSTex::Compile
==================
*/
bool CResourceCompilerSTex::Compile( const char* pSrcPath, const char* pDestPath ) const
{
	// Load a source file
	CSTEXSourceTextureDoc stexSourceFile;
	if ( !stexSourceFile.LoadFromFile( pSrcPath ) )
	{
		Error( "ResourceCompilerSTex: Failed to load STEX file '%s'", pSrcPath );
		return false;
	}

	// Get path to directory with the source file
	eastl::string sourceFileDir;
	{
		eastl::string tmpBuffer;
		S_GetFilePath( pSrcPath, tmpBuffer, false );
		S_MakeAbsolutePath( tmpBuffer, sourceFileDir, "", false );
	}

	// Get an absolute source texture paths
	eastl::vector<eastl::string> srcPaths;
	{
		const eastl::vector<eastl::string>& originalSrcPaths = stexSourceFile.GetSourcePaths();
		srcPaths.resize( originalSrcPaths.size() );
		for ( uint32 sourcePathIdx = 0, numSourcePaths = (uint32)originalSrcPaths.size(); sourcePathIdx < numSourcePaths; ++sourcePathIdx )
		{
			eastl::string& srcPath = srcPaths[sourcePathIdx];
			S_MakeAbsolutePath( originalSrcPaths[sourcePathIdx], srcPath, sourceFileDir );
		}
	}

	// Make sure that we does not compile a 3D texture, because we don't support compilation it yet
	if ( stexSourceFile.GetType() == STUDIOAPI_TEXTURE_TYPE_3D )
	{
		Error( "ResourceCompilerSTex: The compiler does not support 3D texture compilation" );
		return false;
	}

	// Make sure that we support a pixel format
	if ( !IsSupportPixelFormat( stexSourceFile.GetPixelFormat() ) )
	{
		Error( "ResourceCompilerSTex: The compiler does not support pixel format 0x%X", stexSourceFile.GetPixelFormat() );
		return false;
	}

	// Compile each texture layer
	stexTextureMipMaps_t mipmaps;
	eastl::vector<byte>	 data;
	for ( uint32 sourcePathIdx = 0, numSrcPaths = (uint32)srcPaths.size(); sourcePathIdx < numSrcPaths; ++sourcePathIdx )
	{
		Msg( "ResourceCompilerSTex: Compiling texture layer %i...", sourcePathIdx );

		// Load an source image
		CMP_MipSet cmpMipSet;
		Mem_Memzero( &cmpMipSet, sizeof( CMP_MipSet ) );
		if ( LoadImage( srcPaths[sourcePathIdx].c_str(), cmpMipSet ) != CMP_OK )
		{
			return false;
		}

		// Make sure that all textures in the array have same size
		if ( !mipmaps.empty() )
		{
			const stexTextureMipMap_t& mipmap0 = mipmaps[0];
			if ( mipmap0.sizeX != cmpMipSet.m_nWidth || mipmap0.sizeY != cmpMipSet.m_nHeight || mipmap0.sizeZ != cmpMipSet.m_nDepth )
			{
				Msg( "ResourceCompilerSTex: For array and cube textures all layers must have same size. Layer 0: %ix%ix%i, Layer %i: %ix%ix%i",
					 mipmap0.sizeX, mipmap0.sizeY, mipmap0.sizeZ,
					 sourcePathIdx, cmpMipSet.m_nWidth, cmpMipSet.m_nHeight, cmpMipSet.m_nDepth );

				// Free Compressonator data
				CMP_FreeMipSet( &cmpMipSet );
				return false;
			}
		}

		// Generate mipmap levels for the source image, if it need
		if ( stexSourceFile.IsNeedGenerateMipMaps() )
		{
			GenerateMipmaps( cmpMipSet, mipmaps );
		}
		else if ( mipmaps.empty() )
		{
			stexTextureMipMap_t& mipmap0 = mipmaps.emplace_back();
			mipmap0.sizeX				 = cmpMipSet.m_nWidth;
			mipmap0.sizeY				 = cmpMipSet.m_nHeight;
			mipmap0.sizeZ				 = cmpMipSet.m_nDepth;
		}

		// Convert the texture to a new format
		CMP_FORMAT cmpNewFormat = STex_ConvPixelFormatToCmpFormat( stexSourceFile.GetPixelFormat() );
		if ( cmpMipSet.m_format != cmpNewFormat )
		{
			CMP_ERROR cmpResult = ConvertMipsData( cmpMipSet, cmpNewFormat, mipmaps, data, (uint32)data.size() );
			if ( cmpResult != CMP_OK )
			{
				Error( "ResourceCompilerSTex: Failed to convert pixel format" );

				// Free Compressonator data
				CMP_FreeMipSet( &cmpMipSet );
				return false;
			}
		}
		// Otherwise if we don't need convert then simply copy a data from CMP_MipSet to own data
		else
		{
			CopyMipsData( cmpMipSet, data, (uint32)data.size() );
		}

		// Free Compressonator data
		CMP_FreeMipSet( &cmpMipSet );
		Msg( "ResourceCompilerSTex: ..Texture layer %i compiled", sourcePathIdx );
	}

	// Save texture to compiled STEX format
	eastl::string destPath = S_Sprintf( "%s.stex_c", pDestPath );
	Msg( "ResourceCompilerSTex: Saving the texture to '%s'...", destPath.c_str() );
	CSTEXCompiledTextureDoc stexCompiledFile;
	stexCompiledFile.SetData( stexSourceFile.GetType(), (uint32)srcPaths.size(), mipmaps, data, stexSourceFile.GetPixelFormat() );
	stexCompiledFile.SetAddressModeU( stexSourceFile.GetAddressModeU() );
	stexCompiledFile.SetAddressModeV( stexSourceFile.GetAddressModeV() );
	stexCompiledFile.SetAddressModeW( stexSourceFile.GetAddressModeW() );
	stexCompiledFile.SetFilter( stexSourceFile.GetFilter() );
	stexCompiledFile.SetMaxAnisotropy( stexSourceFile.GetMaxAnisotropy() );
	if ( !stexCompiledFile.SaveFile( destPath.c_str() ) )
	{
		Error( "ResourceCompilerSTex: Failed to save the texture to '%s'", destPath.c_str() );
		return false;
	}
	Msg( "ResourceCompilerSTex: ..The texture saved" );

	// We are done
	return true;
}

/*
==================
CResourceCompilerSTex::IsSupportPixelFormat
==================
*/
FORCEINLINE bool CResourceCompilerSTex::IsSupportPixelFormat( studioAPIPixelFormat_t pixelFormat ) const
{
	return pixelFormat != STUDIOAPI_PIXEL_FORMAT_UNKNOWN && pixelFormat != STUDIOAPI_PIXEL_FORMAT_DEPTH16 && pixelFormat != STUDIOAPI_PIXEL_FORMAT_DEPTH32F && pixelFormat != STUDIOAPI_PIXEL_FORMAT_DEPTH16_STENCIL8 && pixelFormat != STUDIOAPI_PIXEL_FORMAT_DEPTH24_STENCIL8 && pixelFormat != STUDIOAPI_PIXEL_FORMAT_DEPTH32F_STENCIL8 && pixelFormat != STUDIOAPI_PIXEL_NUM_FORMATS;
}

/*
==================
CResourceCompilerSTex::GetNumSrcFormats
==================
*/
uint32 CResourceCompilerSTex::GetNumSrcFormats() const
{
	return 1;
}

/*
==================
CResourceCompilerSTex::GetNumSrcFormats
==================
*/
const char* CResourceCompilerSTex::GetSrcFormat( uint32 index ) const
{
	Assert( index < GetNumSrcFormats() );
	return "stex";
}

/*
==================
CResourceCompilerSTex::GetName
==================
*/
const char* CResourceCompilerSTex::GetName() const
{
	return "Singularity Texture";
}

/*
==================
CResourceCompilerSTex::LoadImage
==================
*/
CMP_ERROR CResourceCompilerSTex::LoadImage( const char* pPath, CMP_MipSet& cmpMipSet ) const
{
	CMP_ERROR cmpStatus = CMP_LoadTexture( pPath, &cmpMipSet );
	if ( cmpStatus != CMP_OK )
	{
		Error( "ResourceCompilerSTex: Failed to load file '%s'. Compressonator error: 0x%X", pPath, cmpStatus );
		return cmpStatus;
	}

	Msg( "ResourceCompilerSTex: Loaded image '%s'", pPath );
	return CMP_OK;
}

/*
==================
CResourceCompilerSTex::GenerateMipmaps
==================
*/
void CResourceCompilerSTex::GenerateMipmaps( CMP_MipSet& cmpMipSet, stexTextureMipMaps_t& mipmaps ) const
{
	// If the mipmaps is empty calculate they
	if ( mipmaps.empty() )
	{
		// Calculate mip count to request
		uint32 maxSize		   = S_Max<uint32>( cmpMipSet.m_nDepth, S_Max<uint32>( cmpMipSet.m_nWidth, cmpMipSet.m_nHeight ) );
		uint32 requestMipCount = (uint32)S_Floor( S_Log2( (float)maxSize ) ) + 1;

		// Calculate texture size for each mip level
		mipmaps.resize( requestMipCount );
		for ( uint32 mipIdx = 0; mipIdx < requestMipCount; ++mipIdx )
		{
			stexTextureMipMap_t& mipmap = mipmaps[mipIdx];
			mipmap.sizeX				= S_Max( cmpMipSet.m_nWidth >> mipIdx, 1 );
			mipmap.sizeY				= S_Max( cmpMipSet.m_nHeight >> mipIdx, 1 );
			mipmap.sizeZ				= S_Max( cmpMipSet.m_nDepth >> mipIdx, 1 );
		}
	}

	// Checks what the minimum image size will be for the requested mip levels
	// if the request is too large, a adjusted minimum size will be returned
	const stexTextureMipMap_t& lastMipmap = mipmaps[(uint32)mipmaps.size() - 1];
	uint32					   minSize	  = S_Max( lastMipmap.sizeZ, S_Max( lastMipmap.sizeX, lastMipmap.sizeY ) );

	// Now that the minimum size is known, generate the miplevels
	Msg( "ResourceCompilerSTex: Generating mipmaps.." );
	CMP_GenerateMIPLevels( &cmpMipSet, minSize );
	Msg( "ResourceCompilerSTex: ..Generated %i mipmaps", (uint32)mipmaps.size() );
}

/*
==================
CResourceCompilerSTex::ConvertMipsData
==================
*/
CMP_ERROR CResourceCompilerSTex::ConvertMipsData( CMP_MipSet& cmpMipSet, CMP_FORMAT cmpNewFormat, const stexTextureMipMaps_t& mipmaps, eastl::vector<byte>& data, uint32 dataOffset /* = 0 */ ) const
{
	// Set compression options
	CMP_CompressOptions cmpCompressOptions = {};
	cmpCompressOptions.dwSize			   = sizeof( CMP_CompressOptions );
	cmpCompressOptions.fquality			   = 1.f;

	// Convert each mip level and save into our data
	Msg( "ResourceCompilerSTex: Texture format conversion started.." );
	for ( uint32 mipIdx = 0; mipIdx < (uint32)cmpMipSet.m_nMipLevels; ++mipIdx )
	{
		const stexTextureMipMap_t& mipmap		= mipmaps[mipIdx];
		CMP_MipLevel*			   pCmpMipLevel = NULL;
		CMP_GetMipLevel( &pCmpMipLevel, &cmpMipSet, mipIdx, 0 );

		// If cmpNewFormat isn't float format or source format is compressed then we use CMP_ConvertTexture
		if ( CMP_IsCompressedFormat( cmpMipSet.m_format ) || !CMP_IsFloatFormat( cmpNewFormat ) )
		{
			// Fill a description about a source texture
			CMP_Texture cmpSrcTexture = {};
			cmpSrcTexture.dwSize	  = sizeof( CMP_Texture );
			cmpSrcTexture.format	  = cmpMipSet.m_format;
			cmpSrcTexture.dwWidth	  = mipmap.sizeX;
			cmpSrcTexture.dwHeight	  = mipmap.sizeY;
			cmpSrcTexture.dwDataSize  = pCmpMipLevel->m_dwLinearSize;
			cmpSrcTexture.pData		  = pCmpMipLevel->m_pbData;

			// Fill a description about a destination texture
			CMP_Texture cmpDstTexture = {};
			cmpDstTexture.dwSize	  = sizeof( CMP_Texture );
			cmpDstTexture.dwWidth	  = mipmap.sizeX;
			cmpDstTexture.dwHeight	  = mipmap.sizeY;
			cmpDstTexture.format	  = cmpNewFormat;
			cmpDstTexture.dwDataSize  = CMP_CalculateBufferSize( &cmpDstTexture );
			data.resize( cmpDstTexture.dwDataSize + dataOffset );
			cmpDstTexture.pData = (CMP_BYTE*)data.data() + dataOffset;
			dataOffset += cmpDstTexture.dwDataSize;

			// Convert the texture using Compressonator
			CMP_ERROR cmpResult = CMP_ConvertTexture( &cmpSrcTexture, &cmpDstTexture, &cmpCompressOptions, NULL );
			if ( cmpResult != CMP_OK )
			{
				Error( "ResourceCompilerSTex: Failed to convert into texture format. Compressonator error: 0x%X", cmpResult );
				return cmpResult;
			}
		}
		// Otherwise we will have to convert manually to float format, because Compressonator can't do it and we have a crash (it call abort(), I don't understand why)
		else
		{
			// Initialize a destination array
			Assert( STex_GetNumBytesPerChannel( cmpMipSet.m_format ) == 8 );  // We support only 8 bit per channel in the source texture
			uint32 numSrcChannels	  = STex_GetNumChannels( cmpMipSet.m_format );
			uint32 numDstChannels	  = STex_GetNumChannels( cmpNewFormat );
			uint32 numBytesPerChannel = STex_GetNumBytesPerChannel( cmpNewFormat );
			uint32 step				  = S_Max<uint32>( (uint32)S_Abs( (float)numSrcChannels - (float)numDstChannels ), 1 );
			uint32 srcMipDataSize	  = pCmpMipLevel->m_nWidth * pCmpMipLevel->m_nHeight * numSrcChannels;
			uint32 dstMipDataSize	  = pCmpMipLevel->m_nWidth * pCmpMipLevel->m_nHeight * numDstChannels * numBytesPerChannel;
			data.resize( dstMipDataSize + dataOffset );

			// Convert from 8 bit to 16 bit
			if ( numBytesPerChannel == 16 )
			{
				uint16* pData = (uint16*)( data.data() + dataOffset );
				for ( uint32 idx = 0; idx < srcMipDataSize; idx += step )
				{
					*pData = (uint16)pCmpMipLevel->m_pbData[idx] * 65535 / 255;	 // Convert to 16 bit (0-65535)
					++pData;
				}
			}
			// Convert from 8 bit to 32 bit
			else if ( numBytesPerChannel == 32 )
			{
				float* pData = (float*)( data.data() + dataOffset );
				for ( uint32 idx = 0; idx < srcMipDataSize; idx += step )
				{
					*pData = (float)pCmpMipLevel->m_pbData[idx] / 255.f;  // Convert to float (0.0-1.0)
					++pData;
				}
			}
			else
			{
				Assert( false );
				return CMP_ERR_UNKNOWN_DESTINATION_FORMAT;
			}

			// Update the data offset
			dataOffset += dstMipDataSize;
		}

		Msg( "ResourceCompilerSTex: Mip %i: Done", mipIdx );
	}

	Msg( "ResourceCompilerSTex: ..Conversion completed" );
	return CMP_OK;
}

/*
==================
CResourceCompilerSTex::CopyMipsData
==================
*/
void CResourceCompilerSTex::CopyMipsData( CMP_MipSet& cmpMipSet, eastl::vector<byte>& data, uint32 dataOffset /* = 0 */ ) const
{
	// Copy a data from CMP_MipLevel to own data
	Assert( data.empty() );
	for ( uint32 mipIdx = 0; mipIdx < (uint32)cmpMipSet.m_nMipLevels; ++mipIdx )
	{
		CMP_MipLevel* pCmpMipLevel = NULL;
		CMP_GetMipLevel( &pCmpMipLevel, &cmpMipSet, mipIdx, 0 );

		data.resize( pCmpMipLevel->m_dwLinearSize + dataOffset );
		Mem_Memcpy( data.data() + dataOffset, pCmpMipLevel->m_pbData, pCmpMipLevel->m_dwLinearSize );
		dataOffset += pCmpMipLevel->m_dwLinearSize;
	}
}
