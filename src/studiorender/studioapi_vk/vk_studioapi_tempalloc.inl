#pragma once

/*
==================
CStudioAPITempAllocVk::CStudioAPITempAllocVk
==================
*/
template<uint32 blockSize, VkBufferUsageFlags vkBufferUsageFlags>
FORCEINLINE CStudioAPITempAllocVk<blockSize, vkBufferUsageFlags>::CStudioAPITempAllocVk( const char* pAllocName /* = "StudioAPIVk TempAlloc" */ )
	: currentPoolId( INVALID_INDEX )
	, pAllocName( pAllocName )
{
}

/*
==================
CStudioAPITempAllocVk::~CStudioAPITempAllocVk
==================
*/
template<uint32 blockSize, VkBufferUsageFlags vkBufferUsageFlags>
FORCEINLINE CStudioAPITempAllocVk<blockSize, vkBufferUsageFlags>::~CStudioAPITempAllocVk()
{
	Shutdown();
}

/*
==================
CStudioAPITempAllocVk::Init
==================
*/
template<uint32 blockSize, VkBufferUsageFlags vkBufferUsageFlags>
void CStudioAPITempAllocVk<blockSize, vkBufferUsageFlags>::Init()
{
	// Allocate one page for each pool
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	for ( uint32 index = 0; index < STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT; ++index )
	{
		tempMemoryPool_t& pool = pools[index];
		pool.id				   = index;
		pool.currentBlockIt	   = AllocBlock( pool );
	}
}

/*
==================
CStudioAPITempAllocVk::Shutdown
==================
*/
template<uint32 blockSize, VkBufferUsageFlags vkBufferUsageFlags>
void CStudioAPITempAllocVk<blockSize, vkBufferUsageFlags>::Shutdown()
{
	// Free all pages
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	for ( uint32 index = 0; index < STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT; ++index )
	{
		FreeAllBlocks( pools[index] );
	}

	// Reset current pool index
	currentPoolId = INVALID_INDEX;
}

/*
==================
CStudioAPITempAllocVk::Alloc
==================
*/
template<uint32 blockSize, VkBufferUsageFlags vkBufferUsageFlags>
tempAlloc_t CStudioAPITempAllocVk<blockSize, vkBufferUsageFlags>::Alloc( uint32 size, uint32 alignment )
{
	// Calculate alignment
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( currentPoolId != INVALID_INDEX );
	alignment = S_Max<uint32>( alignment, STUDIOAPI_VK_BUFFER_OFFSET_ALIGNMENT );
	AssertMsg( size <= blockSize, "A temp allocator can't allocate memory more then block size" );

	// Try to allocate memory in the current block, moving forward through already recycled blocks
	// and growing the pool only when none of them is left
	tempMemoryPool_t& pool = pools[currentPoolId];
	while ( true )
	{
		// Allocate a new block if we ran out of them
		if ( pool.currentBlockIt == pool.blockList.end() )
		{
			pool.currentBlockIt = AllocBlock( pool );
		}

		// NOTE: We align the offset inside the buffer, not the mapped CPU pointer, because it is the
		// offset that Vulkan requires to be aligned (VkDescriptorBufferInfo::offset, vkCmdBindVertexBuffers and etc)
		tempMemoryBlock_t& block		 = *pool.currentBlockIt;
		uint32			   freeSize		 = blockSize - block.usedSize;
		uint32			   alignedOffset = S_Align<uint32>( block.usedSize, alignment );
		uint32			   alignPadding	 = alignedOffset - block.usedSize;
		if ( size + alignPadding <= freeSize )
		{
			tempAlloc_t tempAlloc = {};
			block.usedSize		  = size + alignedOffset;
			tempAlloc.vkBuffer	  = block.vkBuffer;
			tempAlloc.size		  = size;
			tempAlloc.offset	  = alignedOffset;
			tempAlloc.pData		  = block.pData + alignedOffset;
			return tempAlloc;
		}

		// Not enough room left in the block. If the block was empty the request
		// can never be satisfied and moving forward would grow the pool forever
		if ( block.usedSize == 0 )
		{
			Sys_Error( "Vulkan temp allocator can't allocate %i bytes with alignment %i (block size: %i, pool: %i, name: '%s')", size, alignment, blockSize, pool.id, pAllocName );
			return tempAlloc_t{};
		}

		// Not enough room left in the block
		++pool.currentBlockIt;
	}

	// If we here it is fatal error
	AssertNoEntry();
	return tempAlloc_t{};
}

/*
==================
CStudioAPITempAllocVk::SwapPools
==================
*/
template<uint32 blockSize, VkBufferUsageFlags vkBufferUsageFlags>
void CStudioAPITempAllocVk<blockSize, vkBufferUsageFlags>::SwapPools()
{
	// Move to the pool of the frame-in-flight we've just waited on and recycle its blocks,
	// its GPU work is guaranteed to be finished by now
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	currentPoolId = g_StudioAPIVk.GetCurrentFrameInFlight();

	// Reset used size in blocks
	tempMemoryPool_t& pool = pools[currentPoolId];
	for ( auto it = pool.blockList.begin(), itEnd = pool.blockList.end(); it != itEnd; ++it )
	{
		it->usedSize = 0;
	}

	// Reset current block to the begin
	pool.currentBlockIt = pool.blockList.begin();
}

/*
==================
CStudioAPITempAllocVk::AllocBlock
==================
*/
template<uint32 blockSize, VkBufferUsageFlags vkBufferUsageFlags>
typename CStudioAPITempAllocVk<blockSize, vkBufferUsageFlags>::tempMemoryBlockListIt_t CStudioAPITempAllocVk<blockSize, vkBufferUsageFlags>::AllocBlock( tempMemoryPool_t& pool )
{
	// Allocate memory for a new block in the pool
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	VkBufferCreateInfo		vkBufferCreateInfo		= {};
	VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
	vkBufferCreateInfo.sType						= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBufferCreateInfo.size							= blockSize;
	vkBufferCreateInfo.usage						= vkBufferUsageFlags;
	vmaAllocationCreateInfo.usage					= VMA_MEMORY_USAGE_CPU_TO_GPU;

	// Grab queue family indices which the buffer should be use
	uint32 graphicsQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	uint32 computeQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
	uint32 transferQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	VK_GetUsedQueueFamiliesFromVkBufferUsage( vkBufferUsageFlags, graphicsQueueFamilyIndex, computeQueueFamilyIndex, transferQueueFamilyIndex );

	tempMemoryBlock_t				  block = {};
	CStudioAPIQueueSharingModeSetupVk queueSharingModeSetup( graphicsQueueFamilyIndex, computeQueueFamilyIndex, transferQueueFamilyIndex );
	queueSharingModeSetup.Setup( vkBufferCreateInfo.sharingMode, vkBufferCreateInfo.queueFamilyIndexCount, vkBufferCreateInfo.pQueueFamilyIndices );
	block.vmaAllocation = g_StudioAPIVk.GetMemoryMgr().AllocateBuffer( "tempblock", vkBufferCreateInfo, vmaAllocationCreateInfo, block.vkBuffer );
	if ( block.vmaAllocation == VK_NULL_HANDLE )
	{
		Sys_Error( "Failed to allocate GPU buffer for a temp block with size %llu (pool: %i, name: '%s')", blockSize, pool.id, pAllocName );
		return pool.blockList.end();
	}
	Msg( "StudioAPIVk: Allocated GPU block for a temp block (%llu bytes, pool: %i, name: '%s')", blockSize, pool.id, pAllocName );

	// Map memory and add the block into the list
	block.pData = g_StudioAPIVk.GetMemoryMgr().MapMemory<byte>( block.vmaAllocation );
	pool.blockList.emplace_back( block );
	PROFILER_MEM_ALLOC( (void*)block.vkBuffer, blockSize, pAllocName );
	return --pool.blockList.end();
}

/*
==================
CStudioAPITempAllocVk::FreeBlock
==================
*/
template<uint32 blockSize, VkBufferUsageFlags vkBufferUsageFlags>
typename CStudioAPITempAllocVk<blockSize, vkBufferUsageFlags>::tempMemoryBlockListIt_t CStudioAPITempAllocVk<blockSize, vkBufferUsageFlags>::FreeBlock( tempMemoryPool_t& pool, tempMemoryBlockListIt_t blockIt )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( blockIt != pool.blockList.end() );

	// Free GPU memory
	tempMemoryBlock_t& block = *blockIt;
	PROFILER_MEM_FREE( (void*)block.vkBuffer, pAllocName );
	g_StudioAPIVk.GetMemoryMgr().UnmapMemory( block.vmaAllocation );
	g_StudioAPIVk.GetMemoryMgr().DestroyBuffer( block.vkBuffer, block.vmaAllocation );

	// Remove the block from the list
	bool					bCurrentBlock = pool.currentBlockIt == blockIt;
	tempMemoryBlockListIt_t nextBlockIt	  = pool.blockList.erase( blockIt );
	if ( bCurrentBlock )
	{
		pool.currentBlockIt = nextBlockIt;
	}

	// We are done
	Msg( "StudioAPIVk: Freed GPU buffer for a temp block (%llu bytes, pool: %i, name: '%s')", blockSize, pool.id, pAllocName );
	return nextBlockIt;
}

/*
==================
CStudioAPITempAllocVk::FreeAllBlocks
==================
*/
template<uint32 blockSize, VkBufferUsageFlags vkBufferUsageFlags>
void CStudioAPITempAllocVk<blockSize, vkBufferUsageFlags>::FreeAllBlocks( tempMemoryPool_t& pool )
{
	// Do nothing if the list is empty
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	if ( pool.blockList.empty() )
	{
		return;
	}

	// Free each block
	for ( tempMemoryBlockListIt_t it = pool.blockList.begin(); it != pool.blockList.end(); )
	{
		it = FreeBlock( pool, it );
	}
	pool.currentBlockIt = pool.blockList.end();
}
