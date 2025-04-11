/**
 * @file
 * @addtogroup stexdoc stexdoc
 */

#ifndef STEX_COMPILED_DOC_H
#define STEX_COMPILED_DOC_H

#include <vector>

#include "core/core.h"
#include "studiorender/studioapi/istudioapi_texture.h"

/**
 * @ingroup stexdoc
 * @brief STEX texture mipmap
 */
struct stexTextureMipMap_t
{
	uint32		sizeX;	/**< Mipmap width */
	uint32		sizeY;	/**< Mipmap height */
	uint32		sizeZ;	/**< Mipmap depth */
};

/**
 * @ingroup stxdoc
 * @brief STEX texture mipmap array
 */
typedef std::vector<stexTextureMipMap_t>		stexTextureMipMaps_t;

/**
 * @ingroup stexdoc
 * @brief Helper for work with compiled STEX (Singularity Texture) files
 */
class CSTEXCompiledTextureDoc
{
public:
	/**
	 * @brief Constructor
	 */
	CSTEXCompiledTextureDoc();

	/**
	 * @brief Load from a file
	 * @warning For use StdLib must be connected by ConnectStdLib
	 *
	 * @param pPath		Path to file
	 * @return Return TRUE when a file success loaded, otherwise return FALSE
	 */
	bool LoadFromFile( const achar* pPath );

	/**
	 * @brief Save the texture document
	 * @warning For use StdLib must be connected by ConnectStdLib
	 *
	 * @param pPath		Path to save
	 * @return Return TRUE if the texture was successes saved, otherwise return FALSE
	 */
	bool SaveFile( const achar* pPath );

	/**
	 * @brief Set data
	 * @param type				Texture type
	 * @param numLayers			Layers number
	 * @param mipmaps			Texture mipmaps
	 * @param data				Texture data
	 * @param pixelFormat		Pixel format
	 */
	FORCEINLINE void SetData( studioAPITextureType_t type, uint32 numLayers, const stexTextureMipMaps_t& mipmaps, const std::vector<byte>& data, studioAPIPixelFormat_t pixelFormat )
	{
		Validate( type, numLayers, mipmaps );
		CSTEXCompiledTextureDoc::type			= type;
		CSTEXCompiledTextureDoc::numLayers		= numLayers;
		CSTEXCompiledTextureDoc::mipmaps		= mipmaps;
		CSTEXCompiledTextureDoc::data			= data;
		CSTEXCompiledTextureDoc::pixelFormat	= pixelFormat;
	}

	/**
	 * @brief Set data
	 * @param type				Texture type
	 * @param numLayers			Layers number
	 * @param mipmaps			Texture mipmaps
	 * @param data				Texture data
	 * @param pixelFormat		Pixel format
	 */
	FORCEINLINE void SetData( studioAPITextureType_t type, uint32 numLayers, stexTextureMipMaps_t& mipmaps, std::vector<byte>& data, studioAPIPixelFormat_t pixelFormat )
	{
		Validate( type, numLayers, mipmaps );
		CSTEXCompiledTextureDoc::type			= type;
		CSTEXCompiledTextureDoc::numLayers		= numLayers;
		CSTEXCompiledTextureDoc::mipmaps		= std::move( mipmaps );
		CSTEXCompiledTextureDoc::data			= std::move( data );
		CSTEXCompiledTextureDoc::pixelFormat	= pixelFormat;
	}

	/**
	 * @brief Get pixel format
	 * @return Return pixel format
	 */
	FORCEINLINE studioAPIPixelFormat_t GetPixelFormat() const
	{
		return pixelFormat;
	}

	/**
	 * @brief Set address mode for U coord
	 * @param addressModeU		Address mode for U coord
	 */
	FORCEINLINE void SetAddressModeU( studioAPISamplerAddressMode_t addressModeU )
	{
		CSTEXCompiledTextureDoc::addressModeU = addressModeU;
	}

	/**
	 * @brief Get address mode for U coord
	 * @return Return address mode for U coord
	 */
	FORCEINLINE studioAPISamplerAddressMode_t GetAddressModeU() const
	{
		return addressModeU;
	}

	/**
	 * @brief Set address mode for V coord
	 * @param addressModeV		Address mode for V coord
	 */
	FORCEINLINE void SetAddressModeV( studioAPISamplerAddressMode_t addressModeV )
	{
		CSTEXCompiledTextureDoc::addressModeV = addressModeV;
	}

	/**
	 * @brief Get address mode for V coord
	 * @return Return address mode for V coord
	 */
	FORCEINLINE studioAPISamplerAddressMode_t GetAddressModeV() const
	{
		return addressModeV;
	}

	/**
	 * @brief Set address mode for W coord
	 * @param addressModeW		Address mode for W coord
	 */
	FORCEINLINE void SetAddressModeW( studioAPISamplerAddressMode_t addressModeW )
	{
		CSTEXCompiledTextureDoc::addressModeW = addressModeW;
	}

	/**
	 * @brief Get address mode for W coord
	 * @return Return address mode for W coord
	 */
	FORCEINLINE studioAPISamplerAddressMode_t GetAddressModeW() const
	{
		return addressModeW;
	}

	/**
	 * @brief Set texture filter
	 * @param filter	Texture filter
	 */
	FORCEINLINE void SetFilter( studioAPISamplerFilter_t filter )
	{
		CSTEXCompiledTextureDoc::filter = filter;
	}

	/**
	 * @brief Get texture filter
	 * @return Return texture filter	
	 */
	FORCEINLINE studioAPISamplerFilter_t GetFilter() const
	{
		return filter;
	}

	/**
	 * @brief Set maximum anisotropy value clamp
	 * @param maxAnisotropy		Anisotropy value clamp
	 */
	FORCEINLINE void SetMaxAnisotropy( uint32 maxAnisotropy )
	{
		CSTEXCompiledTextureDoc::maxAnisotropy = maxAnisotropy;
	}

	/**
	 * @brief Get maximum anisotropy value clamp
	 * @return Return maximum anisotropy value clamp
	 */
	FORCEINLINE uint32 GetMaxAnisotropy() const
	{
		return maxAnisotropy;
	}

	/**
	 * @brief Get layers number
	 * @return Return layers number
	 */
	FORCEINLINE uint32 GetNumLayers() const
	{
		return numLayers;
	}

	/**
	 * @brief Get mipmaps number
	 * @return Return mipmaps number
	 */
	FORCEINLINE uint32 GetNumMips() const
	{
		return ( uint32 )mipmaps.size();
	}

	/**
	 * @brief Get width of a mip level
	 * @param mipLevel	Mip level
	 * @return Return width of the mip level
	 */
	FORCEINLINE uint32 GetSizeX( uint32 mipLevel = 0 ) const
	{
		Assert( mipLevel < mipmaps.size() );
		return mipmaps[mipLevel].sizeX;
	}

	/**
	 * @brief Get height of a mip level
	 * @param mipLevel	Mip level
	 * @return Return height of the mip level
	 */
	FORCEINLINE uint32 GetSizeY( uint32 mipLevel = 0 ) const
	{
		Assert( mipLevel < mipmaps.size() );
		return mipmaps[mipLevel].sizeY;
	}

	/**
	 * @brief Get depth of a mip level
	 * @param mipLevel	Mip level
	 * @return Return depth of the mip level
	 */
	FORCEINLINE uint32 GetSizeZ( uint32 mipLevel = 0 ) const
	{
		Assert( mipLevel < mipmaps.size() );
		return mipmaps[mipLevel].sizeZ;
	}

	/**
	 * @brief Get a mip level information
	 * @param mipLevel	Mip level
	 * @return Return the mip level information
	 */
	FORCEINLINE const stexTextureMipMap_t& GetMip( uint32 mipLevel ) const
	{
		Assert( mipLevel < mipmaps.size() );
		return mipmaps[mipLevel];
	}

	/**
	 * @brief Get texture mipmaps
	 * @return Return texture mipmaps
	 */
	FORCEINLINE const stexTextureMipMaps_t& GetMipmaps() const
	{
		return mipmaps;
	}

	/**
	 * @brief Get texture data
	 * @return Return texture data
	 */
	FORCEINLINE const std::vector<byte>& GetData() const
	{
		return data;
	}

	/**
	 * @brief Get texture type
	 * @return Return texture type
	 */
	FORCEINLINE studioAPITextureType_t GetType() const
	{
		return type;
	}

	/**
	 * @brief Clear the compiled texture
	 */
	FORCEINLINE void Clear()
	{
		type				= STUDIOAPI_TEXTURE_TYPE_1D;
		pixelFormat			= STUDIOAPI_PIXEL_FORMAT_UNKNOWN;
		numLayers			= 0;
		maxAnisotropy		= 0;
		addressModeU		= STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
		addressModeV		= STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
		addressModeW		= STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
		filter				= STUDIOAPI_SAMPLER_FILTER_POINT;
		mipmaps.clear();
		data.clear();
	}

private:
	/**
	 * @brief Validate parameters (only for debug)
	 * @param type			Texture type
	 * @param numLayers		Layers number
	 * @param mipmaps		Texture mipmaps
	 */
	void Validate( studioAPITextureType_t type, uint32 numLayers, const stexTextureMipMaps_t& mipmaps ) const;

	studioAPITextureType_t			type;				/**< Texture type */
	studioAPIPixelFormat_t			pixelFormat;		/**< Pixel format */
	uint32							numLayers;			/**< Layers number */
	uint32							maxAnisotropy;		/**< Anisotropy value clamp */
	studioAPISamplerAddressMode_t	addressModeU;		/**< Address mode for U coord */
	studioAPISamplerAddressMode_t	addressModeV;		/**< Address mode for V coord */
	studioAPISamplerAddressMode_t	addressModeW;		/**< Address mode for W coord */
	studioAPISamplerFilter_t		filter;				/**< Sampler filter */
	stexTextureMipMaps_t			mipmaps;			/**< Texture mipmaps */
	std::vector<byte>				data;				/**< Texture data */
};

#endif // !STEX_COMPILED_DOC_H