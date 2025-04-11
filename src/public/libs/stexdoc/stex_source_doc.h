/**
 * @file
 * @addtogroup stexdoc stexdoc
 */

#ifndef STEX_SOURCE_DOC_H
#define STEX_SOURCE_DOC_H

#include "studiorender/studioapi/istudioapi_texture.h"
#include "stexdoc/stex_types.h"

/**
 * @ingroup stexdoc
 * @brief Helper for work with source STEX (Singularity Texture) files
 */
class CSTEXSourceTextureDoc
{
public:
	/**
	 * @brief Constructor
	 */
	CSTEXSourceTextureDoc();

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
	 * @brief Set generate mip maps
	 * @param bGenerateMipMaps		Is need generate mip maps
	 */
	FORCEINLINE void SetGenerateMipMaps( bool bGenerateMipMaps )
	{
		CSTEXSourceTextureDoc::bGenerateMipMaps = bGenerateMipMaps;
	}

	/**
	 * @brief Is need generate mip maps
	 * @return Return TRUE if need generate mip maps, otherwise FALSE
	 */
	FORCEINLINE bool IsNeedGenerateMipMaps() const
	{
		return bGenerateMipMaps;
	}

	/**
	 * @brief Set address mode for U coord
	 * @param addressModeU		Address mode for U coord
	 */
	FORCEINLINE void SetAddressModeU( studioAPISamplerAddressMode_t addressModeU )
	{
		CSTEXSourceTextureDoc::addressModeU = addressModeU;
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
		CSTEXSourceTextureDoc::addressModeV = addressModeV;
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
		CSTEXSourceTextureDoc::addressModeW = addressModeW;
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
		CSTEXSourceTextureDoc::filter = filter;
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
	 * @brief Set pixel format
	 * @param pixelFormat	Pixel format
	 */
	FORCEINLINE void SetPixelFormat( studioAPIPixelFormat_t pixelFormat )
	{
		CSTEXSourceTextureDoc::pixelFormat = pixelFormat;
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
	 * @brief Set default pixel format
	 * @param pixelFormat	Pixel format
	 */
	static FORCEINLINE void SetDefaultPixelFormat( studioAPIPixelFormat_t pixelFormat )
	{
		defaultPixelFormat = pixelFormat;
	}

	/**
	 * @brief Set texture type
	 * @param type		Texture type
	 */
	FORCEINLINE void SetType( studioAPITextureType_t type )
	{
		CSTEXSourceTextureDoc::type = type;
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
	 * @brief Add a path to a source texture
	 * @param pSourcePath	Path to a source texture
	 */
	FORCEINLINE void AddSourcePath( const achar* pSourcePath )
	{
		sourcePaths.emplace_back( pSourcePath );
	}

	/**
	 * @brief Remove the path to a source texture
	 * @param sourcePathIdx		Source path index
	 */
	FORCEINLINE void RemoveSourcePath( uint32 sourcePathIdx )
	{
		Assert( sourcePathIdx < ( uint32 )sourcePaths.size() );
		sourcePaths.erase( sourcePaths.begin() + sourcePathIdx );
	}

	/**
	 * @brief Get source paths number
	 * @return Return source paths number
	 */
	FORCEINLINE uint32 GetNumSourcePaths() const
	{
		return ( uint32 )sourcePaths.size();
	}

	/**
	 * @brief Get source path by index
	 * @return sourcePathIdx	Source path index
	 * @return Return source path at index
	 */
	FORCEINLINE const achar* GetSourcePath( uint32 sourcePathIdx )
	{
		Assert( sourcePathIdx < ( uint32 )sourcePaths.size() );
		return sourcePaths[sourcePathIdx].c_str();
	}

	/**
	 * @brief Get source paths
	 * @return Return source paths
	 */
	FORCEINLINE const std::vector<std::string>& GetSourcePaths() const
	{
		return sourcePaths;
	}

	/**
	 * @brief Set maximum anisotropy value clamp
	 * @param maxAnisotropy		Anisotropy value clamp
	 */
	FORCEINLINE void SetMaxAnisotropy( uint32 maxAnisotropy )
	{
		CSTEXSourceTextureDoc::maxAnisotropy = maxAnisotropy;
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
	 * @brief Set path to an output directory
	 * @param pOutputDir	Path to an output directory
	 */
	FORCEINLINE void SetOutputDir( const achar* pOutputDir )
	{
		outputDir = pOutputDir;
	}

	/**
	 * @brief Get path to an output directory
	 * @return Return path to an output directory
	 */
	FORCEINLINE const achar* GetOutputDir() const
	{
		return outputDir.c_str();
	}

	/**
	 * @brief Clear the source texture
	 */
	FORCEINLINE void Clear()
	{
		bGenerateMipMaps	= false;
		type				= STUDIOAPI_TEXTURE_TYPE_1D;
		addressModeU		= STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
		addressModeV		= STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
		addressModeW		= STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
		filter				= STUDIOAPI_SAMPLER_FILTER_POINT;
		pixelFormat			= STUDIOAPI_PIXEL_FORMAT_UNKNOWN;
		maxAnisotropy		= 0;
		outputDir			= "";
		sourcePaths.clear();
	}

private:
	/**
	 * @brief Is valid source paths number for a texture type
	 * @param type				Texture type
	 * @param numSourcePaths	Source paths number
	 * @param validateMsg		Output validate message
	 * @return Return TRUE if source paths number is valid for the texture type, otherwise FALSE
	 */
	FORCEINLINE bool IsValidNumSourcePaths( studioAPITextureType_t type, uint32 numSourcePaths, std::string& validateMsg ) const
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

	bool								bGenerateMipMaps;	/**< Is need generate mipmaps */
	studioAPITextureType_t				type;				/**< Texture type */
	studioAPISamplerAddressMode_t		addressModeU;		/**< Address mode for U coord */
	studioAPISamplerAddressMode_t		addressModeV;		/**< Address mode for V coord */
	studioAPISamplerAddressMode_t		addressModeW;		/**< Address mode for W coord */
	studioAPISamplerFilter_t			filter;				/**< Sampler filter */
	studioAPIPixelFormat_t				pixelFormat;		/**< Pixel format */
	uint32								maxAnisotropy;		/**< Anisotropy value clamp */
	std::string							outputDir;			/**< Path to an output directory */
	std::vector<std::string>			sourcePaths;		/**< Paths to source textures */
	static studioAPIPixelFormat_t		defaultPixelFormat;	/**< Default pixel format */
};

#endif // !STEX_SOURCE_DOC_H