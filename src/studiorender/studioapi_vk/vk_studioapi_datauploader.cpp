#include "pch_studioapi_vk.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdcontext.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdbuffer.h"
#include "studiorender/studioapi_vk/vk_studioapi_datauploader.h"

/*
==================
CStudioAPIDataUploaderVk::CStudioAPIDataUploaderVk
==================
*/
CStudioAPIDataUploaderVk::CStudioAPIDataUploaderVk()
	: currentStagingBufferIndex( 0 )
	, onStudioAPIVkShutdownHandle( INVALID_HANDLE )
{
}

/*
==================
CStudioAPIDataUploaderVk::~CStudioAPIDataUploaderVk
==================
*/
CStudioAPIDataUploaderVk::~CStudioAPIDataUploaderVk()
{
	Shutdown();
}

/*
==================
CStudioAPIDataUploaderVk::Init
==================
*/
void CStudioAPIDataUploaderVk::Init()
{
	// Allocate staging buffers
	for ( uint32 index = 0; index < NUM_STAGING_BUFFERS; ++index )
	{
		stagingBuffer_t&		stagingBuffer			= stagingBuffers[index];
		VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
		VkBufferCreateInfo		vkBufferCreateInfo		= {};
		vmaAllocationCreateInfo.usage					= VMA_MEMORY_USAGE_CPU_ONLY;
		vmaAllocationCreateInfo.flags					= VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT | VMA_ALLOCATION_CREATE_WITHIN_BUDGET_BIT;
		vkBufferCreateInfo.sType						= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vkBufferCreateInfo.size							= STAGING_BUFFER_SIZE;
		vkBufferCreateInfo.usage						= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		vkBufferCreateInfo.sharingMode					= VK_SHARING_MODE_EXCLUSIVE;

		stagingBuffer.vmaAllocation = g_StudioAPIVk.GetMemoryMgr().AllocateBuffer( S_Sprintf( "stagingbufferuploader %i", index ).c_str(), vkBufferCreateInfo, vmaAllocationCreateInfo, stagingBuffer.vkBuffer );
		if ( stagingBuffer.vmaAllocation == VK_NULL_HANDLE )
		{
			Sys_Error( "Failed to allocate GPU staging buffer with size %llu", vkBufferCreateInfo.size );
			return;
		}

		// Map the buffer memory and initialize a command list with a batch
		stagingBuffer.pBufferData	= g_StudioAPIVk.GetMemoryMgr().MapMemory<byte>( stagingBuffer.vmaAllocation );
		stagingBuffer.currentOffset = 0;
		stagingBuffer.pCmdListBatch = new CStudioAPICmdListBatchVk( (CStudioAPICmdContextVk*)g_StudioAPIVk.GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_TRANSFER ) );
		stagingBuffer.pCmdList		= new CStudioAPICmdListVk( (CStudioAPICmdContextVk*)g_StudioAPIVk.GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_TRANSFER ) );
		stagingBuffer.pCmdListBatch->AddCmdList( stagingBuffer.pCmdList );
	}

	// Register in 'onStudioAPIVkShutodwn' for destroy Vulkan objects when the one is shutdown
	onStudioAPIVkShutdownHandle = g_StudioAPIVk.OnStudioAPIVkShutdown().Subscribe( &CStudioAPIDataUploaderVk::OnStudioAPIVkShutdown, this );
}

/*
==================
CStudioAPIDataUploaderVk::Shutdown
==================
*/
void CStudioAPIDataUploaderVk::Shutdown()
{
	// Destroy allocated staging buffers
	for ( uint32 index = 0; index < NUM_STAGING_BUFFERS; ++index )
	{
		stagingBuffer_t& stagingBuffer = stagingBuffers[index];
		if ( stagingBuffer.vkBuffer == VK_NULL_HANDLE )
		{
			continue;
		}

		g_StudioAPIVk.GetMemoryMgr().UnmapMemory( stagingBuffer.vmaAllocation );
		g_StudioAPIVk.GetMemoryMgr().DestroyBuffer( stagingBuffer.vkBuffer, stagingBuffer.vmaAllocation );
		stagingBuffer.pBufferData	= NULL;
		stagingBuffer.vkBuffer		= VK_NULL_HANDLE;
		stagingBuffer.vmaAllocation = VK_NULL_HANDLE;
		stagingBuffer.pCmdListBatch = NULL;
		stagingBuffer.pCmdList		= NULL;
		stagingBuffer.currentOffset = 0;
	}

	// Remove CStudioAPIDataUploaderVk::OnStudioAPIVkShutdown from event 'onStudioAPIVkShutodwn'
	if ( onStudioAPIVkShutdownHandle != INVALID_HANDLE )
	{
		g_StudioAPIVk.OnStudioAPIVkShutdown().Unsubscribe( onStudioAPIVkShutdownHandle );
		onStudioAPIVkShutdownHandle = INVALID_HANDLE;
	}
}

/*
==================
CStudioAPIDataUploaderVk::SubmitCmdBuffer
==================
*/
void CStudioAPIDataUploaderVk::SubmitCmdBuffer( bool bWait /* = false */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Submit current staging buffer if it need
	stagingBuffer_t& stagingBuffer = stagingBuffers[currentStagingBufferIndex];
	if ( stagingBuffer.currentOffset > 0 )
	{
		// End record to the command buffer
		Assert( stagingBuffer.pCmdList->GetState() == STUDIOAPI_VK_CMDLIST_STATE_HAS_BEGUN_RECORD );
		stagingBuffer.pCmdList->EndRecord();

		// Submit the command buffer
		CStudioAPICmdContextVk* pCmdContext = (CStudioAPICmdContextVk*)stagingBuffer.pCmdList->GetCmdContext();
		pCmdContext->GetCmdListBatchMgr().Submit( stagingBuffer.pCmdListBatch );

		// Reset current offset
		stagingBuffer.currentOffset = 0;

		// Wait the current upload if it need
		if ( bWait )
		{
			WaitUpload( currentStagingBufferIndex );
		}
	}

	// Move to the next index of staging buffer
	currentStagingBufferIndex = ( currentStagingBufferIndex + 1 ) % NUM_STAGING_BUFFERS;
}

/*
==================
CStudioAPIDataUploaderVk::WaitUpload
==================
*/
void CStudioAPIDataUploaderVk::WaitUpload( uint8 stagingBufferIndex )
{
	CStudioAPICmdListBatchVk* pCmdListBatch = stagingBuffers[stagingBufferIndex].pCmdListBatch;
	CStudioAPICmdContextVk*	  pCmdContext	= (CStudioAPICmdContextVk*)pCmdListBatch->GetCmdContext();
	pCmdContext->GetCmdListBatchMgr().Wait( pCmdListBatch );
}

/*
==================
CStudioAPIDataUploaderVk::OnStudioAPIVkShutdown
==================
*/
void CStudioAPIDataUploaderVk::OnStudioAPIVkShutdown( void* pUserData )
{
	Assert( pUserData );
	CStudioAPIDataUploaderVk* pStudioAPIBufferUploaderVk	= (CStudioAPIDataUploaderVk*)pUserData;
	pStudioAPIBufferUploaderVk->onStudioAPIVkShutdownHandle = INVALID_HANDLE;
	pStudioAPIBufferUploaderVk->Shutdown();
}
