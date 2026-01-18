#pragma once
#include <list>

#include "vma/vk_mem_alloc.h"
#include "studiorender/studioapi/vk/vk_helpers.h"

//-----------------------------------------------------------------------------
// Vulkan memory constants and structs
//-----------------------------------------------------------------------------
#define STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION !RETAIL

struct studioAPIMemoryStatsVk_t
{
	uint64 usedMemory;
	uint64 totalAvailableMemory;
	uint64 allocationCount;
	uint64 bufferAllocationSize;
	uint64 bufferAllocationCount;
	uint64 imageAllocationSize;
	uint64 imageAllocationCount;
};

//-----------------------------------------------------------------------------
// Vulkan StudioAPI memory manager
//-----------------------------------------------------------------------------
class CStudioAPIMemoryMgrVk
{
public:
	CStudioAPIMemoryMgrVk();
	~CStudioAPIMemoryMgrVk();

	void Init( uint32 vkVersion );
	void Shutdown();

	VmaAllocation AllocateBuffer( const char* pDebugTag, const VkBufferCreateInfo& vkBufferCreateInfo, const VmaAllocationCreateInfo& vmaAllocationCreateInfo, VkBuffer& vkBuffer, VkDeviceSize* pAllocatedSize = NULL ) const;
	VmaAllocation AllocateImage( const char* pDebugTag, const VkImageCreateInfo& vkImageCreateInfo, const VmaAllocationCreateInfo& vmaAllocationCreateInfo, VkImage& vkImage, VkDeviceSize* pAllocatedSize = NULL ) const;
	void		  DestroyBuffer( VkBuffer vkBuffer, VmaAllocation vmaAllocation ) const;
	void		  DestroyImage( VkImage vkImage, VmaAllocation vmaAllocation ) const;

	template<typename TFreeLambda>
	void FreeResource( TFreeLambda&& freeLambda ) const;
	void FreePendingResources( uint32 indexFrameInFlight ) const;

	template<typename TType>
	TType* MapMemory( VmaAllocation vmaAllocation ) const;
	void   UnmapMemory( VmaAllocation vmaAllocation ) const;

	studioAPIMemoryStatsVk_t GetStats() const;
	static uint64			 GetBufferAlignmentFromVkUsageFlags( VkBufferUsageFlags vkBufferUsageFlags );

private:
	typedef void ( *pendingFreeResourceFn_t )( void* pUserData );
	enum allocationType_t
	{
		ALLOCATION_TYPE_NONE,
		ALLOCATION_TYPE_BUFFER,
		ALLOCATION_TYPE_IMAGE
	};

	struct allocationInfo_t
	{
		allocationType_t type;
		uint64			 size;
	};

	struct cmdBuffer_t
	{
		byte*  pBuffer;
		byte*  pWritePtr;
		uint32 size;
		uint32 numCmds;
	};

	cmdBuffer_t& GetCurrentPendingFreeCmdBuffer() const;
	void		 ExecPendingFreeCmdBuffer( cmdBuffer_t& cmdBuffer ) const;

#if STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
	mutable uint64												memoryUsage;
	mutable std::unordered_map<VmaAllocation, allocationInfo_t> allocationDict;
#endif	// STUDIOAPI_VK_GPU_TRACK_MEMORY_ALLOCATION
	VmaAllocator		vmaAllocator;
	mutable cmdBuffer_t pendingFreeCmdBuffers[STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT];
};

#include "studiorender/studioapi/vk/vk_studioapi_memorymgr.inl"