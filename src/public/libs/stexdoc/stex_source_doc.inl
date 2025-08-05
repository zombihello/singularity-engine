#pragma once

/*
==================
CSTEXSourceTextureDoc::SetGenerateMipMaps
==================
*/
FORCEINLINE void CSTEXSourceTextureDoc::SetGenerateMipMaps( bool bGenerateMipMaps )
{
	CSTEXSourceTextureDoc::bGenerateMipMaps = bGenerateMipMaps;
}

/*
==================
CSTEXSourceTextureDoc::IsNeedGenerateMipMaps
==================
*/
FORCEINLINE bool CSTEXSourceTextureDoc::IsNeedGenerateMipMaps() const
{
	return bGenerateMipMaps;
}

/*
==================
CSTEXSourceTextureDoc::SetAddressModeU
==================
*/
FORCEINLINE void CSTEXSourceTextureDoc::SetAddressModeU( studioAPISamplerAddressMode_t addressModeU )
{
	CSTEXSourceTextureDoc::addressModeU = addressModeU;
}

/*
==================
CSTEXSourceTextureDoc::GetAddressModeU
==================
*/
FORCEINLINE studioAPISamplerAddressMode_t CSTEXSourceTextureDoc::GetAddressModeU() const
{
	return addressModeU;
}

/*
==================
CSTEXSourceTextureDoc::SetAddressModeV
==================
*/
FORCEINLINE void CSTEXSourceTextureDoc::SetAddressModeV( studioAPISamplerAddressMode_t addressModeV )
{
	CSTEXSourceTextureDoc::addressModeV = addressModeV;
}

/*
==================
CSTEXSourceTextureDoc::GetAddressModeV
==================
*/
FORCEINLINE studioAPISamplerAddressMode_t CSTEXSourceTextureDoc::GetAddressModeV() const
{
	return addressModeV;
}

/*
==================
CSTEXSourceTextureDoc::SetAddressModeW
==================
*/
FORCEINLINE void CSTEXSourceTextureDoc::SetAddressModeW( studioAPISamplerAddressMode_t addressModeW )
{
	CSTEXSourceTextureDoc::addressModeW = addressModeW;
}

/*
==================
CSTEXSourceTextureDoc::GetAddressModeW
==================
*/
FORCEINLINE studioAPISamplerAddressMode_t CSTEXSourceTextureDoc::GetAddressModeW() const
{
	return addressModeW;
}

/*
==================
CSTEXSourceTextureDoc::SetFilter
==================
*/
FORCEINLINE void CSTEXSourceTextureDoc::SetFilter( studioAPISamplerFilter_t filter )
{
	CSTEXSourceTextureDoc::filter = filter;
}

/*
==================
CSTEXSourceTextureDoc::GetFilter
==================
*/
FORCEINLINE studioAPISamplerFilter_t CSTEXSourceTextureDoc::GetFilter() const
{
	return filter;
}

/*
==================
CSTEXSourceTextureDoc::SetPixelFormat
==================
*/
FORCEINLINE void CSTEXSourceTextureDoc::SetPixelFormat( studioAPIPixelFormat_t pixelFormat )
{
	CSTEXSourceTextureDoc::pixelFormat = pixelFormat;
}

/*
==================
CSTEXSourceTextureDoc::GetPixelFormat
==================
*/
FORCEINLINE studioAPIPixelFormat_t CSTEXSourceTextureDoc::GetPixelFormat() const
{
	return pixelFormat;
}

/*
==================
CSTEXSourceTextureDoc::SetDefaultPixelFormat
==================
*/
FORCEINLINE void CSTEXSourceTextureDoc::SetDefaultPixelFormat( studioAPIPixelFormat_t pixelFormat )
{
	defaultPixelFormat = pixelFormat;
}

/*
==================
CSTEXSourceTextureDoc::SetType
==================
*/
FORCEINLINE void CSTEXSourceTextureDoc::SetType( studioAPITextureType_t type )
{
	CSTEXSourceTextureDoc::type = type;
}

/*
==================
CSTEXSourceTextureDoc::GetType
==================
*/
FORCEINLINE studioAPITextureType_t CSTEXSourceTextureDoc::GetType() const
{
	return type;
}

/*
==================
CSTEXSourceTextureDoc::AddSourcePath
==================
*/
FORCEINLINE void CSTEXSourceTextureDoc::AddSourcePath( const achar* pSourcePath )
{
	sourcePaths.emplace_back( pSourcePath );
}

/*
==================
CSTEXSourceTextureDoc::RemoveSourcePath
==================
*/
FORCEINLINE void CSTEXSourceTextureDoc::RemoveSourcePath( uint32 sourcePathIdx )
{
	Assert( sourcePathIdx < ( uint32 )sourcePaths.size() );
	sourcePaths.erase( sourcePaths.begin() + sourcePathIdx );
}

/*
==================
CSTEXSourceTextureDoc::GetNumSourcePaths
==================
*/
FORCEINLINE uint32 CSTEXSourceTextureDoc::GetNumSourcePaths() const
{
	return ( uint32 )sourcePaths.size();
}

/*
==================
CSTEXSourceTextureDoc::GetSourcePath
==================
*/
FORCEINLINE const achar* CSTEXSourceTextureDoc::GetSourcePath( uint32 sourcePathIdx )
{
	Assert( sourcePathIdx < ( uint32 )sourcePaths.size() );
	return sourcePaths[sourcePathIdx].c_str();
}

/*
==================
CSTEXSourceTextureDoc::GetSourcePaths
==================
*/
FORCEINLINE const std::vector<std::string>& CSTEXSourceTextureDoc::GetSourcePaths() const
{
	return sourcePaths;
}

/*
==================
CSTEXSourceTextureDoc::SetMaxAnisotropy
==================
*/
FORCEINLINE void CSTEXSourceTextureDoc::SetMaxAnisotropy( uint32 maxAnisotropy )
{
	CSTEXSourceTextureDoc::maxAnisotropy = maxAnisotropy;
}

/*
==================
CSTEXSourceTextureDoc::GetMaxAnisotropy
==================
*/
FORCEINLINE uint32 CSTEXSourceTextureDoc::GetMaxAnisotropy() const
{
	return maxAnisotropy;
}

/*
==================
CSTEXSourceTextureDoc::SetOutputDir
==================
*/
FORCEINLINE void CSTEXSourceTextureDoc::SetOutputDir( const achar* pOutputDir )
{
	outputDir = pOutputDir;
}

/*
==================
CSTEXSourceTextureDoc::GetOutputDir
==================
*/
FORCEINLINE const achar* CSTEXSourceTextureDoc::GetOutputDir() const
{
	return outputDir.c_str();
}

/*
==================
CSTEXSourceTextureDoc::Clear
==================
*/
FORCEINLINE void CSTEXSourceTextureDoc::Clear()
{
	bGenerateMipMaps = false;
	type = STUDIOAPI_TEXTURE_TYPE_1D;
	addressModeU = STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
	addressModeV = STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
	addressModeW = STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
	filter = STUDIOAPI_SAMPLER_FILTER_POINT;
	pixelFormat = STUDIOAPI_PIXEL_FORMAT_UNKNOWN;
	maxAnisotropy = 0;
	outputDir = "";
	sourcePaths.clear();
}

/*
==================
CSTEXSourceTextureDoc::IsValidNumSourcePaths
==================
*/
FORCEINLINE bool CSTEXSourceTextureDoc::IsValidNumSourcePaths( studioAPITextureType_t type, uint32 numSourcePaths, std::string& validateMsg ) const
{
	bool	bResult = true;
	switch ( type )
	{
	case STUDIOAPI_TEXTURE_TYPE_1D:
	case STUDIOAPI_TEXTURE_TYPE_1D_ARRAY:
		bResult = numSourcePaths == 1 || type == STUDIOAPI_TEXTURE_TYPE_1D_ARRAY;
		if ( !bResult )
		{
			validateMsg = "Sources number must be 1 for 1D textures";
		}
		break;

	case STUDIOAPI_TEXTURE_TYPE_2D:
	case STUDIOAPI_TEXTURE_TYPE_2D_ARRAY:
		bResult = numSourcePaths == 1 || type == STUDIOAPI_TEXTURE_TYPE_2D_ARRAY;
		if ( !bResult )
		{
			validateMsg = "Sources number must be 1 for 2D textures";
		}
		break;

	case STUDIOAPI_TEXTURE_TYPE_CUBE:
		bResult = numSourcePaths == STEX_CUBE_FACE_COUNT;
		if ( !bResult )
		{
			validateMsg = S_Sprintf( "Sources number must be %i for cube textures", STEX_CUBE_FACE_COUNT );
		}
		break;

	case STUDIOAPI_TEXTURE_TYPE_CUBE_ARRAY:
		bResult = numSourcePaths % STEX_CUBE_FACE_COUNT;
		if ( !bResult )
		{
			validateMsg = S_Sprintf( "Sources number must be multiple of %i for cube textures", STEX_CUBE_FACE_COUNT );
		}
		break;

	case STUDIOAPI_TEXTURE_TYPE_3D:
		bResult = numSourcePaths == 1;
		if ( !bResult )
		{
			validateMsg = "Sources number must be 1 for 3D textures";
		}
		break;
	}

	return bResult;
}
