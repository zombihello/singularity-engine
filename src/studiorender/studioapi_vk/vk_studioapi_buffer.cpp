#include "pch_studioapi_vk.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdcontext.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdbuffer.h"
#include "studiorender/studioapi_vk/vk_studioapi_buffer.h"

/*
==================
VK_TranslateBufferUsageFlags
==================
*/
static void VK_TranslateBufferUsageFlags( uint32& usageFlags, VkBufferUsageFlags& vkBufferUsageFlags, VmaMemoryUsage& vmaMemoryUsage, uint8& memoryFlags )
{
	// Clear output parameters
	vkBufferUsageFlags = 0;
	memoryFlags		   = 0;
	vmaMemoryUsage	   = VMA_MEMORY_USAGE_UNKNOWN;

	// Translate the buffer usage flags
	// Vertex buffer
	if ( usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_VERTEX_BUFFER )
	{
		vkBufferUsageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	}

	// Index buffer
	if ( usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_INDEX_BUFFER )
	{
		vkBufferUsageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	}

	// Constant buffer
	if ( usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_CONSTANT_BUFFER )
	{
		vkBufferUsageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	}

	// Structured buffer
	if ( usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_STRUCTURED_BUFFER )
	{
		vkBufferUsageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	}

	// Transfer source
	if ( usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_SRC )
	{
		vkBufferUsageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	}

	// Transfer destination
	if ( usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST )
	{
		vkBufferUsageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}

	// Select the right memory type
	if ( usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_VOLATILE )
	{
		vmaMemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		memoryFlags	   = STUDIOAPI_BUFFER_MEMORY_FLAG_CPU_GPU;

		// Volatile buffers share concurrently-owned temp blocks, so they must behave as if they were
		// created with STUDIOAPI_BUFFER_USAGE_FLAG_CONCURRENT regardless of what the caller passed in
		usageFlags |= STUDIOAPI_BUFFER_USAGE_FLAG_CONCURRENT;
	}
	else
	{
		vmaMemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		memoryFlags	   = STUDIOAPI_BUFFER_MEMORY_FLAG_GPU;

		usageFlags |= STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST;
		vkBufferUsageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}
}

/*
==================
CStudioAPIBufferVk::CStudioAPIBufferVk
==================
*/
CStudioAPIBufferVk::CStudioAPIBufferVk( const byte* pData, uint64 dataSize, uint32 dataStride, uint32 usageFlags, const char* pDebugName /* = "" */ )
	: usageFlags( usageFlags )
	, memoryFlags( 0 )
	, size( dataSize )
	, stride( dataStride )
	, numBuffers( GetNumBuffersFromUsage( usageFlags ) )
	, currentBufferIndex( 0 )
	, onStudioAPIVkShutdownHandle( INVALID_HANDLE )
{
	// The buffer size and data stride must be valid
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( dataSize > 0 );
	Assert( dataSize >= dataStride );
	Mem_Memzero( &alloc, sizeof( bufferAlloc_t ) );

	// Convert to VMA memory usage and Vulkan buffer usage flags
	VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
	VkBufferUsageFlags		vkBufferUsageFlags;
	VK_TranslateBufferUsageFlags( CStudioAPIBufferVk::usageFlags, vkBufferUsageFlags, vmaAllocationCreateInfo.usage, memoryFlags );

	// Allocate a new Vulkan buffer if the buffer isn't volatile
	// NOTE: volatile buffers don't own GPU memory of their own: every MapMemory/UpdateData grabs a
	// fresh region from the engine-wide temp allocator instead, so past draws that reference
	// an older region keep seeing their own data
	if ( !( usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_VOLATILE ) )
	{
		// Allocate memory for buffers
		VkBufferCreateInfo vkBufferCreateInfo = {};
		vkBufferCreateInfo.sType			  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vkBufferCreateInfo.size				  = dataSize * numBuffers;
		vkBufferCreateInfo.usage			  = vkBufferUsageFlags;

		// Grab queue family indices which the buffer should be use
		uint32 graphicsQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		uint32 computeQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
		uint32 transferQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		if ( usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_CONCURRENT )
		{
			VK_GetUsedQueueFamiliesFromVkBufferUsage( vkBufferUsageFlags, graphicsQueueFamilyIndex, computeQueueFamilyIndex, transferQueueFamilyIndex );
		}

		CStudioAPIQueueSharingModeSetupVk queueSharingModeSetup( graphicsQueueFamilyIndex, computeQueueFamilyIndex, transferQueueFamilyIndex );
		queueSharingModeSetup.Setup( vkBufferCreateInfo.sharingMode, vkBufferCreateInfo.queueFamilyIndexCount, vkBufferCreateInfo.pQueueFamilyIndices );
		alloc.vmaAllocation = g_StudioAPIVk.GetMemoryMgr().AllocateBuffer( S_Sprintf( "Buffer (usageFlags: 0x%X)", usageFlags ).c_str(), vkBufferCreateInfo, vmaAllocationCreateInfo, alloc.vkBuffer );
		if ( alloc.vmaAllocation == VK_NULL_HANDLE )
		{
			Sys_Error( "Failed to allocate GPU buffer with size %llu", dataSize );
			return;
		}
		alloc.frameNumber = g_StudioAPIVk.GetFrameNumber();

		// Register in 'onStudioAPIVkShutodwn' for destroy Vulkan objects when the one is shutdown
		onStudioAPIVkShutdownHandle = g_StudioAPIVk.OnStudioAPIVkShutdown().Subscribe( &CStudioAPIBufferVk::OnStudioAPIVkShutdown, this );
	}

	// Copy data if we have it
	if ( pData )
	{
		UpdateData( g_StudioAPIVk.GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_GRAPHICS ), (byte*)pData, dataSize );
	}
}

/*
==================
CStudioAPIBufferVk::~CStudioAPIBufferVk
==================
*/
CStudioAPIBufferVk::~CStudioAPIBufferVk()
{
	// Destroy the buffer (volatile buffers don't own their allocation, it belongs to the shared temp allocator)
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	if ( alloc.vmaAllocation != VK_NULL_HANDLE )
	{
		g_StudioAPIVk.GetMemoryMgr().FreeResource( [vkBuffer = alloc.vkBuffer, vmaAllocation = alloc.vmaAllocation]()
												   { g_StudioAPIVk.GetMemoryMgr().DestroyBuffer( vkBuffer, vmaAllocation ); } );
		alloc.vkBuffer		= VK_NULL_HANDLE;
		alloc.vmaAllocation = VK_NULL_HANDLE;
	}

	// Remove CStudioAPIBufferVk::OnStudioAPIVkShutdown from event 'onStudioAPIVkShutodwn'
	if ( onStudioAPIVkShutdownHandle != INVALID_HANDLE )
	{
		g_StudioAPIVk.OnStudioAPIVkShutdown().Unsubscribe( onStudioAPIVkShutdownHandle );
		onStudioAPIVkShutdownHandle = INVALID_HANDLE;
	}
}

/*
==================
CStudioAPIBufferVk::GetUsageFlags
==================
*/
uint32 CStudioAPIBufferVk::GetUsageFlags() const
{
	return usageFlags;
}

/*
==================
CStudioAPIBufferVk::GetSize
==================
*/
uint64 CStudioAPIBufferVk::GetSize() const
{
	return size;
}

/*
==================
CStudioAPIBufferVk::GetStride
==================
*/
uint32 CStudioAPIBufferVk::GetStride() const
{
	return stride;
}

/*
==================
CStudioAPIBufferVk::GetMemoryFlags
==================
*/
uint8 CStudioAPIBufferVk::GetMemoryFlags() const
{
	return memoryFlags;
}

/*
==================
CStudioAPIBufferVk::AllocFromTempAlloc
==================
*/
void CStudioAPIBufferVk::AllocFromTempAlloc()
{
	// Translate usage flags again to get the Vulkan usage flags for the alignment query below
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_VOLATILE );
	VmaAllocationCreateInfo dummyVmaAllocationCreateInfo = {};
	uint32					dummyUsageFlags				 = usageFlags;
	uint8					dummyMemoryFlags;
	VkBufferUsageFlags		vkBufferUsageFlags;
	VK_TranslateBufferUsageFlags( dummyUsageFlags, vkBufferUsageFlags, dummyVmaAllocationCreateInfo.usage, dummyMemoryFlags );

	// Grab a fresh sub-allocation from the temp allocator
	uint64		alignment = CStudioAPIMemoryMgrVk::GetBufferAlignmentFromVkUsageFlags( vkBufferUsageFlags );
	tempAlloc_t tempAlloc = g_StudioAPIVk.GetTempAlloc().Alloc( (uint32)size, (uint32)alignment );
	alloc.vkBuffer		  = tempAlloc.vkBuffer;
	alloc.vmaAllocation	  = VK_NULL_HANDLE;
	alloc.pData			  = tempAlloc.pData;
	alloc.offset		  = tempAlloc.offset;
	alloc.frameNumber	  = g_StudioAPIVk.GetFrameNumber();

	// A fresh temp allocation is a fresh piece of memory, nothing has touched it yet
	syncState = studioAPISyncStateBufferVk_t();
}

/*
==================
CStudioAPIBufferVk::IsValidAlloc
==================
*/
bool CStudioAPIBufferVk::IsValidAlloc() const
{
	bool bVolatile = usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_VOLATILE;
	return alloc.vkBuffer != VK_NULL_HANDLE && ( !bVolatile || alloc.frameNumber == g_StudioAPIVk.GetFrameNumber() );
}

/*
==================
CStudioAPIBufferVk::PrepareForGPUWrite
==================
*/
void CStudioAPIBufferVk::PrepareForGPUWrite()
{
	// A GPU write (`CopyBuffer`/`CopyTextureToBuffer`/etc as destination) into a volatile buffer must land in a brand-new
	// temp sub-allocation, exactly like a host write does: otherwise the copy either targets a stale block already
	// recycled to someone else this frame, or aliases memory the buffer's own `GetVkBuffer()`/`GetOffset()` no longer
	// point at. Grabbing a fresh block here also resets the sync state, so the subsequent transfer-write and the
	// barrier that reads it are tracked against the block that actually received the copy.
	// Non-volatile buffers keep their own allocation and ring-buffer via `SwapCurrentBufferIndex` instead
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	if ( usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_VOLATILE )
	{
		AllocFromTempAlloc();
	}
}

/*
==================
CStudioAPIBufferVk::MapMemory
==================
*/
void CStudioAPIBufferVk::MapMemory( uint64 size, uint64 offset, studioAPIMappedBufferData_t& mappedData )
{
	// We can use MapMemory only for CPU shared memory, i.e. volatile buffers
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	AssertMsg( memoryFlags & STUDIOAPI_BUFFER_MEMORY_FLAG_CPU, "The buffer doesn't support the map memory, use CStudioAPICmdListVk::CopyBuffer" );
	Assert( !mappedData.pData && offset < CStudioAPIBufferVk::size && size <= CStudioAPIBufferVk::size - offset );
	Mem_Memzero( &mappedData, sizeof( studioAPIMappedBufferData_t ) );

	// If the buffer is volatile allocate a new memory from the temp allocator
	if ( usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_VOLATILE )
	{
		AllocFromTempAlloc();
		mappedData.pData = alloc.pData + offset;
	}
	// Otherwise we can use MapMemory to read/write data in the buffer
	else
	{
		mappedData.pData = g_StudioAPIVk.GetMemoryMgr().MapMemory<byte>( alloc.vmaAllocation ) + alloc.offset + offset;
	}
	mappedData.size = size;
}

/*
==================
CStudioAPIBufferVk::UnmapMemory
==================
*/
void CStudioAPIBufferVk::UnmapMemory( studioAPIMappedBufferData_t& mappedData )
{
	// Unmap CPU shared memory if the buffer isn't volatile because the temp allocation is persistently mapped
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( mappedData.pData );
	if ( !( usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_VOLATILE ) )
	{
		g_StudioAPIVk.GetMemoryMgr().UnmapMemory( alloc.vmaAllocation );
	}
	mappedData.pData = NULL;
}

/*
==================
CStudioAPIBufferVk::UpdateData
==================
*/
void CStudioAPIBufferVk::UpdateData( IStudioAPICmdContext* pCmdContext, byte* pData, uint64 dataSize, uint64 offset /* = 0 */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Copy the data by CStudioAPIDataUploaderVk (only for GPU only memory)
	Assert( pData && offset + dataSize <= size );
	if ( !( memoryFlags & STUDIOAPI_BUFFER_MEMORY_FLAG_CPU ) )
	{
		// Swap the current buffer index
		Assert( pCmdContext );
		Assert( usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST );
		SwapCurrentBufferIndex();

		// Copy the data by CStudioAPIDataUploaderVk
		CStudioAPICmdContextVk* pStudioAPICmdContext   = (CStudioAPICmdContextVk*)pCmdContext;
		CStudioAPICmdContextVk* pTransferCmdContext	   = (CStudioAPICmdContextVk*)g_StudioAPIVk.GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_TRANSFER );
		bool					bNeedOwnershipTransfer = !( usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_CONCURRENT ) && pStudioAPICmdContext->GetQueue().GetQueueFamilyIndex() != pTransferCmdContext->GetQueue().GetQueueFamilyIndex();
		VkBufferMemoryBarrier	vkBufferMemoryBarrier  = {};
		vkBufferMemoryBarrier.sType					   = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		vkBufferMemoryBarrier.srcQueueFamilyIndex	   = VK_QUEUE_FAMILY_IGNORED;
		vkBufferMemoryBarrier.dstQueueFamilyIndex	   = VK_QUEUE_FAMILY_IGNORED;
		vkBufferMemoryBarrier.buffer				   = alloc.vkBuffer;
		vkBufferMemoryBarrier.offset				   = alloc.offset + offset;
		vkBufferMemoryBarrier.size					   = size;
		CStudioAPIDataUploaderVk& dataUploader		   = g_StudioAPIVk.GetDataUploader();
		dataUploader.Upload( dataSize, STUDIOAPI_VK_BUFFER_OFFSET_ALIGNMENT,
							 [this, offset, &vkBufferMemoryBarrier, bNeedOwnershipTransfer, pStudioAPICmdContext, pTransferCmdContext, pData, dataSize]( const CStudioAPIDataUploaderVk::uploadParams_t& uploadParams )
							 {
								 // Copy the data into the staging buffer
								 uint64 uploadOffset = dataSize - uploadParams.remainSizeToUpload;
								 Mem_Memcpy( uploadParams.pStagingBufferData, pData + uploadOffset, uploadParams.partialUploadSize );

								 // Make a barrier if it need
								 if ( uploadParams.remainSizeToUpload == dataSize )
								 {
									 vkBufferMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
									 vkBufferMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
									 vkCmdPipelineBarrier( uploadParams.pCmdBuffer->GetVkCommandBuffer(), VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 1, &vkBufferMemoryBarrier, 0, NULL );
								 }

								 // Place vkCmdCopyBuffer call
								 VkBufferCopy vkBufferCopy = {};
								 vkBufferCopy.srcOffset	   = uploadParams.stagingBufferOffset;
								 vkBufferCopy.dstOffset	   = alloc.offset + offset + uploadOffset;
								 vkBufferCopy.size		   = uploadParams.partialUploadSize;
								 vkCmdCopyBuffer( uploadParams.pCmdBuffer->GetVkCommandBuffer(), uploadParams.vkStagingBuffer, alloc.vkBuffer, 1, &vkBufferCopy );

								 // Make a barrier if it need
								 if ( uploadParams.remainSizeToUpload - uploadParams.partialUploadSize == 0 )
								 {
									 // This either just barrier that waits for copy completion, or if bNeedOwnershipTransfer is TRUE,
									 // then it is a wait and release ownership barrier
									 // NOTE BS yehor.pohuliaka - Maybe no need do transfer ownership here and it must do user on outside?
									 vkBufferMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
									 vkBufferMemoryBarrier.dstAccessMask = VK_ACCESS_NONE;
									 if ( bNeedOwnershipTransfer )
									 {
										 vkBufferMemoryBarrier.srcQueueFamilyIndex = pTransferCmdContext->GetQueue().GetQueueFamilyIndex();
										 vkBufferMemoryBarrier.dstQueueFamilyIndex = pStudioAPICmdContext->GetQueue().GetQueueFamilyIndex();
									 }
									 vkCmdPipelineBarrier( uploadParams.pCmdBuffer->GetVkCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 1, &vkBufferMemoryBarrier, 0, NULL );
								 }

								 return uploadParams.partialUploadSize;
							 } );

		// Update the synchronization state
		UpdateSyncState( VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, pTransferCmdContext->GetQueue().GetQueueFamilyIndex() );
		syncState.bHasPendingOwnershipTransfer = bNeedOwnershipTransfer;
	}
	// Otherwise use mapped memory
	else
	{
		studioAPIMappedBufferData_t mappedData = {};
		MapMemory( dataSize, offset, mappedData );
		Mem_Memcpy( mappedData.pData, pData, mappedData.size );
		UnmapMemory( mappedData );
	}
}

/*
==================
CStudioAPIBufferVk::UpdateSyncStateWithBarrier
==================
*/
void CStudioAPIBufferVk::UpdateSyncStateWithBarrier( CStudioAPICmdListVk* pCmdList, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex )
{
	// A volatile buffer only gets its memory at the moment of a write, so a barrier placed before that write refers to
	// a block the buffer doesn't own yet - there is nothing to synchronize against, skip it. Once the buffer has been
	// written this frame the barrier below is emitted as usual, which is what makes a GPU write visible to a reader
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	if ( ( usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_VOLATILE ) && !IsValidAlloc() )
	{
		return;
	}

	studioAPIBufferMemoryBarrierVk_t bufferMemoryBarrier = {};
	bufferMemoryBarrier.vkBufferMemoryBarrier.buffer	 = alloc.vkBuffer;
	bufferMemoryBarrier.vkBufferMemoryBarrier.offset	 = alloc.offset;
	bufferMemoryBarrier.vkBufferMemoryBarrier.size		 = size;
	if ( VK_UpdateSyncStateBufferWithBarrier( vkDstAccessMask, vkDstStageMask, dstQueueFamilyIndex, syncState, bufferMemoryBarrier, usageFlags ) )
	{
		CStudioAPICmdContextVk* pCmdContext = (CStudioAPICmdContextVk*)pCmdList->GetCmdContext();
		pCmdContext->AddPendingBufferBarriers( pCmdList, 1, &bufferMemoryBarrier.vkBufferMemoryBarrier, bufferMemoryBarrier.vkSrcStageMask, bufferMemoryBarrier.vkDstStageMask );
	}
}

/*
==================
CStudioAPIBufferVk::OnStudioAPIVkShutdown
==================
*/
void CStudioAPIBufferVk::OnStudioAPIVkShutdown( void* pUserData )
{
	Assert( pUserData );
	CStudioAPIBufferVk* pStudioAPIMultiBuffer		   = (CStudioAPIBufferVk*)pUserData;
	pStudioAPIMultiBuffer->onStudioAPIVkShutdownHandle = INVALID_HANDLE;
	pStudioAPIMultiBuffer->~CStudioAPIBufferVk();
}
