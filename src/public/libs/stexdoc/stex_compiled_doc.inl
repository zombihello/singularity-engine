#pragma once

/*
==================
CSTEXCompiledTextureDoc::SetData
==================
*/
FORCEINLINE void CSTEXCompiledTextureDoc::SetData( studioAPITextureType_t type, uint32 numLayers, const stexTextureMipMaps_t& mipmaps, const std::vector<byte>& data, studioAPIPixelFormat_t pixelFormat )
{
	Validate( type, numLayers, mipmaps );
	CSTEXCompiledTextureDoc::type		 = type;
	CSTEXCompiledTextureDoc::numLayers	 = numLayers;
	CSTEXCompiledTextureDoc::mipmaps	 = mipmaps;
	CSTEXCompiledTextureDoc::data		 = data;
	CSTEXCompiledTextureDoc::pixelFormat = pixelFormat;
}

/*
==================
CSTEXCompiledTextureDoc::SetData
==================
*/
FORCEINLINE void CSTEXCompiledTextureDoc::SetData( studioAPITextureType_t type, uint32 numLayers, stexTextureMipMaps_t& mipmaps, std::vector<byte>& data, studioAPIPixelFormat_t pixelFormat )
{
	Validate( type, numLayers, mipmaps );
	CSTEXCompiledTextureDoc::type		 = type;
	CSTEXCompiledTextureDoc::numLayers	 = numLayers;
	CSTEXCompiledTextureDoc::mipmaps	 = std::move( mipmaps );
	CSTEXCompiledTextureDoc::data		 = std::move( data );
	CSTEXCompiledTextureDoc::pixelFormat = pixelFormat;
}

/*
==================
CSTEXCompiledTextureDoc::GetPixelFormat
==================
*/
FORCEINLINE studioAPIPixelFormat_t CSTEXCompiledTextureDoc::GetPixelFormat() const
{
	return pixelFormat;
}

/*
==================
CSTEXCompiledTextureDoc::SetAddressModeU
==================
*/
FORCEINLINE void CSTEXCompiledTextureDoc::SetAddressModeU( studioAPISamplerAddressMode_t addressModeU )
{
	CSTEXCompiledTextureDoc::addressModeU = addressModeU;
}

/*
==================
CSTEXCompiledTextureDoc::GetAddressModeU
==================
*/
FORCEINLINE studioAPISamplerAddressMode_t CSTEXCompiledTextureDoc::GetAddressModeU() const
{
	return addressModeU;
}

/*
==================
CSTEXCompiledTextureDoc::SetAddressModeV
==================
*/
FORCEINLINE void CSTEXCompiledTextureDoc::SetAddressModeV( studioAPISamplerAddressMode_t addressModeV )
{
	CSTEXCompiledTextureDoc::addressModeV = addressModeV;
}

/*
==================
CSTEXCompiledTextureDoc::GetAddressModeV
==================
*/
FORCEINLINE studioAPISamplerAddressMode_t CSTEXCompiledTextureDoc::GetAddressModeV() const
{
	return addressModeV;
}

/*
==================
CSTEXCompiledTextureDoc::SetAddressModeW
==================
*/
FORCEINLINE void CSTEXCompiledTextureDoc::SetAddressModeW( studioAPISamplerAddressMode_t addressModeW )
{
	CSTEXCompiledTextureDoc::addressModeW = addressModeW;
}

/*
==================
CSTEXCompiledTextureDoc::GetAddressModeW
==================
*/
FORCEINLINE studioAPISamplerAddressMode_t CSTEXCompiledTextureDoc::GetAddressModeW() const
{
	return addressModeW;
}

/*
==================
CSTEXCompiledTextureDoc::SetFilter
==================
*/
FORCEINLINE void CSTEXCompiledTextureDoc::SetFilter( studioAPISamplerFilter_t filter )
{
	CSTEXCompiledTextureDoc::filter = filter;
}

/*
==================
CSTEXCompiledTextureDoc::GetFilter
==================
*/
FORCEINLINE studioAPISamplerFilter_t CSTEXCompiledTextureDoc::GetFilter() const
{
	return filter;
}

/*
==================
CSTEXCompiledTextureDoc::SetMaxAnisotropy
==================
*/
FORCEINLINE void CSTEXCompiledTextureDoc::SetMaxAnisotropy( uint32 maxAnisotropy )
{
	CSTEXCompiledTextureDoc::maxAnisotropy = maxAnisotropy;
}

/*
==================
CSTEXCompiledTextureDoc::GetMaxAnisotropy
==================
*/
FORCEINLINE uint32 CSTEXCompiledTextureDoc::GetMaxAnisotropy() const
{
	return maxAnisotropy;
}

/*
==================
CSTEXCompiledTextureDoc::GetNumLayers
==================
*/
FORCEINLINE uint32 CSTEXCompiledTextureDoc::GetNumLayers() const
{
	return numLayers;
}

/*
==================
CSTEXCompiledTextureDoc::GetNumMips
==================
*/
FORCEINLINE uint32 CSTEXCompiledTextureDoc::GetNumMips() const
{
	return (uint32)mipmaps.size();
}

/*
==================
CSTEXCompiledTextureDoc::GetSizeX
==================
*/
FORCEINLINE uint32 CSTEXCompiledTextureDoc::GetSizeX( uint32 mipLevel /*= 0*/ ) const
{
	Assert( mipLevel < mipmaps.size() );
	return mipmaps[mipLevel].sizeX;
}

/*
==================
CSTEXCompiledTextureDoc::GetSizeY
==================
*/
FORCEINLINE uint32 CSTEXCompiledTextureDoc::GetSizeY( uint32 mipLevel /*= 0*/ ) const
{
	Assert( mipLevel < mipmaps.size() );
	return mipmaps[mipLevel].sizeY;
}

/*
==================
CSTEXCompiledTextureDoc::GetSizeZ
==================
*/
FORCEINLINE uint32 CSTEXCompiledTextureDoc::GetSizeZ( uint32 mipLevel /*= 0*/ ) const
{
	Assert( mipLevel < mipmaps.size() );
	return mipmaps[mipLevel].sizeZ;
}

/*
==================
CSTEXCompiledTextureDoc::GetMip
==================
*/
FORCEINLINE const stexTextureMipMap_t& CSTEXCompiledTextureDoc::GetMip( uint32 mipLevel ) const
{
	Assert( mipLevel < mipmaps.size() );
	return mipmaps[mipLevel];
}

/*
==================
CSTEXCompiledTextureDoc::GetMipmaps
==================
*/
FORCEINLINE const stexTextureMipMaps_t& CSTEXCompiledTextureDoc::GetMipmaps() const
{
	return mipmaps;
}

/*
==================
CSTEXCompiledTextureDoc::GetData
==================
*/
FORCEINLINE const std::vector<byte>& CSTEXCompiledTextureDoc::GetData() const
{
	return data;
}

/*
==================
CSTEXCompiledTextureDoc::GetType
==================
*/
FORCEINLINE studioAPITextureType_t CSTEXCompiledTextureDoc::GetType() const
{
	return type;
}

/*
==================
CSTEXCompiledTextureDoc::Clear
==================
*/
FORCEINLINE void CSTEXCompiledTextureDoc::Clear()
{
	type		  = STUDIOAPI_TEXTURE_TYPE_1D;
	pixelFormat	  = STUDIOAPI_PIXEL_FORMAT_UNKNOWN;
	numLayers	  = 0;
	maxAnisotropy = 0;
	addressModeU  = STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
	addressModeV  = STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
	addressModeW  = STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
	filter		  = STUDIOAPI_SAMPLER_FILTER_POINT;
	mipmaps.clear();
	data.clear();
}
