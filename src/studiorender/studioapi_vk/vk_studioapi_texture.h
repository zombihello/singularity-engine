#pragma once
#include "utils/pixelformatinfos/pixelformatinfos.h"
#include "studiorender/studioapi/istudioapi_texture.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPICmdListVk;

//-----------------------------------------------------------------------------
// Vulkan StudioAPI image subresource range
//-----------------------------------------------------------------------------
class CStudioAPIImageSubresourceRangeVk
{
public:
	CStudioAPIImageSubresourceRangeVk()
	{
	}

	static CStudioAPIImageSubresourceRangeVk CreateForAllSubresources();
	static CStudioAPIImageSubresourceRangeVk CreateForSubresources( uint32 startMip, uint32 numMips = 1, uint32 startLayer = 0, uint32 numLayers = 1 );

	bool					IsAllSubresources() const;
	VkImageSubresourceRange AsVkStructure( VkImageAspectFlags vkImageAspectFlags ) const;
	uint32					GetStartMip() const;
	uint32					GetStartLayer() const;
	uint32					GetNumMips() const;
	uint32					GetNumLayers() const;

private:
	CStudioAPIImageSubresourceRangeVk( uint32 startMip, uint32 numMips, uint32 startLayer, uint32 numLayers )
		: startMip( startMip )
		, numMips( numMips )
		, startLayer( startLayer )
		, numLayers( numLayers )
	{
	}

	uint32 startMip;
	uint32 numMips;
	uint32 startLayer;
	uint32 numLayers;
};

//-----------------------------------------------------------------------------
// Vulkan StudioAPI sampler
//-----------------------------------------------------------------------------
class CStudioAPISamplerVk : public CRefCounted<IStudioAPISampler>
{
public:
	CStudioAPISamplerVk( const studioAPISamplerCreateInfo_t& createInfo, const char* pDebugName = "" );
	~CStudioAPISamplerVk();

	VkSampler GetVkSampler() const;

private:
	static void OnStudioAPIVkShutdown( void* pUserData );

	VkSampler								vkSampler;
	COnStudioAPIVkShutdown::funcDelegate_t* pStudioAPIVkShutdownDelegate;
};

//-----------------------------------------------------------------------------
// Vulkan StudioAPI texture
//-----------------------------------------------------------------------------
class CStudioAPITextureVk : public CRefCounted<IStudioAPITexture>
{
public:
	// IStudioAPITexture interface
	// For use the texture must have STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_DST flag
	virtual void UpdateData( IStudioAPICmdContext* pCmdContext, byte* pData, uint32 startLayer = 0, uint32 startMip = 0 ) override;

	// NOTE: Before use check memory flags by GetMemoryFlags(). If STUDIOAPI_TEXTURE_MEMORY_FLAG_CPU is set then you can use the function,
	// otherwise use IStudioAPICmdList::CopyBufferToTexture, IStudioAPICmdList::CopyTextureToBuffer or IStudioAPICmdList::CopyTexture
	virtual void MapMemory( uint32 layer, uint32 mip, studioAPIMappedTextureData_t& mappedData ) override;
	virtual void UnmapMemory( studioAPIMappedTextureData_t& mappedData ) override;

	virtual uint32				   GetSizeX() const override;
	virtual uint32				   GetSizeY() const override;
	virtual uint32				   GetSizeZ() const override;
	virtual uint32				   GetNumMips() const override;
	virtual uint32				   GetNumLayers() const override;
	virtual studioAPIPixelFormat_t GetPixelFormat() const override;
	virtual studioAPITextureType_t GetType() const override;
	virtual uint32				   GetUsageFlags() const override;
	virtual uint8				   GetMemoryFlags() const override;

	CStudioAPITextureVk( studioAPITextureType_t type, uint32 sizeX, uint32 sizeY, uint32 sizeZ, uint32 numLayers, uint32 numMips, uint32 usageFlags, studioAPIPixelFormat_t pixelFormat, const byte* pData = NULL, const char* pDebugName = "" );
	~CStudioAPITextureVk();

	// The function update a synchronization state of the texture. Useful for for implicit transfer of ownership without barrier, change an image layout
	// and update access with stage masks
	void UpdateSyncState( const CStudioAPIImageSubresourceRangeVk& subresourceRange, VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex );

	// The function update a synchronization state of the texture and make an image barrier if it need. Useful for transfer ownership, change
	// an image layout and make barriers for the texture
	void UpdateSyncStateWithBarrier( CStudioAPICmdListVk* pCmdList, const CStudioAPIImageSubresourceRangeVk& subresourceRange, VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex );

	VkImageAspectFlags				   GetVkImageAspectFlags() const;
	VkImage							   GetVkImage() const;
	VkImageView						   GetVkImageView() const;
	const studioAPISyncStateImageVk_t& GetSyncState( uint32 mip = 0, uint32 layer = 0 ) const;

	// NOTE: The function only for render targets
	VkImageView GetVkImageView( uint32 mip, uint32 layer ) const;

	// The function only for depth/stencil textures
	VkImageView GetVkImageView( bool bDepthOnly, bool bStencilOnly ) const;

private:
	enum
	{
		IMAGE_VIEW_INDEX_DEPTH_ONLY,
		IMAGE_VIEW_INDEX_STENCIL_ONLY,
		IMAGE_VIEW_INDEX_DEPTH_AND_STENCIL
	};

	uint32		GetVkImageViewIndex( uint32 mip, uint32 layer ) const;
	uint32		GetLayerSize() const;
	uint32		GetLayerOffset( uint32 layer ) const;
	uint32		GetMipSize( uint32 mip ) const;
	uint32		GetMipRowPitch( uint32 mip ) const;
	uint32		GetMipDepthPitch( uint32 mip ) const;
	uint32		GetMipOffset( uint32 mip ) const;
	uint32		GetSyncStateIndex( uint32 mip, uint32 layer ) const;
	static void OnStudioAPIVkShutdown( void* pUserData );

	uint32									   usageFlags;
	uint8									   memoryFlags;
	studioAPITextureType_t					   type;
	studioAPIPixelFormat_t					   pixelFormat;
	VkImageAspectFlags						   vkImageAspectFlags;
	uint32									   sizeX;
	uint32									   sizeY;
	uint32									   sizeZ;
	uint32									   numLayers;
	uint32									   numMips;
	VkImage									   vkImage;
	VmaAllocation							   vmaAllocation;
	COnStudioAPIVkShutdown::funcDelegate_t*	   pStudioAPIVkShutdownDelegate;
	eastl::vector<studioAPISyncStateImageVk_t> syncStates;
	eastl::vector<VkImageView>				   vkImageViews;
};

#include "studiorender/studioapi_vk/vk_studioapi_texture.inl"
