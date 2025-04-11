/**
 * @file
 * @addtogroup materialsystem materialsystem
 */

#ifndef ITEXTURE_H
#define ITEXTURE_H

#include "stdlib/refcount.h"
#include "studiorender/studioapi/istudioapi_texture.h"

/**
 * @ingroup materialsystem
 * @brief Texture mipmap
 */
struct textureMipMap_t
{
	uint32		sizeX;	/**< Mipmap width */
	uint32		sizeY;	/**< Mipmap height */
	uint32		sizeZ;	/**< Mipmap depth */
};

/**
 * @ingroup materialsystem
 * @brief Texture interface
 */
class ITexture : public IRefCounted
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~ITexture() {}

	/**
	 * @brief Set a texture data
	 * @param type			Texture type
	 * @param pixelFormat	Pixel format
	 * @param numLayers		Layers number
	 * @param pMipmaps		Texture mipmaps
	 * @param numMipmaps	Texture mipmaps count
	 * @param samplerInfo	Sampler information
	 * @param pData			Texture data
	 * @param dataSize		Texture data size
	 */
	virtual void SetData( studioAPITextureType_t type, studioAPIPixelFormat_t pixelFormat, uint32 numLayers, const textureMipMap_t* pMipmaps, uint32 numMipmaps, const studioAPISamplerCreateInfo_t& samplerInfo, const byte* pData = NULL, uint32 dataSize = 0 ) = 0;

	/**
	 * @brief Get pixel format
	 * @return Return pixel format
	 */
	virtual studioAPIPixelFormat_t GetPixelFormat() const = 0;

	/**
	 * @brief Get layers number
	 * @return Return layers number
	 */
	virtual uint32 GetNumLayers() const = 0;

	/**
	 * @brief Get mipmaps number
	 * @return Return mipmaps number
	 */
	virtual uint32 GetNumMips() const = 0;

	/**
	 * @brief Get a mip level information
	 * @param mipLevel	Mip level
	 * @return Return the mip level information
	 */
	virtual const textureMipMap_t& GetMip( uint32 mipLevel ) const = 0;

	/**
	 * @brief Get texture type
	 * @return Return texture type
	 */
	virtual studioAPITextureType_t GetType() const = 0;

	/**
	 * @brief Get StudioAPI texture
	 * @return Return StudioAPI texture. If the texture not loaded on GPU return NULL
	 */
	virtual IStudioAPITexture* GetStudioAPITexture() const = 0;

	/**
	 * @brief Get StudioAPI sampler
	 * @return Return StudioAPI sampler. If the texture not loaded on GPU return NULL
	 */
	virtual IStudioAPISampler* GetStudioAPISampler() const = 0;
};

#endif // !ITEXTURE_H