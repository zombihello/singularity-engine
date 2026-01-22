#pragma once

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPIFenceVk;
class CStudioAPISemaphoreVk;
class CStudioAPINamedSemaphoreVk;

//-----------------------------------------------------------------------------
// Vulkan barriers
//-----------------------------------------------------------------------------
struct studioAPIBufferMemoryBarrierVk_t
{
	VkBufferMemoryBarrier vkBufferMemoryBarrier;
	VkPipelineStageFlags  vkDstStageMask;
	VkPipelineStageFlags  vkSrcStageMask;
};

struct studioAPIImageMemoryBarrierVk_t
{
	VkImageMemoryBarrier vkImageMemoryBarrier;
	VkPipelineStageFlags vkDstStageMask;
	VkPipelineStageFlags vkSrcStageMask;
};

//-----------------------------------------------------------------------------
// Vulkan StudioAPI synchronization scope
//-----------------------------------------------------------------------------
struct studioAPISyncScopeVk_t
{
	studioAPISyncScopeVk_t()
		: vkAccessFlags( VK_ACCESS_NONE )
		, vkPipelineStages( VK_PIPELINE_STAGE_NONE )
	{
	}
	studioAPISyncScopeVk_t( VkAccessFlags vkAccessFlags, VkPipelineStageFlags vkPipelineStages )
		: vkAccessFlags( vkAccessFlags )
		, vkPipelineStages( vkPipelineStages )
	{
	}

	bool operator==( const studioAPISyncScopeVk_t& other ) const;
	bool operator!=( const studioAPISyncScopeVk_t& other ) const;

	VkAccessFlags		 vkAccessFlags;
	VkPipelineStageFlags vkPipelineStages;
};

//-----------------------------------------------------------------------------
// Vulkan StudioAPI buffer synchronization state
//-----------------------------------------------------------------------------
struct studioAPISyncStateBufferVk_t
{
public:
	studioAPISyncStateBufferVk_t()
		: bHasPendingOwnershipTransfer( false )
		, ownerQueueFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
	{
	}
	studioAPISyncStateBufferVk_t( const studioAPISyncScopeVk_t& readScope, const studioAPISyncScopeVk_t& writeScope, uint32 ownerQueueFamilyIndex )
		: bHasPendingOwnershipTransfer( false )
		, readScope( readScope )
		, writeScope( writeScope )
		, ownerQueueFamilyIndex( ownerQueueFamilyIndex )
	{
	}

	bool operator==( const studioAPISyncStateBufferVk_t& other ) const;
	bool operator!=( const studioAPISyncStateBufferVk_t& other ) const;

	bool				   bHasPendingOwnershipTransfer;
	studioAPISyncScopeVk_t readScope;
	studioAPISyncScopeVk_t writeScope;
	uint32				   ownerQueueFamilyIndex;
};

//-----------------------------------------------------------------------------
// Vulkan StudioAPI image synchronization state
//-----------------------------------------------------------------------------
struct studioAPISyncStateImageVk_t
{
	studioAPISyncStateImageVk_t()
		: bHasPendingOwnershipTransfer( false )
		, vkImageLayout( VK_IMAGE_LAYOUT_UNDEFINED )
		, ownerQueueFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
	{
	}
	studioAPISyncStateImageVk_t( VkImageLayout vkImageLayout, const studioAPISyncScopeVk_t& readScope, const studioAPISyncScopeVk_t& writeScope, uint32 ownerQueueFamilyIndex )
		: bHasPendingOwnershipTransfer( false )
		, vkImageLayout( vkImageLayout )
		, readScope( readScope )
		, writeScope( writeScope )
		, ownerQueueFamilyIndex( ownerQueueFamilyIndex )
	{
	}

	bool operator==( const studioAPISyncStateImageVk_t& other ) const;
	bool operator!=( const studioAPISyncStateImageVk_t& other ) const;

	bool				   bHasPendingOwnershipTransfer;
	VkImageLayout		   vkImageLayout;
	studioAPISyncScopeVk_t readScope;
	studioAPISyncScopeVk_t writeScope;
	uint32				   ownerQueueFamilyIndex;
};

//-----------------------------------------------------------------------------
// Vulkan StudioAPI synchronization manager
//-----------------------------------------------------------------------------
class CStudioAPISyncMgrVk
{
public:
	CStudioAPISyncMgrVk();
	~CStudioAPISyncMgrVk();

	void Init();
	void Shutdown();

	void AddFrameInFlightFence( CStudioAPIFenceVk* pFence, uint32 indexFrameInFlight );

	void WaitFrameInFlight( uint32 indexFrameInFlight );
	void WaitAllFrameInFlights();

	CStudioAPIFenceVk*			CreateFence( bool bCreateSignaled = false );
	CStudioAPISemaphoreVk*		CreateSemaphore();
	CStudioAPINamedSemaphoreVk* FindOrCreateNamedSemaphore( const char* pName );

	// The function places a fence/semaphore into a pending list to free and will be released
	// at same index of frame in-flight later
	void ReleaseFence( CStudioAPIFenceVk*& pFence );
	void ReleaseSemaphore( CStudioAPISemaphoreVk*& pSemaphore );
	void ReleaseNamedSemaphore( CStudioAPINamedSemaphoreVk*& pNamedSemaphore );

private:
	eastl::list<CStudioAPIFenceVk*>									  fences;
	eastl::list<CStudioAPISemaphoreVk*>								  semaphores;
	eastl::list<CStudioAPIFenceVk*>									  frameInFlightFences[STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT];
	eastl::list<CStudioAPIFenceVk*>									  pendingFreeFences[STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT];
	eastl::list<CStudioAPISemaphoreVk*>								  pendingFreeSemaphores[STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT];
	eastl::list<CStudioAPIFenceVk*>									  freeFences;
	eastl::list<CStudioAPISemaphoreVk*>								  freeSemaphores;
	eastl::unordered_map<eastl::string_view, CStudioAPINamedSemaphoreVk*> namedSemaphoresDict;
};

//-----------------------------------------------------------------------------
// Update synchronization functions
//-----------------------------------------------------------------------------
void VK_UpdateSyncStateBuffer( VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex, studioAPISyncStateBufferVk_t& syncState );
bool VK_UpdateSyncStateBufferWithBarrier( VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex, studioAPISyncStateBufferVk_t& syncState, studioAPIBufferMemoryBarrierVk_t& bufferMemoryBarrier, uint32 bufferUsageFlags );
void VK_UpdateSyncStateImage( VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex, studioAPISyncStateImageVk_t& syncState );
bool VK_UpdateSyncStateImageWithBarrier( VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex, studioAPISyncStateImageVk_t& syncState, studioAPIImageMemoryBarrierVk_t& imageMemoryBarrier, uint32 textureUsageFlags );

#include "studiorender/studioapi/vk/vk_studioapi_syncmgr.inl"