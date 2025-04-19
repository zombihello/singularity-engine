#include "interfaces/interfaces.h"
#include "core/profile.h"
#include "filesystem/ifilesystem.h"
#include "stdlib/compression/zlib.h"
#include "stexdoc/stex_types.h"
#include "stexdoc/stex_compiled_doc.h"

// Singularity texture magic
static const uint32		s_STEXMagicSize					= 5;
static const achar		s_STEXMagic[s_STEXMagicSize]	= { 'S', 'T', 'E', 'X', 'C' };		// Singularity Texture Compiled
static_assert( sizeof( s_STEXMagic ) == s_STEXMagicSize, "Size of s_STEXMagic must be equal to s_STEXMagicSize" );

// Singularity texture version
static const uint32		s_STEXVersion					= 1;


/*
==================
CSTEXCompiledTextureDoc::CSTEXCompiledTextureDoc
==================
*/
CSTEXCompiledTextureDoc::CSTEXCompiledTextureDoc()
	: type( STUDIOAPI_TEXTURE_TYPE_1D )
	, pixelFormat( STUDIOAPI_PIXEL_FORMAT_UNKNOWN )
	, numLayers( 0 )
	, maxAnisotropy( 0 )
	, addressModeU( STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP )
	, addressModeV( STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP )
	, addressModeW( STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP )
	, filter( STUDIOAPI_SAMPLER_FILTER_POINT )
{}

/*
==================
CSTEXCompiledTextureDoc::SaveFile
==================
*/
bool CSTEXCompiledTextureDoc::SaveFile( const achar* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );
	
	// Try to open a file
	TRefPtr<IStreamDataWriter>	pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Error( "STEXDoc: Failed to open file '%s'", pPath );
		return false;
	}

	// Write format magic and version
	pFile->Write( ( void* )s_STEXMagic, s_STEXMagicSize );
	pFile->Write( ( void* )&s_STEXVersion, sizeof( uint32 ) );

	// Write texture header
	pFile->Write( &type,			sizeof( studioAPITextureType_t ) );
	pFile->Write( &pixelFormat,		sizeof( studioAPIPixelFormat_t ) );
	pFile->Write( &numLayers,		sizeof( uint32 ) );
	pFile->Write( &addressModeU,	sizeof( studioAPISamplerAddressMode_t ) );
	pFile->Write( &addressModeV,	sizeof( studioAPISamplerAddressMode_t ) );
	pFile->Write( &addressModeW,	sizeof( studioAPISamplerAddressMode_t ) );
	pFile->Write( &filter,			sizeof( studioAPISamplerFilter_t ) );
	pFile->Write( &maxAnisotropy,	sizeof( uint32 ) );

	// Write texture mipmap information
	uint32		numMipmaps = ( uint32 )mipmaps.size();
	pFile->Write( &numMipmaps, sizeof( uint32 ) );
	pFile->Write( mipmaps.data(), numMipmaps * sizeof( stexTextureMipMap_t ) );

	// Write texture data
	uint32		dataSize = ( uint32 )data.size();
	pFile->Write( &dataSize, sizeof( uint32 ) );
	CZLib::Compress( pFile, data.data(), dataSize );

	// We are done
	return true;
}

/*
==================
CSTEXCompiledTextureDoc::LoadFromFile
==================
*/
bool CSTEXCompiledTextureDoc::LoadFromFile( const achar* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );
	
	// Try to open a file
	TRefPtr<IStreamDataReader>	pFile = g_pFileSystem->CreateFileReader( pPath );
	if ( !pFile )
	{
		Error( "STEXDoc: Failed to open file '%s'", pPath );
		return false;
	}

	// Read format magic and version
	achar	magic[s_STEXMagicSize];
	uint32	version = 0;
	pFile->Read( magic, s_STEXMagicSize );
	pFile->Read( &version, sizeof( uint32 ) );
	if ( S_Strncmp( magic, s_STEXMagic, s_STEXMagicSize ) )
	{
		Error( "STEXDoc: Invalid magic value in '%s'. Expected '%.*s', received '%.*s'", pPath, s_STEXMagicSize, s_STEXMagic, s_STEXMagicSize, magic );
		return false;
	}

	if ( version != s_STEXVersion )
	{
		Error( "STEXDoc: Invalid version format in '%s'. Expected %i, received %i", pPath, s_STEXVersion, version );
		return false;
	}
	Clear();

	// Read texture header
	pFile->Read( &type,				sizeof( studioAPITextureType_t ) );
	pFile->Read( &pixelFormat,		sizeof( studioAPIPixelFormat_t ) );
	pFile->Read( &numLayers,		sizeof( uint32 ) );
	pFile->Read( &addressModeU,		sizeof( studioAPISamplerAddressMode_t ) );
	pFile->Read( &addressModeV,		sizeof( studioAPISamplerAddressMode_t ) );
	pFile->Read( &addressModeW,		sizeof( studioAPISamplerAddressMode_t ) );
	pFile->Read( &filter,			sizeof( studioAPISamplerFilter_t ) );
	pFile->Read( &maxAnisotropy,	sizeof( uint32 ) );

	// Read texture mipmap information
	uint32		numMipmaps = 0;
	pFile->Read( &numMipmaps, sizeof( uint32 ) );
	if ( numMipmaps > 0 )
	{
		mipmaps.resize( numMipmaps );
		pFile->Read( mipmaps.data(), numMipmaps * sizeof( stexTextureMipMap_t ) );
	}

	// Read texture data
	uint32		dataSize = 0;
	pFile->Read( &dataSize, sizeof( uint32 ) );
	if ( dataSize > 0 )
	{
		data.resize( dataSize );
		CZLib::Uncompress( pFile, data.data(), dataSize );
	}

	// We are done
	return true;
}

/*
==================
CSTEXCompiledTextureDoc::Validate
==================
*/
void CSTEXCompiledTextureDoc::Validate( studioAPITextureType_t type, uint32 numLayers, const stexTextureMipMaps_t& mipmaps ) const
{
	AssertMsg( !mipmaps.empty(), "A texture must have at least one mip level" );
	const stexTextureMipMap_t&		mipmap0 = mipmaps[0];

	switch ( type )
	{
	case STUDIOAPI_TEXTURE_TYPE_1D:
	case STUDIOAPI_TEXTURE_TYPE_1D_ARRAY:
		AssertMsg( mipmap0.sizeY == 1 && mipmap0.sizeZ == 1, "SizeY and SizeZ must be 1 for 1D textures" );
		AssertMsg( numLayers == 1 || type == STUDIOAPI_TEXTURE_TYPE_1D_ARRAY, "NumLayers must be 1 for 1D textures" );
		break;

	case STUDIOAPI_TEXTURE_TYPE_2D:
	case STUDIOAPI_TEXTURE_TYPE_2D_ARRAY:
		AssertMsg( mipmap0.sizeZ == 1, "SizeZ must be 1 for 2D textures" );
		AssertMsg( numLayers == 1 || type == STUDIOAPI_TEXTURE_TYPE_2D_ARRAY, "NumLayers must be 1 for 2D textures" );
		break;

	case STUDIOAPI_TEXTURE_TYPE_CUBE:
	case STUDIOAPI_TEXTURE_TYPE_CUBE_ARRAY:
		AssertMsg( mipmap0.sizeX == mipmap0.sizeY, "SizeX and SizeY must be equal for cube textures" );
		AssertMsg( mipmap0.sizeZ == 1, "SizeZ must be 1 for cube textures" );
		if ( type == STUDIOAPI_TEXTURE_TYPE_CUBE )
		{
			AssertMsg( numLayers == STEX_CUBE_FACE_COUNT, "NumLayers must be STEX_CUBE_FACE_COUNT for cube textures" );
		}
		else
		{
			AssertMsg( numLayers % STEX_CUBE_FACE_COUNT, "NumLayers must be multiple of STEX_CUBE_FACE_COUNT for cube textures" );
		}
		break;

	case STUDIOAPI_TEXTURE_TYPE_3D:
		AssertMsg( numLayers == 1, "NumLayers must be 1 for 3D textures" );
		break;
	}
}