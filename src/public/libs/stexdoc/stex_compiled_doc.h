#pragma once
#include <vector>

#include "core/core.h"
#include "studiorender/studioapi/istudioapi_texture.h"

//-----------------------------------------------------------------------------
// Helper for work with compiled STEX (Singularity Texture) files
//-----------------------------------------------------------------------------
struct stexTextureMipMap_t
{
	uint32 sizeX;
	uint32 sizeY;
	uint32 sizeZ;
};
typedef std::vector<stexTextureMipMap_t> stexTextureMipMaps_t;

class CSTEXCompiledTextureDoc
{
public:
	CSTEXCompiledTextureDoc();

	// Load and save a texture document
	// NOTE: For use StdLib must be connected by ConnectStdLib
	bool LoadFromFile( const char* pPath );
	bool SaveFile( const char* pPath );
	void Clear();

	void SetData( studioAPITextureType_t type, uint32 numLayers, const stexTextureMipMaps_t& mipmaps, const std::vector<byte>& data, studioAPIPixelFormat_t pixelFormat );
	void SetData( studioAPITextureType_t type, uint32 numLayers, stexTextureMipMaps_t& mipmaps, std::vector<byte>& data, studioAPIPixelFormat_t pixelFormat );
	void SetAddressModeU( studioAPISamplerAddressMode_t addressModeU );
	void SetAddressModeV( studioAPISamplerAddressMode_t addressModeV );
	void SetAddressModeW( studioAPISamplerAddressMode_t addressModeW );
	void SetFilter( studioAPISamplerFilter_t filter );
	void SetMaxAnisotropy( uint32 maxAnisotropy );

	studioAPIPixelFormat_t		  GetPixelFormat() const;
	studioAPISamplerAddressMode_t GetAddressModeU() const;
	studioAPISamplerAddressMode_t GetAddressModeV() const;
	studioAPISamplerAddressMode_t GetAddressModeW() const;
	studioAPISamplerFilter_t	  GetFilter() const;
	uint32						  GetMaxAnisotropy() const;
	uint32						  GetNumLayers() const;
	uint32						  GetNumMips() const;
	uint32						  GetSizeX( uint32 mipLevel = 0 ) const;
	uint32						  GetSizeY( uint32 mipLevel = 0 ) const;
	uint32						  GetSizeZ( uint32 mipLevel = 0 ) const;
	const stexTextureMipMap_t&	  GetMip( uint32 mipLevel ) const;
	const stexTextureMipMaps_t&	  GetMipmaps() const;
	const std::vector<byte>&	  GetData() const;
	studioAPITextureType_t		  GetType() const;

private:
	// Validate parameters (only for debug)
	void Validate( studioAPITextureType_t type, uint32 numLayers, const stexTextureMipMaps_t& mipmaps ) const;

	studioAPITextureType_t		  type;
	studioAPIPixelFormat_t		  pixelFormat;
	uint32						  numLayers;
	uint32						  maxAnisotropy;
	studioAPISamplerAddressMode_t addressModeU;
	studioAPISamplerAddressMode_t addressModeV;
	studioAPISamplerAddressMode_t addressModeW;
	studioAPISamplerFilter_t	  filter;
	stexTextureMipMaps_t		  mipmaps;
	std::vector<byte>			  data;
};

#include "stexdoc/stex_compiled_doc.inl"