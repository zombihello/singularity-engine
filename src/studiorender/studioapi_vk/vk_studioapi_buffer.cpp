#include "pch_studioapi_vk.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdcontext.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdbuffer.h"
#include "studiorender/studioapi_vk/vk_studioapi_buffer.h"

/*
==================
VK_TranslateBufferUsageFlags
==================
*/
void VK_TranslateBufferUsageFlags( uint32& usageFlags, VkBufferUsageFlags& vkBufferUsageFlags, VmaMemoryUsage& vmaMemoryUsage, uint8& memoryFlags )
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
VK_GetUsedQueueFamiliesFromBufferUsage
==================
*/
void VK_GetUsedQueueFamiliesFromBufferUsage( uint32 usageFlags, VkBufferUsageFlags vkBufferUsageFlags, VmaMemoryUsage vmaMemoryUsage, uint32& graphicsQueueFamilyIndex, uint32& computeQueueFamilyIndex, uint32& transferQueueFamilyIndex )
{
	graphicsQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	computeQueueFamilyIndex	 = VK_QUEUE_FAMILY_IGNORED;
	transferQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	// Do nothing if we haven't STUDIOAPI_BUFFER_USAGE_FLAG_CONCURRENT
	if ( !( usageFlags & STUDIOAPI_BUFFER_USAGE_FLAG_CONCURRENT ) )
	{
		return;
	}

	// Graphics queue family
	if ( vkBufferUsageFlags & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT || vkBufferUsageFlags & VK_BUFFER_USAGE_INDEX_BUFFER_BIT || vkBufferUsageFlags & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT || vkBufferUsageFlags & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT )
	{
		graphicsQueueFamilyIndex = g_StudioAPIVk.GetDevice().GetGraphicsQueue().GetQueueFamilyIndex();
	}

	// Compute queue family
	if ( vkBufferUsageFlags & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT || vkBufferUsageFlags & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT )
	{
		computeQueueFamilyIndex = g_StudioAPIVk.GetDevice().GetComputeQueue().GetQueueFamilyIndex();
	}

	// Transfer queue family
	if ( vmaMemoryUsage == VMA_MEMORY_USAGE_GPU_ONLY )
	{
		transferQueueFamilyIndex = g_StudioAPIVk.GetDevice().GetTransferQueue().GetQueueFamilyIndex();
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
	, vkBuffer( VK_NULL_HANDLE )
	, vmaAllocation( VK_NULL_HANDLE )
	, pStudioAPIVkShutdownDelegate( NULL )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// The buffer size and data stride must be valid
	Assert( dataSize > 0 );
	Assert( dataSize >= dataStride );

	// Convert to VMA memory usage and Vulkan buffer usage flags
	VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
	VkBufferUsageFlags		vkBufferUsageFlags;
	VK_TranslateBufferUsageFlags( CStudioAPIBufferVk::usageFlags, vkBufferUsageFlags, vmaAllocationCreateInfo.usage, memoryFlags );

	// Allocate memory for buffers
	VkBufferCreateInfo vkBufferCreateInfo = {};
	vkBufferCreateInfo.sType			  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBufferCreateInfo.size				  = dataSize * numBuffers;
	vkBufferCreateInfo.usage			  = vkBufferUsageFlags;

	// Grab queue family indices which the buffer should be use
	uint32 graphicsQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	uint32 computeQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
	uint32 transferQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	VK_GetUsedQueueFamiliesFromBufferUsage( CStudioAPIBufferVk::usageFlags, vkBufferUsageFlags, vmaAllocationCreateInfo.usage, graphicsQueueFamilyIndex, computeQueueFamilyIndex, transferQueueFamilyIndex );

	CStudioAPIQueueSharingModeSetupVk queueSharingModeSetup( graphicsQueueFamilyIndex, computeQueueFamilyIndex, transferQueueFamilyIndex );
	queueSharingModeSetup.Setup( vkBufferCreateInfo.sharingMode, vkBufferCreateInfo.queueFamilyIndexCount, vkBufferCreateInfo.pQueueFamilyIndices );
	vmaAllocation = g_StudioAPIVk.GetMemoryMgr().AllocateBuffer( S_Sprintf( "Buffer (usageFlags: 0x%X)", usageFlags ).c_str(), vkBufferCreateInfo, vmaAllocationCreateInfo, vkBuffer );
	if ( vmaAllocation == VK_NULL_HANDLE )
	{
		Sys_Error( "Failed to allocate GPU buffer with size %llu", dataSize );
		return;
	}

	// Initialize buffer offsets
	for ( uint32 bufferIdx = 0; bufferIdx < numBuffers; ++bufferIdx )
	{
		bufferOffsets[bufferIdx] = size * bufferIdx;
	}

	// Copy data if we have it
	if ( pData )
	{
		UpdateData( g_StudioAPIVk.GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_GRAPHICS ), (byte*)pData, dataSize );
	}

	// Register in 'onStudioAPIVkShutodwn' for destroy Vulkan objects when the one is shutdown
	pStudioAPIVkShutdownDelegate = g_StudioAPIVk.OnStudioAPIVkShutdown().AddFunc( &CStudioAPIBufferVk::OnStudioAPIVkShutdown, this );
}

/*
==================
CStudioAPIBufferVk::~CStudioAPIBufferVk
==================
*/
CStudioAPIBufferVk::~CStudioAPIBufferVk()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Destroy the buffer
	if ( vkBuffer != VK_NULL_HANDLE )
	{
		g_StudioAPIVk.GetMemoryMgr().FreeResource( [vkBuffer = vkBuffer, vmaAllocation = vmaAllocation]() {
			g_StudioAPIVk.GetMemoryMgr().DestroyBuffer( vkBuffer, vmaAllocation );
		} );
		vkBuffer	  = VK_NULL_HANDLE;
		vmaAllocation = VK_NULL_HANDLE;
	}

	// Remove CStudioAPIBufferVk::OnStudioAPIVkShutdown from event 'onStudioAPIVkShutodwn'
	if ( pStudioAPIVkShutdownDelegate )
	{
		g_StudioAPIVk.OnStudioAPIVkShutdown().RemoveFunc( pStudioAPIVkShutdownDelegate );
		pStudioAPIVkShutdownDelegate = NULL;
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
CStudioAPIBufferVk::MapMemory
==================
*/
void CStudioAPIBufferVk::MapMemory( uint64 size, uint64 offset, studioAPIMappedBufferData_t& mappedData )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// We can use MapMemory only for CPU shared memory
	AssertMsg( memoryFlags & STUDIOAPI_BUFFER_MEMORY_FLAG_CPU, "The buffer doesn't support the map memory, use IStudioAPICmdList::CopyBuffer" );
	Assert( !mappedData.pData && offset < CStudioAPIBufferVk::size && size <= CStudioAPIBufferVk::size - offset );
	Mem_Memzero( &mappedData, sizeof( studioAPIMappedBufferData_t ) );

	// For volatile buffers we can use MapMemory to read/write data in the buffer
	mappedData.pData = g_StudioAPIVk.GetMemoryMgr().MapMemory<byte>( vmaAllocation ) + bufferOffsets[currentBufferIndex] + offset;
	mappedData.size	 = size;
}

/*
==================
CStudioAPIBufferVk::UnmapMemory
==================
*/
void CStudioAPIBufferVk::UnmapMemory( studioAPIMappedBufferData_t& mappedData )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// For volatile buffers we can use UnmapMemory to read/write data in the buffer
	Assert( mappedData.pData );
	g_StudioAPIVk.GetMemoryMgr().UnmapMemory( vmaAllocation );
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
		vkBufferMemoryBarrier.buffer				   = vkBuffer;
		vkBufferMemoryBarrier.offset				   = bufferOffsets[currentBufferIndex] + offset;
		vkBufferMemoryBarrier.size					   = size;
		CStudioAPIDataUploaderVk& dataUploader		   = g_StudioAPIVk.GetDataUploader();
		dataUploader.Upload( dataSize, STUDIOAPI_VK_BUFFER_OFFSET_ALIGNMENT,
							 [this, offset, &vkBufferMemoryBarrier, bNeedOwnershipTransfer, pStudioAPICmdContext, pTransferCmdContext, pData, dataSize]( const CStudioAPIDataUploaderVk::uploadParams_t& uploadParams ) {
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
								 vkBufferCopy.dstOffset	   = bufferOffsets[currentBufferIndex] + offset + uploadOffset;
								 vkBufferCopy.size		   = uploadParams.partialUploadSize;
								 vkCmdCopyBuffer( uploadParams.pCmdBuffer->GetVkCommandBuffer(), uploadParams.vkStagingBuffer, vkBuffer, 1, &vkBufferCopy );

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
		MapMemory( dataSize, 0, mappedData );
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
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	studioAPIBufferMemoryBarrierVk_t bufferMemoryBarrier = {};
	bufferMemoryBarrier.vkBufferMemoryBarrier.buffer	 = vkBuffer;
	bufferMemoryBarrier.vkBufferMemoryBarrier.offset	 = bufferOffsets[currentBufferIndex];
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
	CStudioAPIBufferVk* pStudioAPIMultiBuffer			= (CStudioAPIBufferVk*)pUserData;
	pStudioAPIMultiBuffer->pStudioAPIVkShutdownDelegate = NULL;
	pStudioAPIMultiBuffer->~CStudioAPIBufferVk();
}