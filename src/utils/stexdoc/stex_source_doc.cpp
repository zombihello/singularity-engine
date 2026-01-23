#include "utils/interfaces/interfaces.h"
#include "core/profile.h"
#include "filesystem/ifilesystem.h"
#include "stdlib/jsondoc.h"
#include "utils/stexdoc/stex_source_doc.h"

// Table for convert text to studioAPITextureType_t
static const char* s_pTextureTypeNames[] = {
	"1d",		  // STUDIOAPI_TEXTURE_TYPE_1D
	"2d",		  // STUDIOAPI_TEXTURE_TYPE_2D
	"cube",		  // STUDIOAPI_TEXTURE_TYPE_CUBE
	"3d",		  // STUDIOAPI_TEXTURE_TYPE_3D
	"1d-array",	  // STUDIOAPI_TEXTURE_TYPE_1D_ARRAY
	"2d-array",	  // STUDIOAPI_TEXTURE_TYPE_2D_ARRAY
	"cube-array"  // STUDIOAPI_TEXTURE_TYPE_CUBE_ARRAY
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
	"mirror-wrap",	 // STUDIOAPI_SAMPLER_ADDRESS_MODE_MIRROR_WRAP
	"mirror-clamp",	 // STUDIOAPI_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP
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
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Load a JSON file
	CJsonDoc jsonDoc;
	if ( !jsonDoc.LoadFromFile( pPath ) )
	{
		Warning( "STEXDoc: Failed to load '%s', maybe wrong JSON syntax?", pPath );
		return false;
	}
	Clear();
	bool bResult = true;

	// Get texture type
	{
		CJsonValue jsonType = jsonDoc.GetValue( "type" );
		if ( jsonType.IsValid() )
		{
			if ( jsonType.IsA( JSONVALUE_TYPE_STRING ) )
			{
				eastl::string typeName = jsonType.GetString();
				if ( !ConvTextToTextureType( typeName.c_str(), type ) )
				{
					Error( "STEXDoc: Invalid 'type', unknown or unsupported texture type '%s'", typeName.c_str() );
					bResult = false;
				}
			}
			else
			{
				Error( "STEXDoc: Invalid 'type', must be string type" );
				bResult = false;
			}
		}
		else
		{
			Error( "STEXDoc: A source texture '%s' must have 'type' field", pPath );
			bResult = false;
		}
	}

	// Is need generate mipmaps
	{
		CJsonValue jsonGenerateMipmaps = jsonDoc.GetValue( "generate-mipmaps" );
		if ( jsonGenerateMipmaps.IsValid() )
		{
			if ( jsonGenerateMipmaps.IsNumber() )
			{
				bGenerateMipMaps = jsonGenerateMipmaps.GetNumber() > 0.f;
			}
			else
			{
				Error( "STEXDoc: Invalid 'generate-mipmaps', must be number type" );
				bResult = false;
			}
		}
		else
		{
			bGenerateMipMaps = false;
		}
	}

	// Get an address mode by U coord
	{
		CJsonValue jsonAddressModeU = jsonDoc.GetValue( "address-mode-u" );
		if ( jsonAddressModeU.IsValid() )
		{
			if ( jsonAddressModeU.IsA( JSONVALUE_TYPE_STRING ) )
			{
				eastl::string addressModeUName = jsonAddressModeU.GetString();
				if ( !ConvTextToSamplerAddressMode( addressModeUName.c_str(), addressModeU ) )
				{
					Error( "STEXDoc: Invalid 'address-mode-u', unknown or unsupported sampler address mode '%s'", addressModeUName.c_str() );
					bResult = false;
				}
			}
			else
			{
				Error( "STEXDoc: Invalid 'address-mode-u', must be string type" );
				bResult = false;
			}
		}
		else
		{
			addressModeU = STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
		}
	}

	// Get an address mode by V coord
	{
		CJsonValue jsonAddressModeV = jsonDoc.GetValue( "address-mode-v" );
		if ( jsonAddressModeV.IsValid() )
		{
			if ( jsonAddressModeV.IsA( JSONVALUE_TYPE_STRING ) )
			{
				eastl::string addressModeVName = jsonAddressModeV.GetString();
				if ( !ConvTextToSamplerAddressMode( addressModeVName.c_str(), addressModeV ) )
				{
					Error( "STEXDoc: Invalid 'address-mode-v', unknown or unsupported sampler address mode '%s'", addressModeVName.c_str() );
					bResult = false;
				}
			}
			else
			{
				Error( "STEXDoc: Invalid 'address-mode-v', must be string type" );
				bResult = false;
			}
		}
		else
		{
			addressModeV = STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
		}
	}

	// Get an address mode by W coord
	{
		CJsonValue jsonAddressModeW = jsonDoc.GetValue( "address-mode-w" );
		if ( jsonAddressModeW.IsValid() )
		{
			if ( jsonAddressModeW.IsA( JSONVALUE_TYPE_STRING ) )
			{
				eastl::string addressModeWName = jsonAddressModeW.GetString();
				if ( !ConvTextToSamplerAddressMode( addressModeWName.c_str(), addressModeW ) )
				{
					Error( "STEXDoc: Invalid 'address-mode-w', unknown or unsupported sampler address mode '%s'", addressModeWName.c_str() );
					bResult = false;
				}
			}
			else
			{
				Error( "STEXDoc: Invalid 'address-mode-w', must be string type" );
				bResult = false;
			}
		}
		else
		{
			addressModeW = STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
		}
	}

	// Get a filter mode
	{
		CJsonValue jsonFilter = jsonDoc.GetValue( "filter" );
		if ( jsonFilter.IsValid() )
		{
			if ( jsonFilter.IsA( JSONVALUE_TYPE_STRING ) )
			{
				eastl::string filterName = jsonFilter.GetString();
				if ( !ConvTextToSamplerFilter( filterName.c_str(), filter ) )
				{
					Error( "STEXDoc: Invalid 'filter', unknown or unsupported sampler filter '%s'", filterName.c_str() );
					bResult = false;
				}
			}
			else
			{
				Error( "STEXDoc: Invalid 'filter', must be string type" );
				bResult = false;
			}
		}
		else
		{
			filter = STUDIOAPI_SAMPLER_FILTER_POINT;
		}
	}

	// Get a pixel format
	{
		CJsonValue jsonPixelFormat = jsonDoc.GetValue( "format" );
		if ( jsonPixelFormat.IsValid() )
		{
			if ( jsonPixelFormat.IsA( JSONVALUE_TYPE_STRING ) )
			{
				eastl::string pixelFormatName = jsonPixelFormat.GetString();
				if ( !ConvTextToPixelFormat( pixelFormatName.c_str(), pixelFormat ) )
				{
					Error( "STEXDoc: Invalid 'format', unknown or unsupported pixel format '%s'", pixelFormatName.c_str() );
					bResult = false;
				}
			}
			else
			{
				Error( "STEXDoc: Invalid 'format', must be string type" );
				bResult = false;
			}
		}
		else
		{
			pixelFormat = defaultPixelFormat;
		}
	}

	// Get a source file
	{
		CJsonValue jsonSources = jsonDoc.GetValue( "sources" );
		if ( jsonSources.IsValid() )
		{
			if ( jsonSources.IsA( JSONVALUE_TYPE_ARRAY ) )
			{
				eastl::string			  validateMsg;
				eastl::vector<CJsonValue> jsonArray = jsonSources.GetArray();
				if ( !IsValidNumSourcePaths( type, (uint32)jsonArray.size(), validateMsg ) )
				{
					Error( "STEXDoc: Invalid 'sources', %s", validateMsg.c_str() );
					bResult = false;
				}

				for ( uint32 index = 0, count = (uint32)jsonArray.size(); index < count; ++index )
				{
					const CJsonValue& jsonSource = jsonArray[index];
					if ( jsonSource.IsValid() && jsonSource.IsA( JSONVALUE_TYPE_STRING ) )
					{
						eastl::string sourcePath = jsonSource.GetString();
						if ( sourcePath.empty() )
						{
							Error( "STEXDoc: Invalid 'sources[%i]', a source file can't be empty", index );
							bResult = false;
							continue;
						}

						sourcePaths.emplace_back( sourcePath );
					}
					else
					{
						Error( "STEXDoc: Invalid 'sources[%i]', must be string type", index );
						bResult = false;
					}
				}
			}
			else
			{
				Error( "STEXDoc: Invalid 'sources', must be array of string type" );
				bResult = false;
			}
		}
		else
		{
			Error( "STEXDoc: A source texture '%s' must have 'sources' field", pPath );
			bResult = false;
		}
	}

	// Get a destination file
	{
		CJsonValue jsonOutputDir = jsonDoc.GetValue( "output-dir" );
		if ( jsonOutputDir.IsValid() )
		{
			if ( jsonOutputDir.IsA( JSONVALUE_TYPE_STRING ) )
			{
				eastl::string outputDir = jsonOutputDir.GetString();
				if ( outputDir.empty() )
				{
					Error( "STEXDoc: Invalid 'output-dir', an output directory can't be empty" );
					bResult = false;
				}

				CSTEXSourceTextureDoc::outputDir = outputDir;
			}
			else
			{
				Error( "STEXDoc: Invalid 'output-dir', must be string type" );
				bResult = false;
			}
		}
		else
		{
			Error( "STEXDoc: A source texture '%s' must have 'output-dir' field", pPath );
			bResult = false;
		}
	}

	// Get max anisotropy
	{
		CJsonValue jsonMaxAnisotropy = jsonDoc.GetValue( "max-anisotropy" );
		if ( jsonMaxAnisotropy.IsValid() )
		{
			if ( jsonMaxAnisotropy.IsNumber() )
			{
				maxAnisotropy = (uint32)jsonMaxAnisotropy.GetNumber();
			}
			else
			{
				Error( "STEXDoc: Invalid 'max-anisotropy', must be number type" );
				bResult = false;
			}
		}
		else
		{
			maxAnisotropy = 0;
		}
	}

	return bResult;
}

/*
==================
CSTEXSourceTextureDoc::SaveFile
==================
*/
bool CSTEXSourceTextureDoc::SaveFile( const char* pPath )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try to open a file
	TRefPtr<IStreamDataWriter> pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Error( "STEXDoc: Failed to open file '%s' for save a STEX source texture", pPath );
		return false;
	}

	// Validate source paths number
	eastl::string validateMsg;
	if ( !IsValidNumSourcePaths( type, (uint32)sourcePaths.size(), validateMsg ) )
	{
		Error( "STEXDoc: %s", validateMsg.c_str() );
		return false;
	}

	// Combine all source paths to one string
	eastl::string sources;
	for ( uint32 index = 0, count = (uint32)sourcePaths.size(); index < count; ++index )
	{
		sources += S_Sprintf( "\"%s\"%s", sourcePaths[index].c_str(), index + 1 < count ? ", " : "" );
	}

	// Write the JSON file
	eastl::string buffer;
	buffer += "{\n";
	buffer += S_Sprintf( "\t\"type\":\t%s,\n", ConvTextureTypeToText( type ) );
	buffer += S_Sprintf( "\t\"sources\":\t[ %s ],\n", sources.c_str() );
	buffer += S_Sprintf( "\t\"generate-mipmaps\":\t%s,\n", bGenerateMipMaps ? "true" : "false" );
	buffer += S_Sprintf( "\t\"address-mode-u\":\t\"%s\",\n", ConvSamplerAddressModeToText( addressModeU ) );
	buffer += S_Sprintf( "\t\"address-mode-v\":\t\"%s\",\n", ConvSamplerAddressModeToText( addressModeV ) );
	buffer += S_Sprintf( "\t\"address-mode-w\":\t\"%s\",\n", ConvSamplerAddressModeToText( addressModeW ) );
	buffer += S_Sprintf( "\t\"filter\":\t\"%s\",\n", ConvSamplerFilterToText( filter ) );
	buffer += S_Sprintf( "\t\"format\":\t\"%s\",\n", ConvPixelFormatToText( pixelFormat ) );
	buffer += S_Sprintf( "\t\"output-dir\":\t\"%s\",\n", outputDir.c_str() );
	buffer += S_Sprintf( "\t\"max-anisotropy\":\t\"%i\"\n", maxAnisotropy );
	buffer += "}\n";

	pFile->Write( buffer.data(), buffer.size() * sizeof( char ) );
	return true;
}
