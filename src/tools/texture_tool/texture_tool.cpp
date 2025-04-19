#include "pch_texture_tool.h"
#include "stdlib/math/math.h"
#include "studiorender/studioapi/istudioapi_texture.h"
#include "stexdoc/stex_compiled_doc.h"
#include "tools/resource_tools/itexture_tool.h"

#include <compressonator.h>

/*
==================
TextureTool_ConvPixelFormatToCmpFormat
==================
*/
static CMP_FORMAT TextureTool_ConvPixelFormatToCmpFormat( studioAPIPixelFormat_t pixelFormat )
{
	switch ( pixelFormat )
	{
		// Channel component formats
	case STUDIOAPI_PIXEL_FORMAT_RGBA8:				return CMP_FORMAT_RGBA_8888;
	case STUDIOAPI_PIXEL_FORMAT_RG8:				return CMP_FORMAT_RG_8;
	case STUDIOAPI_PIXEL_FORMAT_R8:					return CMP_FORMAT_R_8;
	case STUDIOAPI_PIXEL_FORMAT_RGBA16:				return CMP_FORMAT_RGBA_16;
	case STUDIOAPI_PIXEL_FORMAT_RG16:				return CMP_FORMAT_RG_16;
	case STUDIOAPI_PIXEL_FORMAT_R16:				return CMP_FORMAT_R_16;
	case STUDIOAPI_PIXEL_FORMAT_RGBA32F:			return CMP_FORMAT_RGBA_32F;
	case STUDIOAPI_PIXEL_FORMAT_RG32F:				return CMP_FORMAT_RG_32F;
	case STUDIOAPI_PIXEL_FORMAT_R32F:				return CMP_FORMAT_R_32F;
	case STUDIOAPI_PIXEL_FORMAT_RGBA16F:			return CMP_FORMAT_RGBA_16F;
	case STUDIOAPI_PIXEL_FORMAT_RG16F:				return CMP_FORMAT_RG_16F;
	case STUDIOAPI_PIXEL_FORMAT_R16F:				return CMP_FORMAT_R_16F;

		// Compression formats
	case STUDIOAPI_PIXEL_FORMAT_BC1:				return CMP_FORMAT_BC1;
	case STUDIOAPI_PIXEL_FORMAT_BC2:				return CMP_FORMAT_BC2;
	case STUDIOAPI_PIXEL_FORMAT_BC3:				return CMP_FORMAT_BC3;
	case STUDIOAPI_PIXEL_FORMAT_BC4:				return CMP_FORMAT_BC4;
	case STUDIOAPI_PIXEL_FORMAT_BC5:				return CMP_FORMAT_BC5;
	case STUDIOAPI_PIXEL_FORMAT_BC6H:				return CMP_FORMAT_BC6H;
	case STUDIOAPI_PIXEL_FORMAT_BC7:				return CMP_FORMAT_BC7;

		// Other formats
	case STUDIOAPI_PIXEL_FORMAT_DEPTH16:
	case STUDIOAPI_PIXEL_FORMAT_DEPTH32F:
	case STUDIOAPI_PIXEL_FORMAT_DEPTH16_STENCIL8:
	case STUDIOAPI_PIXEL_FORMAT_DEPTH24_STENCIL8:
	case STUDIOAPI_PIXEL_FORMAT_DEPTH32F_STENCIL8:
	default:
		Error( "TextureTool: Unsupported pixel format 0x%X", pixelFormat );
		Assert( false );
		return CMP_FORMAT_Unknown;
	}
}


/*
==================
TextureTool_GetNumChannels
==================
*/
static uint32 TextureTool_GetNumChannels( CMP_FORMAT cmpFormat )
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
		Error( "TextureTool: Unknown or unsupported Compressonator format 0x%X", cmpFormat );
		Assert( false );
		return 0;
	}
}


/*
==================
TextureTool_GetNumBytesPerChannel
==================
*/
static uint32 TextureTool_GetNumBytesPerChannel( CMP_FORMAT cmpFormat )
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
		Error( "TextureTool: Unknown or unsupported Compressonator format 0x%X", cmpFormat );
		Assert( false );
		return 0;
	}
}


//-----------------------------------------------------------------------------
// Texture tool
//-----------------------------------------------------------------------------
class CTextureTool : public CBaseAppSystem<ITextureTool>
{
public:
	// IAppSystem interface
	// Here's where the app systems get to learn about each other
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// Initialize and shutdown
	virtual bool Init() override;
	virtual void Shutdown() override;

	// ITextureTool interface
	virtual bool CompileTexture( const resourceToolCompileTextureParams_t& compileParams ) const override;
	virtual bool IsSupportPixelFormat( studioAPIPixelFormat_t pixelFormat ) const override;

private:
	CMP_ERROR LoadImage( const achar* pPath, CMP_MipSet& cmpMipSet ) const;
	void GenerateMipmaps( CMP_MipSet& cmpMipSet, stexTextureMipMaps_t& mipmaps ) const;

	// The function convert the texture data into a new pixel format and copy it into output array
	CMP_ERROR ConvertMipsData( CMP_MipSet& cmpMipSet, CMP_FORMAT cmpNewFormat, const stexTextureMipMaps_t& mipmaps, std::vector<byte>& data, uint32 dataOffset = 0 ) const;

	// Copy a texture data as is
	void CopyMipsData( CMP_MipSet& cmpMipSet, std::vector<byte>& data, uint32 dataOffset = 0 ) const;
};

EXPOSE_SINGLE_INTERFACE( CTextureTool, ITextureTool, TEXTURE_TOOL_INTERFACE_VERSION );


/*
==================
CTextureTool::Connect
==================
*/
bool CTextureTool::Connect( createInterfaceFn_t pFactory )
{
	return ConnectStdLib( pFactory );
}

/*
==================
CTextureTool::Init
==================
*/
bool CTextureTool::Init()
{
	// Initialize frameworks plugin and IO interfaces for Compressonator
	CMP_InitFramework();
	BC_ERROR		bcError = CMP_InitializeBCLibrary();
	if ( bcError != BC_ERROR_NONE && bcError != BC_ERROR_LIBRARY_ALREADY_INITIALIZED )
	{
		Assert( bcError );
		return false;
	}

	return true;
}

/*
==================
CTextureTool::Shutdown
==================
*/
void CTextureTool::Shutdown()
{
	CMP_ShutdownBCLibrary();
}

/*
==================
CTextureTool::Disconnect
==================
*/
void CTextureTool::Disconnect()
{
	DisconnectStdLib();
}

/*
==================
CTextureTool::CompileTexture
==================
*/
bool CTextureTool::CompileTexture( const resourceToolCompileTextureParams_t& compileParams ) const
{
	// Make sure that we does not compile a 3D texture, because we don't support compilation it yet
	if ( compileParams.type == STUDIOAPI_TEXTURE_TYPE_3D )
	{
		Msg( "TextureTool: The tool does not support 3D texture compilation" );
		return false;
	}

	// Compile each texture layer
	stexTextureMipMaps_t	mipmaps;
	std::vector<byte>		data;
	for ( uint32 sourcePathIdx = 0; sourcePathIdx < compileParams.numSrcPaths; ++sourcePathIdx )
	{
		Msg( "TextureTool: Compiling texture layer %i...", sourcePathIdx );
		
		// Load an source image
		CMP_MipSet		cmpMipSet;
		Mem_Memzero( &cmpMipSet, sizeof( CMP_MipSet ) );
		if ( LoadImage( compileParams.ppSrcPaths[sourcePathIdx], cmpMipSet ) != CMP_OK )
		{
			return false;
		}

		// Make sure that all textures in the array have same size
		if ( !mipmaps.empty() )
		{
			const stexTextureMipMap_t&	mipmap0 = mipmaps[0];
			if ( mipmap0.sizeX != cmpMipSet.m_nWidth || mipmap0.sizeY != cmpMipSet.m_nHeight || mipmap0.sizeZ != cmpMipSet.m_nDepth )
			{
				Msg( "TextureTool: For array and cube textures all layers must have same size. Layer 0: %ix%ix%i, Layer %i: %ix%ix%i", 
					 mipmap0.sizeX, mipmap0.sizeY, mipmap0.sizeZ, 
					 sourcePathIdx, cmpMipSet.m_nWidth, cmpMipSet.m_nHeight, cmpMipSet.m_nDepth );

				// Free Compressonator data
				CMP_FreeMipSet( &cmpMipSet );
				return false;
			}
		}

		// Generate mipmap levels for the source image, if it need
		if ( compileParams.bGenerateMipMaps )
		{
			GenerateMipmaps( cmpMipSet, mipmaps );
		}
		else if ( mipmaps.empty() )
		{
			stexTextureMipMap_t&	mipmap0 = mipmaps.emplace_back();
			mipmap0.sizeX			= cmpMipSet.m_nWidth;
			mipmap0.sizeY			= cmpMipSet.m_nHeight;
			mipmap0.sizeZ			= cmpMipSet.m_nDepth;
		}

		// Convert the texture to a new format
		CMP_FORMAT	cmpNewFormat = TextureTool_ConvPixelFormatToCmpFormat( compileParams.pixelFormat );
		if ( cmpMipSet.m_format != cmpNewFormat )
		{
			CMP_ERROR		cmpResult = ConvertMipsData( cmpMipSet, cmpNewFormat, mipmaps, data, ( uint32 )data.size() );
			if ( cmpResult != CMP_OK )
			{
				Msg( "TextureTool: Failed to convert pixel format" );

				// Free Compressonator data
				CMP_FreeMipSet( &cmpMipSet );
				return false;
			}
		}
		// Otherwise if we don't need convert then simply copy a data from CMP_MipSet to own data
		else
		{
			CopyMipsData( cmpMipSet, data, ( uint32 )data.size() );
		}

		// Free Compressonator data
		CMP_FreeMipSet( &cmpMipSet );
		Msg( "TextureTool: ..Texture layer %i compiled", sourcePathIdx );
	}

	// Save texture to compiled STEX format
	std::string			destPath = S_Sprintf( "%s.stex_c", compileParams.pDestPath );
	Msg( "TextureTool: Saving the texture to '%s'...", destPath.c_str() );
	CSTEXCompiledTextureDoc		stexCompiledFile;
	stexCompiledFile.SetData( compileParams.type, compileParams.numSrcPaths, mipmaps, data, compileParams.pixelFormat );
	stexCompiledFile.SetAddressModeU( compileParams.addressModeU );
	stexCompiledFile.SetAddressModeV( compileParams.addressModeV );
	stexCompiledFile.SetAddressModeW( compileParams.addressModeW );
	stexCompiledFile.SetFilter( compileParams.filter );
	stexCompiledFile.SetMaxAnisotropy( compileParams.maxAnisotropy );
	if ( !stexCompiledFile.SaveFile( destPath.c_str() ) )
	{
		Error( "TextureTool: Failed to save the texture to '%s'", destPath.c_str() );
		return false;
	}
	Msg( "TextureTool: ..The texture saved" );

	// We are done
	return true;
}

/*
==================
CTextureTool::IsSupportPixelFormat
==================
*/
bool CTextureTool::IsSupportPixelFormat( studioAPIPixelFormat_t pixelFormat ) const
{
	return	pixelFormat != STUDIOAPI_PIXEL_FORMAT_UNKNOWN &&
			pixelFormat != STUDIOAPI_PIXEL_FORMAT_DEPTH16 &&
			pixelFormat != STUDIOAPI_PIXEL_FORMAT_DEPTH32F &&
			pixelFormat != STUDIOAPI_PIXEL_FORMAT_DEPTH16_STENCIL8 &&
			pixelFormat != STUDIOAPI_PIXEL_FORMAT_DEPTH24_STENCIL8 &&
			pixelFormat != STUDIOAPI_PIXEL_FORMAT_DEPTH32F_STENCIL8 &&
			pixelFormat != STUDIOAPI_PIXEL_NUM_FORMATS;
}

/*
==================
CTextureTool::LoadImage
==================
*/
CMP_ERROR CTextureTool::LoadImage( const achar* pPath, CMP_MipSet& cmpMipSet ) const
{
	CMP_ERROR	cmpStatus = CMP_LoadTexture( pPath, &cmpMipSet );
	if ( cmpStatus != CMP_OK )
	{
		Error( "TextureTool: Failed to load file '%s'. Compressonator error: 0x%X", pPath, cmpStatus );
		return cmpStatus;
	}
	
	Msg( "TextureTool: Loaded image '%s'", pPath );
	return CMP_OK;
}

/*
==================
CTextureTool::GenerateMipmaps
==================
*/
void CTextureTool::GenerateMipmaps( CMP_MipSet& cmpMipSet, stexTextureMipMaps_t& mipmaps ) const
{
	// If the mipmaps is empty calculate they
	if ( mipmaps.empty() )
	{
		// Calculate mip count to request
		uint32		maxSize			= Max<uint32>( cmpMipSet.m_nDepth, Max<uint32>( cmpMipSet.m_nWidth, cmpMipSet.m_nHeight ) );
		uint32		requestMipCount	= ( uint32 )S_Floor( S_Log2( ( float )maxSize ) ) + 1;

		// Calculate texture size for each mip level
		mipmaps.resize( requestMipCount );
		for ( uint32 mipIdx = 0; mipIdx < requestMipCount; ++mipIdx )
		{
			stexTextureMipMap_t&	mipmap = mipmaps[mipIdx];
			mipmap.sizeX			= Max( cmpMipSet.m_nWidth >> mipIdx, 1 );
			mipmap.sizeY			= Max( cmpMipSet.m_nHeight >> mipIdx, 1 );
			mipmap.sizeZ			= Max( cmpMipSet.m_nDepth >> mipIdx, 1 );
		}
	}

	// Checks what the minimum image size will be for the requested mip levels
	// if the request is too large, a adjusted minimum size will be returned 
	const stexTextureMipMap_t&		lastMipmap	= mipmaps[( uint32 )mipmaps.size() - 1];
	uint32							minSize		= Max( lastMipmap.sizeZ, Max( lastMipmap.sizeX, lastMipmap.sizeY ) );

	// Now that the minimum size is known, generate the miplevels
	Msg( "TextureTool: Generating mipmaps.." );
	CMP_GenerateMIPLevels( &cmpMipSet, minSize );
	Msg( "TextureTool: ..Generated %i mipmaps", ( uint32 )mipmaps.size() );
}

/*
==================
CTextureTool::ConvertMipsData
==================
*/
CMP_ERROR CTextureTool::ConvertMipsData( CMP_MipSet& cmpMipSet, CMP_FORMAT cmpNewFormat, const stexTextureMipMaps_t& mipmaps, std::vector<byte>& data, uint32 dataOffset /* = 0 */ ) const
{
	// Set compression options
	CMP_CompressOptions			cmpCompressOptions = {};
	cmpCompressOptions.dwSize	= sizeof( CMP_CompressOptions );
	cmpCompressOptions.fquality	= 1.f;

	// Convert each mip level and save into our data
	Msg( "TextureTool: Texture format conversion started.." );
	for ( uint32 mipIdx = 0; mipIdx < ( uint32 )cmpMipSet.m_nMipLevels; ++mipIdx )
	{
		const stexTextureMipMap_t&	mipmap			= mipmaps[mipIdx];
		CMP_MipLevel*				pCmpMipLevel	= NULL;
		CMP_GetMipLevel( &pCmpMipLevel, &cmpMipSet, mipIdx, 0 );

		// If cmpNewFormat isn't float format or source format is compressed then we use CMP_ConvertTexture
		if ( CMP_IsCompressedFormat( cmpMipSet.m_format ) || !CMP_IsFloatFormat( cmpNewFormat ) )
		{
			// Fill a description about a source texture
			CMP_Texture					cmpSrcTexture = {};
			cmpSrcTexture.dwSize		= sizeof( CMP_Texture );
			cmpSrcTexture.format		= cmpMipSet.m_format;
			cmpSrcTexture.dwWidth		= mipmap.sizeX;
			cmpSrcTexture.dwHeight		= mipmap.sizeY;
			cmpSrcTexture.dwDataSize	= pCmpMipLevel->m_dwLinearSize;
			cmpSrcTexture.pData			= pCmpMipLevel->m_pbData;

			// Fill a description about a destination texture
			CMP_Texture					cmpDstTexture = {};
			cmpDstTexture.dwSize		= sizeof( CMP_Texture );
			cmpDstTexture.dwWidth		= mipmap.sizeX;
			cmpDstTexture.dwHeight		= mipmap.sizeY;
			cmpDstTexture.format		= cmpNewFormat;
			cmpDstTexture.dwDataSize	= CMP_CalculateBufferSize( &cmpDstTexture );
			data.resize( cmpDstTexture.dwDataSize + dataOffset );
			cmpDstTexture.pData			= ( CMP_BYTE* )data.data() + dataOffset;
			dataOffset					+= cmpDstTexture.dwDataSize;

			// Convert the texture using Compressonator
			CMP_ERROR	cmpResult = CMP_ConvertTexture( &cmpSrcTexture, &cmpDstTexture, &cmpCompressOptions, NULL );
			if ( cmpResult != CMP_OK )
			{
				Error( "TextureTool: Failed to convert into texture format. Compressonator error: 0x%X", cmpResult );
				return cmpResult;
			}
		}
		// Otherwise we will have to convert manually to float format, because Compressonator can't do it and we have a crash (it call abort(), I don't understand why)
		else
		{
			// Initialize a destination array
			Assert( TextureTool_GetNumBytesPerChannel( cmpMipSet.m_format ) == 8 );		// We support only 8 bit per channel in the source texture
			uint32				numSrcChannels		= TextureTool_GetNumChannels( cmpMipSet.m_format );
			uint32				numDstChannels		= TextureTool_GetNumChannels( cmpNewFormat );
			uint32				numBytesPerChannel	= TextureTool_GetNumBytesPerChannel( cmpNewFormat );
			uint32				step				= Max<uint32>( ( uint32 )S_Abs( ( float )numSrcChannels - ( float )numDstChannels ), 1 );
			uint32				srcMipDataSize		= pCmpMipLevel->m_nWidth * pCmpMipLevel->m_nHeight * numSrcChannels;
			uint32				dstMipDataSize		= pCmpMipLevel->m_nWidth * pCmpMipLevel->m_nHeight * numDstChannels * numBytesPerChannel;
			data.resize( dstMipDataSize + dataOffset );

			// Convert from 8 bit to 16 bit
			if ( numBytesPerChannel == 16 )
			{
				uint16*		pData = ( uint16* )( data.data() + dataOffset );
				for ( uint32 idx = 0; idx < srcMipDataSize; idx += step )
				{
					*pData = ( uint16 )pCmpMipLevel->m_pbData[idx] * 65535 / 255;		// Convert to 16 bit (0-65535)
					++pData;
				}
			}
			// Convert from 8 bit to 32 bit
			else if ( numBytesPerChannel == 32 )
			{
				float*		pData = ( float* )( data.data() + dataOffset );
				for ( uint32 idx = 0; idx < srcMipDataSize; idx += step )
				{
					*pData = ( float )pCmpMipLevel->m_pbData[idx] / 255.f;				// Convert to float (0.0-1.0)
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

		Msg( "TextureTool: Mip %i: Done", mipIdx );
	}

	Msg( "TextureTool: ..Conversion completed" );
	return CMP_OK;
}

/*
==================
CTextureTool::CopyMipsData
==================
*/
void CTextureTool::CopyMipsData( CMP_MipSet& cmpMipSet, std::vector<byte>& data, uint32 dataOffset /* = 0 */ ) const
{
	// Copy a data from CMP_MipLevel to own data
	Assert( data.empty() );
	for ( uint32 mipIdx = 0; mipIdx < ( uint32 )cmpMipSet.m_nMipLevels; ++mipIdx )
	{
		CMP_MipLevel*		pCmpMipLevel = NULL;
		CMP_GetMipLevel( &pCmpMipLevel, &cmpMipSet, mipIdx, 0 );

		data.resize( pCmpMipLevel->m_dwLinearSize + dataOffset );
		Mem_Memcpy( data.data() + dataOffset, pCmpMipLevel->m_pbData, pCmpMipLevel->m_dwLinearSize );
		dataOffset += pCmpMipLevel->m_dwLinearSize;
	}
}