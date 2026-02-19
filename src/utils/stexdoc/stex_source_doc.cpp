#include "utils/interfaces/interfaces.h"
#include "tier0/profile.h"
#include "filesystem/ifilesystem.h"
#include "tier1/keyvalues.h"
#include "utils/stexdoc/stex_source_doc.h"

// Table for convert text to studioAPITextureType_t
static const char* s_pTextureTypeNames[] = {
	"1d",		  // STUDIOAPI_TEXTURE_TYPE_1D
	"2d",		  // STUDIOAPI_TEXTURE_TYPE_2D
	"cube",		  // STUDIOAPI_TEXTURE_TYPE_CUBE
	"3d",		  // STUDIOAPI_TEXTURE_TYPE_3D
	"1d_array",	  // STUDIOAPI_TEXTURE_TYPE_1D_ARRAY
	"2d_array",	  // STUDIOAPI_TEXTURE_TYPE_2D_ARRAY
	"cube_array"  // STUDIOAPI_TEXTURE_TYPE_CUBE_ARRAY
};
static_assert( ARRAYSIZE( s_pTextureTypeNames ) == STUDIOAPI_TEXTURE_NUM_TYPES, "Array size 's_pTextureTypeNames' must be equal to STUDIOAPI_TEXTURE_NUM_TYPES" );

// Table for convert text to studioAPISamplerFilter_t
static const char* s_pSamplerFilterNames[] = {
	"point",	   // STUDIOAPI_SAMPLER_FILTER_POINT
	"bilinear",	   // STUDIOAPI_SAMPLER_FILTER_BILINEAR
	"trilinear",   // STUDIOAPI_SAMPLER_FILTER_TRILINEAR
	"anisotropic"  // STUDIOAPI_SAMPLER_FILTER_ANISOTROPIC
};
static_assert( ARRAYSIZE( s_pSamplerFilterNames ) == STUDIOAPI_SAMPLER_NUM_FILTERS, "Array size 's_pSamplerFilterNames' must be equal to STUDIOAPI_SAMPLER_NUM_FILTERS" );

// Table for convert text to studioAPISamplerAddressMode_t
static const char* s_pSamplerAddressModeNames[] = {
	"wrap",			 // STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP
	"clamp",		 // STUDIOAPI_SAMPLER_ADDRESS_MODE_CLAMP
	"mirror_wrap",	 // STUDIOAPI_SAMPLER_ADDRESS_MODE_MIRROR_WRAP
	"mirror_clamp",	 // STUDIOAPI_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP
	"border"		 // STUDIOAPI_SAMPLER_ADDRESS_MODE_BORDER
};
static_assert( ARRAYSIZE( s_pSamplerAddressModeNames ) == STUDIOAPI_SAMPLER_ADDRESS_NUM_MODES, "Array size 's_pSamplerAddressModeNames' must be equal to STUDIOAPI_SAMPLER_ADDRESS_NUM_MODES" );

// Table for convert text to studioAPIPixelFormat_t
static const char* s_pPixelFormatNames[] = {
	"unknown",			 // STUDIOAPI_PIXEL_FORMAT_UNKNOWN
	"rgba8",			 // STUDIOAPI_PIXEL_FORMAT_RGBA8
	"rg8",				 // STUDIOAPI_PIXEL_FORMAT_RG8
	"r8",				 // STUDIOAPI_PIXEL_FORMAT_R8
	"rgba16",			 // STUDIOAPI_PIXEL_FORMAT_RGBA16
	"rg16",				 // STUDIOAPI_PIXEL_FORMAT_RG16
	"r16",				 // STUDIOAPI_PIXEL_FORMAT_R16
	"rgba32f",			 // STUDIOAPI_PIXEL_FORMAT_RGBA32F
	"rg32f",			 // STUDIOAPI_PIXEL_FORMAT_RG32F
	"r32f",				 // STUDIOAPI_PIXEL_FORMAT_R32F
	"rgba16f",			 // STUDIOAPI_PIXEL_FORMAT_RGBA16F
	"rg16f",			 // STUDIOAPI_PIXEL_FORMAT_RG16F
	"r16f",				 // STUDIOAPI_PIXEL_FORMAT_R16F
	"bc1",				 // STUDIOAPI_PIXEL_FORMAT_BC1
	"bc2",				 // STUDIOAPI_PIXEL_FORMAT_BC2
	"bc3",				 // STUDIOAPI_PIXEL_FORMAT_BC3
	"bc4",				 // STUDIOAPI_PIXEL_FORMAT_BC4
	"bc5",				 // STUDIOAPI_PIXEL_FORMAT_BC5
	"bc6h",				 // STUDIOAPI_PIXEL_FORMAT_BC6H
	"bc7",				 // STUDIOAPI_PIXEL_FORMAT_BC7
	"depth16",			 // STUDIOAPI_PIXEL_FORMAT_DEPTH16
	"depth32f",			 // STUDIOAPI_PIXEL_FORMAT_DEPTH32F
	"depth16_stencil8",	 // STUDIOAPI_PIXEL_FORMAT_DEPTH16_STENCIL8
	"depth24_stencil8",	 // STUDIOAPI_PIXEL_FORMAT_DEPTH24_STENCIL8
	"depth32f_stencil8"	 // STUDIOAPI_PIXEL_FORMAT_DEPTH32F_STENCIL8
};
static_assert( ARRAYSIZE( s_pPixelFormatNames ) == STUDIOAPI_PIXEL_NUM_FORMATS, "Array size 's_pPixelFormatNames' must be equal to STUDIOAPI_PIXEL_NUM_FORMATS" );

// Default pixel format
studioAPIPixelFormat_t CSTEXSourceTextureDoc::defaultPixelFormat = STUDIOAPI_PIXEL_FORMAT_UNKNOWN;

/*
==================
ConvTextToTextureType
==================
*/
static bool ConvTextToTextureType( const char* pString, studioAPITextureType_t& type )
{
	for ( uint32 index = 0, count = ARRAYSIZE( s_pTextureTypeNames ); index != count; ++index )
	{
		if ( !S_Stricmp( pString, s_pTextureTypeNames[index] ) )
		{
			// We found! Return current type
			type = (studioAPITextureType_t)index;
			return true;
		}
	}

	// We not found, return invalid type
	type = STUDIOAPI_TEXTURE_NUM_TYPES;
	return false;
}

/*
==================
ConvTextureTypeToText
==================
*/
static const char* ConvTextureTypeToText( studioAPITextureType_t type )
{
	Assert( type < STUDIOAPI_TEXTURE_NUM_TYPES );
	return s_pTextureTypeNames[type];
}

/*
==================
ConvTextToSamplerAddressMode
==================
*/
static bool ConvTextToSamplerAddressMode( const char* pString, studioAPISamplerAddressMode_t& samplerAddressMode )
{
	for ( uint32 index = 0, count = ARRAYSIZE( s_pSamplerAddressModeNames ); index != count; ++index )
	{
		if ( !S_Stricmp( pString, s_pSamplerAddressModeNames[index] ) )
		{
			// We found! Return current type
			samplerAddressMode = (studioAPISamplerAddressMode_t)index;
			return true;
		}
	}

	// We not found, return invalid type
	samplerAddressMode = STUDIOAPI_SAMPLER_ADDRESS_NUM_MODES;
	return false;
}

/*
==================
ConvSamplerAddressModeToText
==================
*/
static const char* ConvSamplerAddressModeToText( studioAPISamplerAddressMode_t samplerAddressMode )
{
	Assert( samplerAddressMode < STUDIOAPI_PIXEL_NUM_FORMATS );
	return s_pSamplerAddressModeNames[samplerAddressMode];
}

/*
==================
ConvTextToSamplerFilter
==================
*/
static bool ConvTextToSamplerFilter( const char* pString, studioAPISamplerFilter_t& samplerFilter )
{
	for ( uint32 index = 0, count = ARRAYSIZE( s_pSamplerFilterNames ); index != count; ++index )
	{
		if ( !S_Stricmp( pString, s_pSamplerFilterNames[index] ) )
		{
			// We found! Return current type
			samplerFilter = (studioAPISamplerFilter_t)index;
			return true;
		}
	}

	// We not found, return invalid type
	samplerFilter = STUDIOAPI_SAMPLER_NUM_FILTERS;
	return false;
}

/*
==================
ConvSamplerFilterToText
==================
*/
static const char* ConvSamplerFilterToText( studioAPISamplerFilter_t samplerFilter )
{
	Assert( samplerFilter < STUDIOAPI_SAMPLER_NUM_FILTERS );
	return s_pSamplerFilterNames[samplerFilter];
}

/*
==================
ConvTextToPixelFormat
==================
*/
static bool ConvTextToPixelFormat( const char* pString, studioAPIPixelFormat_t& pixelFormat )
{
	for ( uint32 index = 0, count = ARRAYSIZE( s_pPixelFormatNames ); index != count; ++index )
	{
		if ( !S_Stricmp( pString, s_pPixelFormatNames[index] ) )
		{
			// We found! Return current type
			pixelFormat = (studioAPIPixelFormat_t)index;
			return true;
		}
	}

	// We not found, return invalid type
	pixelFormat = STUDIOAPI_PIXEL_NUM_FORMATS;
	return false;
}

/*
==================
ConvPixelFormatToText
==================
*/
static const char* ConvPixelFormatToText( studioAPIPixelFormat_t pixelFormat )
{
	Assert( pixelFormat < STUDIOAPI_PIXEL_NUM_FORMATS );
	return s_pPixelFormatNames[pixelFormat];
}

/*
==================
CSTEXSourceTextureDoc::CSTEXSourceTextureDoc
==================
*/
CSTEXSourceTextureDoc::CSTEXSourceTextureDoc()
	: bGenerateMipMaps( false )
	, type( STUDIOAPI_TEXTURE_TYPE_1D )
	, addressModeU( STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP )
	, addressModeV( STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP )
	, addressModeW( STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP )
	, filter( STUDIOAPI_SAMPLER_FILTER_POINT )
	, pixelFormat( STUDIOAPI_PIXEL_FORMAT_UNKNOWN )
	, maxAnisotropy( 0 )
{
}

/*
==================
CSTEXSourceTextureDoc::LoadFromFile
==================
*/
bool CSTEXSourceTextureDoc::LoadFromFile( const char* pPath )
{
	// Load key values file
	PROFILE_SCOPE();
	CKeyValues keyValues( "stex" );
	if ( !keyValues.LoadFromFile( pPath ) )
	{
		return false;
	}
	Clear();

	// Get texture type
	bool		bGotDefaultValue = false;
	const char* pStringValue	 = keyValues.GetString( "type", "", NULL, &bGotDefaultValue );
	if ( bGotDefaultValue )
	{
		Error( "STEXDoc: Invalid STEX, not found required field 'type' (file: '%s')", pPath );
		return false;
	}
	if ( !ConvTextToTextureType( pStringValue, type ) )
	{
		Error( "STEXDoc: Invalid STEX, unknown or unsupported texture type '%s' (file: '%s')", pStringValue, pPath );
		return false;
	}

	// Get an address mode by U coord
	pStringValue = keyValues.GetString( "address_mode_u", "wrap" );
	if ( !ConvTextToSamplerAddressMode( pStringValue, addressModeU ) )
	{
		Error( "STEXDoc: Invalid STEX, unknown or unsupported sampler address mode '%s' (file: '%s')", pStringValue, pPath );
		return false;
	}

	// Get an address mode by V coord
	pStringValue = keyValues.GetString( "address_mode_v", "wrap" );
	if ( !ConvTextToSamplerAddressMode( pStringValue, addressModeV ) )
	{
		Error( "STEXDoc: Invalid STEX, unknown or unsupported sampler address mode '%s' (file: '%s')", pStringValue, pPath );
		return false;
	}

	// Get an address mode by W coord
	pStringValue = keyValues.GetString( "address_mode_w", "wrap" );
	if ( !ConvTextToSamplerAddressMode( pStringValue, addressModeW ) )
	{
		Error( "STEXDoc: Invalid STEX, unknown or unsupported sampler address mode '%s' (file: '%s')", pStringValue, pPath );
		return false;
	}

	// Get a filter mode
	pStringValue = keyValues.GetString( "filter", "point" );
	if ( !ConvTextToSamplerFilter( pStringValue, filter ) )
	{
		Error( "STEXDoc: Invalid STEX, unknown or unsupported sampler filter '%s' (file: '%s')", pStringValue, pPath );
		return false;
	}

	// Get a pixel format
	pStringValue = keyValues.GetString( "format", ConvPixelFormatToText( defaultPixelFormat ) );
	if ( !ConvTextToPixelFormat( pStringValue, pixelFormat ) )
	{
		Error( "STEXDoc: Invalid STEX, unknown or unsupported pixel format '%s' (file: '%s')", pStringValue, pPath );
		return false;
	}

	// Get source files
	for ( CKeyValuesSubKeysIterator it( &keyValues, "source" ); it; ++it )
	{
		pStringValue = it->GetString( NULL );
		if ( !pStringValue || !pStringValue[0] )
		{
			Error( "STEXDoc: Invalid STEX, a source file can't be empty (file: '%s')", pPath );
			return false;
		}

		sourcePaths.emplace_back( pStringValue );
	}

	// Validate source paths number
	eastl::string validateMsg;
	if ( !IsValidNumSourcePaths( type, (uint32)sourcePaths.size(), validateMsg ) )
	{
		Error( "STEXDoc: Invalid STEX, %s", validateMsg.c_str() );
		return false;
	}

	// Get other parameters
	maxAnisotropy	 = keyValues.GetInt( "max_anisotropy" );
	bGenerateMipMaps = keyValues.GetBool( "generate_mipmaps" );
	return true;
}

/*
==================
CSTEXSourceTextureDoc::SaveFile
==================
*/
bool CSTEXSourceTextureDoc::SaveFile( const char* pPath )
{
	// Validate source paths number
	PROFILE_SCOPE();
	eastl::string validateMsg;
	if ( !IsValidNumSourcePaths( type, (uint32)sourcePaths.size(), validateMsg ) )
	{
		Error( "STEXDoc: %s", validateMsg.c_str() );
		return false;
	}

	// Create key values
	CKeyValues keyValues( "stex" );
	keyValues.SetString( "type", ConvTextureTypeToText( type ) );
	keyValues.SetString( "format", ConvPixelFormatToText( pixelFormat ) );
	keyValues.SetString( "filter", ConvSamplerFilterToText( filter ) );
	keyValues.SetString( "address_mode_u", ConvSamplerAddressModeToText( addressModeU ) );
	keyValues.SetString( "address_mode_v", ConvSamplerAddressModeToText( addressModeV ) );
	keyValues.SetString( "address_mode_w", ConvSamplerAddressModeToText( addressModeW ) );
	keyValues.SetBool( "generate_mipmaps", bGenerateMipMaps );
	keyValues.SetInt( "max_anisotropy", maxAnisotropy );
	for ( uint32 index = 0, count = (uint32)sourcePaths.size(); index < count; ++index )
	{
		CKeyValues* pSource = new CKeyValues( "source", &keyValues );
		pSource->SetString( NULL, sourcePaths[index].c_str() );
	}

	// Save the key values to a file
	return keyValues.SaveToFile( pPath );
}
