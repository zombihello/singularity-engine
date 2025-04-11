/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_TEXTURE_H
#define VK_STUDIOAPI_TEXTURE_H

#include "pixelformatinfos/pixelformatinfos.h"
#include "studiorender/studioapi/istudioapi_texture.h"

// Forward declarations
class CStudioAPICmdListVk;

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI image subresource range
 */
class CStudioAPIImageSubresourceRangeVk
{
public:
	/**
	 * @brief Constructor
	 */
	CStudioAPIImageSubresourceRangeVk()
	{}

	/**
	 * @brief Create a range for all subresources
	 * @return Return created subresource range
	 */
	static FORCEINLINE CStudioAPIImageSubresourceRangeVk CreateForAllSubresources()
	{
		return CStudioAPIImageSubresourceRangeVk( 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS );
	}

	/**
	 * @brief Create a range for specific subresources
	 * @param startMip		Start mip level
	 * @param numMips		Mips number
	 * @param startLayer	Start layer
	 * @param numLayers		Layers number
	 * @return Return created subresource range
	 */
	static FORCEINLINE CStudioAPIImageSubresourceRangeVk CreateForSubresources( uint32 startMip, uint32 numMips = 1, uint32 startLayer = 0, uint32 numLayers = 1 )
	{
		return CStudioAPIImageSubresourceRangeVk( startMip, numMips, startLayer, numLayers );
	}

	/**
	 * @brief Is the range for all subresources
	 * @return Return TRUE if the range for all subresources, otherwise FALSE
	 */
	FORCEINLINE bool IsAllSubresources() const
	{
		return numMips == VK_REMAINING_MIP_LEVELS && numLayers == VK_REMAINING_ARRAY_LAYERS;
	}

	/**
	 * @brief Get start mip level
	 * @return Return start mip level
	 */
	FORCEINLINE uint32 GetStartMip() const
	{
		return startMip;
	}

	/**
	 * @brief Get start layer
	 * @return Return start layer
	 */
	FORCEINLINE uint32 GetStartLayer() const
	{
		return startLayer;
	}

	/**
	 * @brief Get mips number
	 * @warning You can't use it if the range for all subresources
	 * @return Return mips number
	 */
	FORCEINLINE uint32 GetNumMips() const
	{
		Assert( !IsAllSubresources() );
		return numMips;
	}

	/**
	 * @brief Get layers number
	 * @warning You can't use it if the range for all subresources
	 * @return Return layers number
	 */
	FORCEINLINE uint32 GetNumLayers() const
	{
		Assert( !IsAllSubresources() );
		return numLayers;
	}

	/**
	 * @brief Convert the range into a Vulkan structure
	 * @param vkImageAspectFlags	Image aspect flags
	 * @return Return converted the range into a Vulkan structure
	 */
	FORCEINLINE VkImageSubresourceRange ToVkStructure( VkImageAspectFlags vkImageAspectFlags ) const
	{
		VkImageSubresourceRange				vkImageSubresourceRange = {};
		vkImageSubresourceRange.aspectMask	= vkImageAspectFlags;
		if ( IsAllSubresources() )
		{
			vkImageSubresourceRange.baseMipLevel	= 0;
			vkImageSubresourceRange.levelCount		= VK_REMAINING_MIP_LEVELS;
			vkImageSubresourceRange.baseArrayLayer	= 0;
			vkImageSubresourceRange.layerCount		= VK_REMAINING_ARRAY_LAYERS;
		}
		else
		{
			vkImageSubresourceRange.baseMipLevel	= startMip;
			vkImageSubresourceRange.levelCount		= numMips;
			vkImageSubresourceRange.baseArrayLayer	= startLayer;
			vkImageSubresourceRange.layerCount		= numLayers;
		}
		return vkImageSubresourceRange;
	}

private:
	/**
	 * @brief Constructor
	 * @param startMip		Start mip level
	 * @param numMips		Mips number
	 * @param startLayer	Start layer
	 * @param numLayers		Layers number
	 */
	CStudioAPIImageSubresourceRangeVk( uint32 startMip, uint32 numMips, uint32 startLayer, uint32 numLayers )
		: startMip( startMip )
		, numMips( numMips )
		, startLayer( startLayer )
		, numLayers( numLayers )
	{}

	uint32		startMip;		/**< Start mip level */
	uint32		numMips;		/**< Mips number */
	uint32		startLayer;		/**< Start layer */
	uint32		numLayers;		/**< Layers number */
};

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI sampler
 */
class CStudioAPISamplerVk : public TRefCounted<IStudioAPISampler>
{
public:
	/*
	 * @brief Constructor
	 * @param createInfo	Information to create a sampler
	 * @param pDebugName	Debug name
	 */
	CStudioAPISamplerVk( const studioAPISamplerCreateInfo_t& createInfo, const achar* pDebugName = "" );

	/**
	 * @brief Destructor
	 */
	~CStudioAPISamplerVk();

	/**
	 * @brief Get Vulkan sampler
	 * @return Return Vulkan sampler
	 */
	FORCEINLINE VkSampler GetVkSampler() const
	{
		return vkSampler;
	}

private:
	/**
	 * @brief Delegate called when StudioAPI Vulkan is shutdown
	 * @param pUserData		Pointer to CStudioAPISamplerVk
	 */
	static void OnStudioAPIVkShutdown( void* pUserData );

	VkSampler									vkSampler;						/**< Vulkan sampler */
	COnStudioAPIVkShutdown::funcDelegate_t*		pStudioAPIVkShutdownDelegate;	/**< Delegate handle 'on StudioAPI Vulkan shutdown' */
};

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI texture
 */
class CStudioAPITextureVk : public TRefCounted<IStudioAPITexture>
{
public:
	/**
	 * @brief Constructor
	 * @param type			Texture type
	 * @param sizeX			Texture width
	 * @param sizeY			Texture height
	 * @param sizeZ			Texture depth
	 * @param numLayers		Layers number in the texture
	 * @param numMips		Mips number in the texture
	 * @param usageFlags	Usage flags (see studioAPITextureUsageFlag_t)
	 * @param pixelFormat	Texture pixel format
	 * @param pData			Texture data
	 * @param pDebugName	Debug name
	 */
	CStudioAPITextureVk( studioAPITextureType_t type, uint32 sizeX, uint32 sizeY, uint32 sizeZ, uint32 numLayers, uint32 numMips, uint32 usageFlags, studioAPIPixelFormat_t pixelFormat, const byte* pData = NULL, const achar* pDebugName = "" );

	/**
	 * @brief Destructor
	 */
	~CStudioAPITextureVk();

	/**
	 * @brief Update a data in the texture
	 * @warning For use the texture must have STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_DST flag
	 *
	 * @param pCmdContext	Command context
	 * @param pData			A new texture data
	 * @param startLayer	Start layer index
	 * @param startMip		Start mip index
	 */
	virtual void UpdateData( IStudioAPICmdContext* pCmdContext, byte* pData, uint32 startLayer = 0, uint32 startMip = 0 ) override;

	/**
	 * @brief Map memory of the texture data
	 * @warning Before use check memory flags by GetMemoryFlags(). If STUDIOAPI_TEXTURE_MEMORY_FLAG_CPU is set then you can use the function,
	 * otherwise use IStudioAPICmdList::CopyBufferToTexture, IStudioAPICmdList::CopyTextureToBuffer or IStudioAPICmdList::CopyTexture
	 *
	 * @param layer			Layer index
	 * @param mip			Mip level
	 * @param mappedData	Output mapped data
	 */
	virtual void MapMemory( uint32 layer, uint32 mip, studioAPIMappedTextureData_t& mappedData ) override;

	/**
	 * @brief Unmap memory of the texture data
	 * @warning Before use check memory flags by GetMemoryFlags(). If STUDIOAPI_TEXTURE_MEMORY_FLAG_CPU is set then you can use the function,
	 * otherwise use IStudioAPICmdList::CopyBufferToTexture, IStudioAPICmdList::CopyTextureToBuffer or IStudioAPICmdList::CopyTexture
	 *
	 * @param mappedData	Mapped data to unmap
	 */
	virtual void UnmapMemory( studioAPIMappedTextureData_t& mappedData ) override;

	/**
	 * @brief Get texture width
	 * @return Return texture width
	 */
	virtual uint32 GetSizeX() const override;

	/**
	 * @brief Get texture height
	 * @return Return texture height
	 */
	virtual uint32 GetSizeY() const override;

	/**
	 * @brief Get texture depth
	 * @return Return texture depth
	 */
	virtual uint32 GetSizeZ() const override;

	/**
	 * @brief Get mip levels number in the texture
	 * @return Return mip levels number in the texture
	 */
	virtual uint32 GetNumMips() const override;

	/**
	 * @brief Get layers number in the texture
	 * @return Return layers number in the texture
	 */
	virtual uint32 GetNumLayers() const override;

	/**
	 * @brief Get pixel format of the texture
	 * @return Return pixel format of the texture
	 */
	virtual studioAPIPixelFormat_t GetPixelFormat() const override;

	/**
	 * @brief Get texture type
	 * @return Return texture type
	 */
	virtual studioAPITextureType_t GetType() const override;

	/**
	 * @brief Get usage flags
	 * @return Return usage flags (see studioAPITextureUsageFlag_t)
	 */
	virtual uint32 GetUsageFlags() const override;

	/**
	 * @brief Get texture memory flags
	 * @return Return texture memory flags (see studioAPITextureMemoryFlag_t)
	 */
	virtual uint8 GetMemoryFlags() const override;

	/**
	 * @brief Update a synchronization state of the texture
	 * The function update a synchronization state of the texture. Useful for
	 * for implicit transfer of ownership without barrier, change an image layout 
	 * and update access with stage masks
	 *
	 * @param subresourceRange		Subresource range
	 * @param vkDstImageLayout		Destination image layout
	 * @param vkDstAccessMask		Destination Vulkan access mask
	 * @param vkDstStageMask		Destination Vulkan pipeline stage mask
	 * @param dstQueueFamilyIndex	Destination queue family which will be owner of the resource
	 */
	void UpdateSyncState( const CStudioAPIImageSubresourceRangeVk& subresourceRange, VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex );

	/**
	 * @brief Update a synchronization state and make an image barrier
	 * The function update a synchronization state of the texture and make
	 * an image barrier if it need. Useful for transfer ownership, change
	 * an image layout and make barriers for the texture
	 *
	 * @param pCmdList					A command list which will be placed a barrier
	 * @param subresourceRange			Subresource range
	 * @param vkDstImageLayout			Destination image layout
	 * @param vkDstAccessMask			Destination Vulkan access mask
	 * @param vkDstStageMask			Destination Vulkan pipeline stage mask
	 * @param dstQueueFamilyIndex		Destination queue family which will be owner of the resource
	 */
	void UpdateSyncStateWithBarrier( CStudioAPICmdListVk* pCmdList, const CStudioAPIImageSubresourceRangeVk& subresourceRange, VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex );

	/**
	 * @brief Get Vulkan image aspect flags
	 * @return Return Vulkan image aspect flags
	 */
	FORCEINLINE VkImageAspectFlags GetVkImageAspectFlags() const
	{
		return vkImageAspectFlags;
	}

	/**
	 * @brief Get Vulkan image
	 * @return Return Vulkan image
	 */
	FORCEINLINE VkImage GetVkImage() const
	{
		return vkImage;
	}

	/**
	 * @brief Get the main Vulkan image view
	 * @return Return the main Vulkan image view 
	 */
	FORCEINLINE VkImageView GetVkImageView() const
	{
		Assert( !VK_IsDepthPixelFormat( pixelFormat ) );
		return vkImageViews[0];
	}

	/**
	 * @brief Get Vulkan image view of specific mip level and layer
	 * @note The function only for render targets
	 * 
	 * @param mip		Mip level
	 * @param layer		Layer
	 * @return Return Vulkan image view of the mip level and the layer
	 */
	FORCEINLINE VkImageView GetVkImageView( uint32 mip, uint32 layer ) const
	{
		Assert( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_RENDER_TARGET );
		Assert( mip < numMips && layer < numLayers );
		return vkImageViews[GetVkImageViewIndex( mip, layer )];
	}

	/**
	 * @brief Get Vulkan image view with depth/stencil aspect
	 * @note The function only for depth/stencil textures
	 * 
	 * @param bDepthOnly		Is depth only
	 * @param bStencilOnly		Is stencil only
	 * @return Return Vulkan image view for specific depth/stencil aspect
	 */
	FORCEINLINE VkImageView GetVkImageView( bool bDepthOnly, bool bStencilOnly ) const
	{
		Assert( VK_IsDepthPixelFormat( pixelFormat ) );
		if ( bDepthOnly )
		{
			return vkImageViews[IMAGE_VIEW_INDEX_DEPTH_ONLY];
		}
		else if ( bStencilOnly )
		{
			return vkImageViews[IMAGE_VIEW_INDEX_STENCIL_ONLY];
		}
		return vkImageViews[IMAGE_VIEW_INDEX_DEPTH_AND_STENCIL];
	}

	/**
	 * @brief Get synchronization state of specific mip level and layer
	 * @param mip		Mip level
	 * @param layer		Layer
	 * @return Return synchronization state of specific mip level and layer
	 */
	FORCEINLINE const studioAPISyncStateImageVk_t& GetSyncState( uint32 mip = 0, uint32 layer = 0 ) const
	{
		return syncStates[GetSyncStateIndex( mip, layer )];
	}

private:
	enum
	{
		IMAGE_VIEW_INDEX_DEPTH_ONLY,			/**< Depth only Vulkan image view index */
		IMAGE_VIEW_INDEX_STENCIL_ONLY,			/**< Stencil only Vulkan image view index */
		IMAGE_VIEW_INDEX_DEPTH_AND_STENCIL		/**< Depth and stencil Vulkan image view index */
	};

	/**
	 * @brief Get Vulkan image view index from a mip level and layer
	 * @param mip		Mip level
	 * @param layer		Layer
	 * @return Return calculated Vulkan image view for the mip and the layer
	 */
	FORCEINLINE uint32 GetVkImageViewIndex( uint32 mip, uint32 layer ) const
	{
		// A Vulkan image view at 0 index is main view
		return 1 + mip + numMips * layer;
	}

	/**
	 * @brief Get layer size
	 * @return Return calculated layer size
	 */
	FORCEINLINE uint32 GetLayerSize() const
	{
		return GetMipOffset( numMips );
	}

	/**
	 * @brief Get offset to a layer
	 * @param layer		Layer
	 * @return Return calculated offset to the layer
	 */
	FORCEINLINE uint32 GetLayerOffset( uint32 layer ) const
	{
		return layer * GetLayerSize();
	}

	/**
	 * @brief Get size in bytes for a mip level
	 * @param mip	Mip level
	 * @return Return calculated size in bytes for the mip level
	 */
	FORCEINLINE uint32 GetMipSize( uint32 mip ) const
	{
		pixelFormatInfo_t*			pPixelFormatInfo = &g_PixelFormatInfos[pixelFormat];
		uint32 blockSizeX			= pPixelFormatInfo->blockSizeX;
		uint32 blockSizeY			= pPixelFormatInfo->blockSizeY;
		uint32 blockSizeZ			= pPixelFormatInfo->blockSizeZ;
		uint32 blockBytes			= pPixelFormatInfo->blockBytes;
		uint32 mipSizeX				= Max( sizeX >> mip, blockSizeX );
		uint32 mipSizeY				= Max( sizeY >> mip, blockSizeY );
		uint32 mipSizeZ				= Max( sizeZ >> mip, blockSizeZ );
		uint32 numBlocksX			= ( mipSizeX + blockSizeX - 1 ) / blockSizeX;
		uint32 numBlocksY			= ( mipSizeY + blockSizeY - 1 ) / blockSizeY;
		uint32 numBlocksZ			= ( mipSizeZ + blockSizeZ - 1 ) / blockSizeZ;
		return numBlocksX * numBlocksY * numBlocksZ * blockBytes;
	}

	/**
	 * @brief Get row pitch in bytes for a mip level
	 * @param mip	Mip level
	 * @return Return calculated row pitch in bytes for the mip level
	 */
	FORCEINLINE uint32 GetMipRowPitch( uint32 mip ) const
	{
		pixelFormatInfo_t*	pPixelFormatInfo = &g_PixelFormatInfos[pixelFormat];
		uint32 blockSizeX	= pPixelFormatInfo->blockSizeX;
		uint32 blockBytes	= pPixelFormatInfo->blockBytes;
		uint32 mipSizeX		= Max( sizeX >> mip, blockSizeX );
		uint32 numBlocksX	= ( mipSizeX + blockSizeX - 1 ) / blockSizeX;
		return numBlocksX * blockBytes;
	}

	/**
	 * @brief Get depth pitch in bytes for a mip level
	 * @param mip	Mip level
	 * @return Return calculated depth pitch in bytes for the mip level
	 */
	FORCEINLINE uint32 GetMipDepthPitch( uint32 mip ) const
	{
		pixelFormatInfo_t*	pPixelFormatInfo = &g_PixelFormatInfos[pixelFormat];
		uint32 blockSizeX	= pPixelFormatInfo->blockSizeX;
		uint32 blockSizeY	= pPixelFormatInfo->blockSizeY;
		uint32 blockBytes	= pPixelFormatInfo->blockBytes;
		uint32 mipSizeX		= Max( sizeX >> mip, blockSizeX );
		uint32 mipSizeY		= Max( sizeY >> mip, blockSizeY );
		uint32 numBlocksX	= ( mipSizeX + blockSizeX - 1 ) / blockSizeX;
		uint32 numBlocksY	= ( mipSizeY + blockSizeY - 1 ) / blockSizeY;
		return numBlocksX * numBlocksY * blockBytes;
	}

	/**
	 * @brief Get offset to a mip level
	 * @param mip	Mip level
	 * @return Return calculated offset to the mip level
	 */
	FORCEINLINE uint32 GetMipOffset( uint32 mip ) const
	{
		uint32	offset = 0;
		for ( uint32 mipIdx = 0; mipIdx < mip; ++mipIdx )
		{
			offset += GetMipSize( mipIdx );
		}
		return offset;
	}

	/**
	 * @brief Get synchronization state index for specific mip level and layer
	 * @param mip		Mip level
	 * @param layer		Layer
	 * @return Return synchronization state index for specific mip level and layer
	 */
	FORCEINLINE uint32 GetSyncStateIndex( uint32 mip, uint32 layer ) const
	{
		Assert( mip + numMips * layer < ( uint32 )syncStates.size() );
		return mip + numMips * layer;
	}

	/**
	 * @brief Delegate called when StudioAPI Vulkan is shutdown
	 * @param pUserData		Pointer to CStudioAPITextureVk
	 */
	static void OnStudioAPIVkShutdown( void* pUserData );

	uint32										usageFlags;						/**< Usage flags (see studioAPITextureUsageFlag_t) */
	uint8										memoryFlags;					/**< Memory flags (see studioAPITextureMemoryFlag_t) */
	studioAPITextureType_t						type;							/**< Texture type */
	studioAPIPixelFormat_t						pixelFormat;					/**< Pixel format */
	VkImageAspectFlags							vkImageAspectFlags;				/**< Vulkan image aspect flags */
	uint32										sizeX;							/**< Texture width */
	uint32										sizeY;							/**< Texture height */
	uint32										sizeZ;							/**< Texture depth */
	uint32										numLayers;						/**< Layers number in the texture */
	uint32										numMips;						/**< Mips number in the texture */
	VkImage										vkImage;						/**< Vulkan image */
	VmaAllocation								vmaAllocation;					/**< VMA allocation */
	COnStudioAPIVkShutdown::funcDelegate_t*		pStudioAPIVkShutdownDelegate;	/**< Delegate handle 'on StudioAPI Vulkan shutdown' */
	std::vector<studioAPISyncStateImageVk_t>	syncStates;						/**< Synchronization states for each mip level and layer */
	std::vector<VkImageView>					vkImageViews;					/**< Vulkan image views */
};

#endif // !VK_STUDIOAPI_TEXTURE_H