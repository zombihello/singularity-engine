#pragma once
#include "studiorender/studioapi_vk/vk_helpers.h"

//-----------------------------------------------------------------------------
// A temp allocation in a temp allocator
//-----------------------------------------------------------------------------
struct tempAlloc_t
{
	VkBuffer vkBuffer;	// Temp block's Vulkan buffer
	uint32	 size;		// Allocated size for the allocation
	uint32	 offset;	// Byte offset of the allocation inside `vkBuffer`
	byte*	 pData;		// CPU write pointer, already offset to the allocation
};

//-----------------------------------------------------------------------------
// Vulkan simple temp allocator used for volatile allocations
//-----------------------------------------------------------------------------
template<uint32 blockSize, VkBufferUsageFlags vkBufferUsageFlags>
class CStudioAPITempAllocVk
{
public:
	CStudioAPITempAllocVk( const char* pAllocName = "StudioAPIVk TempAlloc" );
	~CStudioAPITempAllocVk();

	void		Init();
	void		Shutdown();
	tempAlloc_t Alloc( uint32 size, uint32 alignment );
	void		SwapPools();

private:
	struct tempMemoryBlock_t;
	using tempMemoryBlockList_t	  = eastl::list<tempMemoryBlock_t>;
	using tempMemoryBlockListIt_t = typename tempMemoryBlockList_t::iterator;

	struct tempMemoryBlock_t
	{
		VkBuffer	  vkBuffer;
		VmaAllocation vmaAllocation;
		byte*		  pData;
		uint32		  usedSize;
	};

	struct tempMemoryPool_t
	{
		uint32					id;
		tempMemoryBlockList_t	blockList;
		tempMemoryBlockListIt_t currentBlockIt;
	};

	tempMemoryBlockListIt_t AllocBlock( tempMemoryPool_t& pool );
	tempMemoryBlockListIt_t FreeBlock( tempMemoryPool_t& pool, tempMemoryBlockListIt_t blockIt );
	void					FreeAllBlocks( tempMemoryPool_t& pool );

	uint32			 currentPoolId;
	tempMemoryPool_t pools[STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT];
	const char*		 pAllocName;
};

// Single engine-wide temp allocator shared by volatile vertex/index/constant buffers
using studioAPITempAllocVk_t = CStudioAPITempAllocVk<STUDIOAPI_VK_TEMPALLOC_BLOCK_SIZE, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT>;
#include "studiorender/studioapi_vk/vk_studioapi_tempalloc.inl"
