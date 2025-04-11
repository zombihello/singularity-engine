/**
 * @file
 * @addtogroup materialsystem materialsystem
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "materialsystem/itexture.h"
#include "studiorender/istudio_renderresource.h"

/**
 * @ingroup materialsystem
 * @brief Texture
 */
class CTexture : public TRefCounted<ITexture>, public TStudioRenderResource<IStudioRenderResource>
{
public:
	/**
	 * @brief Constructor
	 */
	CTexture();

	/**
	 * @brief Clear the texture
	 */
	void Clear();

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
	virtual void SetData( studioAPITextureType_t type, studioAPIPixelFormat_t pixelFormat, uint32 numLayers, const textureMipMap_t* pMipmaps, uint32 numMipmaps, const studioAPISamplerCreateInfo_t& samplerInfo, const byte* pData = NULL, uint32 dataSize = 0 ) override;

	/**
	 * @brief Get pixel format
	 * @return Return pixel format
	 */
	virtual studioAPIPixelFormat_t GetPixelFormat() const override;

	/**
	 * @brief Get layers number
	 * @return Return layers number
	 */
	virtual uint32 GetNumLayers() const override;

	/**
	 * @brief Get mipmaps number
	 * @return Return mipmaps number
	 */
	virtual uint32 GetNumMips() const override;

	/**
	 * @brief Get a mip level information
	 * @param mipLevel	Mip level
	 * @return Return the mip level information
	 */
	virtual const textureMipMap_t& GetMip( uint32 mipLevel ) const override;

	/**
	 * @brief Get texture type
	 * @return Return texture type
	 */
	virtual studioAPITextureType_t GetType() const override;

	/**
	 * @brief Get StudioAPI texture
	 * @return Return StudioAPI texture. If the texture not loaded on GPU return NULL
	 */
	virtual IStudioAPITexture* GetStudioAPITexture() const override;

	/**
	 * @brief Get StudioAPI sampler
	 * @return Return StudioAPI sampler. If the texture not loaded on GPU return NULL
	 */
	virtual IStudioAPISampler* GetStudioAPISampler() const override;

private:
	/**
	 * @brief Initializes the StudioAPI resources used by this resource
	 * @warning This is only called by the render thread
	 *
	 * Called when the resource is initialized
	 */
	virtual void InitStudioAPI() override;

	/**
	 * @brief Releases the StudioAPI resources used by this resource
	 * @warning This is only called by the render thread
	 *
	 * Called when the resource is released
	 */
	virtual void ReleaseStudioAPI() override;

	studioAPITextureType_t			type;						/**< Texture type */
	studioAPIPixelFormat_t			pixelFormat;				/**< Pixel format */
	studioAPISamplerCreateInfo_t	studioAPISamplerCreateInfo;	/**< StudioAPI sampler create information */
	uint32							numLayers;					/**< Layers number in the texture */
	TRefPtr<IStudioAPITexture>		pStudioAPITexture;			/**< StudioAPI texture */
	TRefPtr<IStudioAPISampler>		pStudioAPISampler;			/**< StudioAPI sampler */
	std::vector<textureMipMap_t>	mipmaps;					/**< Mipmaps information */
	std::vector<byte>				data;						/**< Texture data */
};

#endif // !TEXTURE_H