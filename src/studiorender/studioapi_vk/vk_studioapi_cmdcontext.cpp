#include "pch_studioapi_vk.h"
#include "studiorender/studioapi_vk/vk_studioapi_pendingstate.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdcontext.h"

/*
==================
StudioAPIVk_TranslateQueueTypeToQueueFlags
==================
*/
FORCEINLINE uint8 StudioAPIVk_TranslateQueueTypeToQueueFlags( studioAPIQueueType_t queueType )
{
	switch ( queueType )
	{
	case STUDIOAPI_QUEUE_TYPE_GRAPHICS: return STUDIOAPI_VK_QUEUE_FLAG_GRAPHICS;
	case STUDIOAPI_QUEUE_TYPE_COMPUTE: return STUDIOAPI_VK_QUEUE_FLAG_COMPUTE;
	case STUDIOAPI_QUEUE_TYPE_TRANSFER: return STUDIOAPI_VK_QUEUE_FLAG_TRANSFER;
	default:
		AssertMsg( false, "Unknown StudioAPI queue type 0%X", queueType );
		return 0;
	}
}

/*
==================
CStudioAPICmdContextVk::CStudioAPICmdContextVk
==================
*/
CStudioAPICmdContextVk::CStudioAPICmdContextVk( CStudioAPIQueueVk& queue, uint8 supportQueueTypes )
	: supportQueueTypes( supportQueueTypes )
	, queue( queue )
	, cmdListBatchMgr( *this )
	, cmdBufferPools{ queue, queue, queue }
	, pPendingRenderState( NULL )
	, pPendingComputeState( NULL )
{
}

/*
==================
CStudioAPICmdContextVk::~CStudioAPICmdContextVk
==================
*/
CStudioAPICmdContextVk::~CStudioAPICmdContextVk()
{
	Shutdown();
}

/*
==================
CStudioAPICmdContextVk::Init
==================
*/
void CStudioAPICmdContextVk::Init()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Initialize groups of pending barriers
	Mem_Memzero( &pendingBufferBarriers, sizeof( pendingBufferBarriers ) );
	Mem_Memzero( &pendingImageBarriers, sizeof( pendingImageBarriers ) );
	Mem_Memzero( &pendingMemoryBarriers, sizeof( pendingMemoryBarriers ) );

	// Create pending state, contains pipeline states such as current shader and etc
	if ( HasSupportQueueTypes( STUDIOAPI_VK_QUEUE_FLAG_GRAPHICS ) )
	{
		pPendingRenderState = new CStudioAPIPendingRenderStateVk( *this );
	}
	if ( HasSupportQueueTypes( STUDIOAPI_VK_QUEUE_FLAG_COMPUTE ) )
	{
		pPendingComputeState = new CStudioAPIPendingComputeStateVk( *this );
	}

	// Initialize a command buffer pool for each frame in-flight
	for ( uint32 index = 0; index < STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT; ++index )
	{
		// Initialize a command buffer pool
		cmdBufferPools[index].Init();
	}

	// Initialize the command list batch manager
	cmdListBatchMgr.Init();
}

/*
==================
CStudioAPICmdContextVk::Shutdown
==================
*/
void CStudioAPICmdContextVk::Shutdown()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Destroy pending states
	if ( pPendingRenderState )
	{
		delete pPendingRenderState;
		pPendingRenderState = NULL;
	}

	if ( pPendingComputeState )
	{
		delete pPendingComputeState;
		pPendingComputeState = NULL;
	}

	// Shutdown the command list batch manager
	cmdListBatchMgr.Shutdown();

	// Destroy a command buffer pool for each frame in-flight
	for ( uint32 index = 0; index < STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT; ++index )
	{
		// Shutdown a command buffer pool
		cmdBufferPools[index].Shutdown();
	}

	// Clear all groups of pending barriers
	Mem_Memzero( &pendingBufferBarriers, sizeof( pendingBufferBarriers ) );
	Mem_Memzero( &pendingImageBarriers, sizeof( pendingImageBarriers ) );
	Mem_Memzero( &pendingMemoryBarriers, sizeof( pendingMemoryBarriers ) );
}

/*
==================
CStudioAPICmdContextVk::BeginFrame
==================
*/
void CStudioAPICmdContextVk::BeginFrame()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Free unused command buffers in current pool
	cmdBufferPools[g_StudioAPIVk.GetCurrentFrameInFlight()].FreeUnusedCmdBuffers();
}

/*
==================
CStudioAPICmdContextVk::EndFrame
==================
*/
void CStudioAPICmdContextVk::EndFrame()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Reset pending states
	if ( pPendingRenderState )
	{
		pPendingRenderState->Reset();
	}
	if ( pPendingComputeState )
	{
		pPendingComputeState->Reset();
	}

	// Clear submitted batches at the current frame
	cmdListBatchMgr.ClearSubmittedBatches();
}

/*
==================
CStudioAPICmdContextVk::SetRenderPipeline
==================
*/
void CStudioAPICmdContextVk::SetRenderPipeline( CStudioAPICmdListVk* pCmdList, CStudioAPIRenderPipelineVk* pRenderPipeline )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pPendingRenderState );
	if ( pPendingRenderState->SetRenderPipeline( pRenderPipeline, !pCmdList->HasPipeline() ) )
	{
		pPendingRenderState->BindRenderPipeline( pCmdList );
	}
}

/*
==================
CStudioAPICmdContextVk::SetVertexBuffer
==================
*/
void CStudioAPICmdContextVk::SetVertexBuffer( CStudioAPICmdListVk* pCmdList, uint32 slot, CStudioAPIBufferVk* pVertexBuffer, uint64 offset )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pPendingRenderState );
	pPendingRenderState->SetVertexBuffer( pCmdList, slot, pVertexBuffer, offset );
}

/*
==================
CStudioAPICmdContextVk::SetIndexBuffer
==================
*/
void CStudioAPICmdContextVk::SetIndexBuffer( CStudioAPICmdListVk* pCmdList, CStudioAPIBufferVk* pIndexBuffer, uint64 offset )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pPendingRenderState );
	pPendingRenderState->SetIndexBuffer( pCmdList, pIndexBuffer, offset );
}

/*
==================
CStudioAPICmdContextVk::SetVertexBufferUP
==================
*/
void CStudioAPICmdContextVk::SetVertexBufferUP( CStudioAPICmdListVk* pCmdList, uint32 slot, const byte* pVertexData, uint32 numVertices, uint32 vertexDataStride )
{
	// Grab a chunk of the frame's temp memory and copy the caller's vertices into it
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pPendingRenderState && pVertexData && numVertices > 0 && vertexDataStride > 0 );
	uint32		dataSize = numVertices * vertexDataStride;
	tempAlloc_t alloc	 = g_StudioAPIVk.GetTempAlloc().Alloc( dataSize, CStudioAPIMemoryMgrVk::GetBufferAlignmentFromVkUsageFlags( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT ) );
	Mem_Memcpy( alloc.pData, pVertexData, dataSize );
	pPendingRenderState->SetVertexBufferUP( pCmdList, slot, alloc.vkBuffer, alloc.offset );
}

/*
==================
CStudioAPICmdContextVk::SetIndexBufferUP
==================
*/
void CStudioAPICmdContextVk::SetIndexBufferUP( CStudioAPICmdListVk* pCmdList, const byte* pIndexData, uint32 numIndices, uint32 indexDataStride )
{
	// Grab a chunk of the frame's temp memory and copy the caller's indices into it
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pPendingRenderState && pIndexData && numIndices > 0 );
	AssertMsg( indexDataStride == sizeof( uint16 ) || indexDataStride == sizeof( uint32 ), "An index stride must be sizeof( uint16 ) or sizeof( uint32 ), got %i", indexDataStride );
	uint32		dataSize = numIndices * indexDataStride;
	tempAlloc_t alloc	 = g_StudioAPIVk.GetTempAlloc().Alloc( dataSize, CStudioAPIMemoryMgrVk::GetBufferAlignmentFromVkUsageFlags( VK_BUFFER_USAGE_INDEX_BUFFER_BIT ) );
	Mem_Memcpy( alloc.pData, pIndexData, dataSize );
	pPendingRenderState->SetIndexBufferUP( pCmdList, alloc.vkBuffer, alloc.offset, indexDataStride == sizeof( uint32 ) ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16 );
}

/*
==================
CStudioAPICmdContextVk::SetConstantBuffer
==================
*/
void CStudioAPICmdContextVk::SetConstantBuffer( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPIBufferVk* pConstantBuffer )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	pPendingRenderState->SetConstantBuffer( pCmdList, set, slot, pConstantBuffer );
}

/*
==================
CStudioAPICmdContextVk::SetPushConstants
==================
*/
void CStudioAPICmdContextVk::SetPushConstants( CStudioAPICmdListVk* pCmdList, byte* pData, uint32 dataSize )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	pPendingRenderState->SetPushConstants( pCmdList, pData, dataSize );
}

/*
==================
CStudioAPICmdContextVk::SetTexture
==================
*/
void CStudioAPICmdContextVk::SetTexture( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPITextureVk* pTexture )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	pPendingRenderState->SetTexture( pCmdList, set, slot, pTexture );
}

/*
==================
CStudioAPICmdContextVk::SetSampler
==================
*/
void CStudioAPICmdContextVk::SetSampler( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPISamplerVk* pSampler )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	pPendingRenderState->SetSampler( pCmdList, set, slot, pSampler );
}

/*
==================
CStudioAPICmdContextVk::SetViewport
==================
*/
void CStudioAPICmdContextVk::SetViewport( CStudioAPICmdListVk* pCmdList, float x, float y, float width, float height, float minDepth, float maxDepth )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pPendingRenderState );
	pPendingRenderState->SetViewport( x, y, width, height, minDepth, maxDepth );
}

/*
==================
CStudioAPICmdContextVk::SetScissor
==================
*/
void CStudioAPICmdContextVk::SetScissor( CStudioAPICmdListVk* pCmdList, int32 x, int32 y, uint32 width, uint32 height )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pPendingRenderState );
	pPendingRenderState->SetScissor( true, x, y, width, height );
}

/*
==================
CStudioAPICmdContextVk::Draw
==================
*/
void CStudioAPICmdContextVk::Draw( CStudioAPICmdListVk* pCmdList, uint32 baseVertexIndex, uint32 numVertices, uint32 numInstances /* = 1 */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pPendingRenderState );
	AssertMsg( pCmdList->HasPipeline() && pCmdList->GetState() == STUDIOAPI_VK_CMDLIST_STATE_IS_INSIDE_RENDER_PASS, "To draw must be set a render pass and a render pipeline" );
	pPendingRenderState->PrepareForDraw( pCmdList );
	vkCmdDraw( pCmdList->GetCmdBuffer()->GetVkCommandBuffer(), numVertices, numInstances, baseVertexIndex, 0 );
}

/*
==================
CStudioAPICmdContextVk::DrawIndexed
==================
*/
void CStudioAPICmdContextVk::DrawIndexed( CStudioAPICmdListVk* pCmdList, uint32 baseVertexIndex, uint32 baseIndex, uint32 numIndices, uint32 numInstances /* = 1 */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pPendingRenderState );
	AssertMsg( pCmdList->HasPipeline() && pCmdList->GetState() == STUDIOAPI_VK_CMDLIST_STATE_IS_INSIDE_RENDER_PASS, "To draw must be set a render pass and a render pipeline" );
	AssertMsg( pPendingRenderState->HasIndexBuffer(), "To indexed draw an index buffer must be set before" );
	pPendingRenderState->PrepareForDraw( pCmdList );
	vkCmdDrawIndexed( pCmdList->GetCmdBuffer()->GetVkCommandBuffer(), numIndices, numInstances, baseIndex, baseVertexIndex, 0 );
}

/*
==================
CStudioAPICmdContextVk::AddPendingBufferBarriers
==================
*/
void CStudioAPICmdContextVk::AddPendingBufferBarriers( CStudioAPICmdListVk* pCmdList, uint32 numBufferBarriers, const VkBufferMemoryBarrier* pVkBufferMemoryBarriers, VkPipelineStageFlags vkSrcStageMask, VkPipelineStageFlags vkDstStageMask )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// If the context hasn't support of render commands then unset flags which only for a render pipeline
	if ( !HasSupportQueueTypes( STUDIOAPI_VK_QUEUE_FLAG_GRAPHICS ) )
	{
		const VkPipelineStageFlags vkPipelineStagesRenderShaders = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
																   | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT
																   | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT
																   | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT
																   | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

		const VkPipelineStageFlags vkPipelineStagesRender = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
															| VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
															| VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		vkSrcStageMask &= ~vkPipelineStagesRenderShaders;
		vkDstStageMask &= ~vkPipelineStagesRenderShaders;

		if ( vkSrcStageMask & vkPipelineStagesRender )
		{
			vkSrcStageMask |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}
		if ( vkDstStageMask & vkPipelineStagesRender )
		{
			vkDstStageMask |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}
		vkSrcStageMask &= ~vkPipelineStagesRender;
		vkDstStageMask &= ~vkPipelineStagesRender;
	}

	// Flush pending barriers if we haven't enough space
	if ( pendingBufferBarriers.numPendingBarriers + numBufferBarriers > STUDIOAPI_VK_MAX_NUM_PENDING_BUFFER_BARRIERS )
	{
		FlushPendingBarriers( pCmdList );
	}

	// Immediately send the barrier down the pipeline if we have barriers more then STUDIOAPI_VK_MAX_NUM_PENDING_BUFFER_BARRIERS
	if ( numBufferBarriers > STUDIOAPI_VK_MAX_NUM_PENDING_BUFFER_BARRIERS )
	{
		vkCmdPipelineBarrier( pCmdList->GetCmdBuffer()->GetVkCommandBuffer(), vkSrcStageMask, vkDstStageMask, 0, 0, NULL, numBufferBarriers, pVkBufferMemoryBarriers, 0, NULL );
		return;
	}

	// Otherwise add a new pending barrier
	pendingBufferBarriers.vkSrcStageMask |= vkSrcStageMask;
	pendingBufferBarriers.vkDstStageMask |= vkDstStageMask;
	Mem_Memcpy( pendingBufferBarriers.vkPendingBarriers + pendingBufferBarriers.numPendingBarriers, pVkBufferMemoryBarriers, sizeof( VkBufferMemoryBarrier ) * numBufferBarriers );
	pendingBufferBarriers.numPendingBarriers += numBufferBarriers;
}

/*
==================
CStudioAPICmdContextVk::AddPendingImageBarriers
==================
*/
void CStudioAPICmdContextVk::AddPendingImageBarriers( CStudioAPICmdListVk* pCmdList, uint32 numImageBarriers, const VkImageMemoryBarrier* pVkImageMemoryBarrier, VkPipelineStageFlags vkSrcStageMask, VkPipelineStageFlags vkDstStageMask )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// If the context hasn't support of render commands then unset flags which only for a render pipeline
	if ( !HasSupportQueueTypes( STUDIOAPI_VK_QUEUE_FLAG_GRAPHICS ) )
	{
		const VkPipelineStageFlags vkPipelineStagesRenderShaders = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
																   | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT
																   | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT
																   | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT
																   | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

		const VkPipelineStageFlags vkPipelineStagesRender = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
															| VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
															| VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		vkSrcStageMask &= ~vkPipelineStagesRenderShaders;
		vkDstStageMask &= ~vkPipelineStagesRenderShaders;

		if ( vkSrcStageMask & vkPipelineStagesRender )
		{
			vkSrcStageMask |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}
		if ( vkDstStageMask & vkPipelineStagesRender )
		{
			vkDstStageMask |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}
		vkSrcStageMask &= ~vkPipelineStagesRender;
		vkDstStageMask &= ~vkPipelineStagesRender;
	}

	// Flush pending barriers if we haven't enough space
	if ( pendingImageBarriers.numPendingBarriers + numImageBarriers > STUDIOAPI_VK_MAX_NUM_PENDING_IMAGE_BARRIERS )
	{
		FlushPendingBarriers( pCmdList );
	}

	// Immediately send the barrier down the pipeline if we have barriers more then STUDIOAPI_VK_MAX_NUM_PENDING_IMAGE_BARRIERS
	if ( numImageBarriers > STUDIOAPI_VK_MAX_NUM_PENDING_IMAGE_BARRIERS )
	{
		vkCmdPipelineBarrier( pCmdList->GetCmdBuffer()->GetVkCommandBuffer(), vkSrcStageMask, vkDstStageMask, 0, 0, NULL, 0, NULL, numImageBarriers, pVkImageMemoryBarrier );
		return;
	}

	// Otherwise add a new pending barrier
	pendingImageBarriers.vkSrcStageMask |= vkSrcStageMask;
	pendingImageBarriers.vkDstStageMask |= vkDstStageMask;
	Mem_Memcpy( pendingImageBarriers.vkPendingBarriers + pendingImageBarriers.numPendingBarriers, pVkImageMemoryBarrier, sizeof( VkImageMemoryBarrier ) * numImageBarriers );
	pendingImageBarriers.numPendingBarriers += numImageBarriers;
}

/*
==================
CStudioAPICmdContextVk::AddPendingMemoryBarriers
==================
*/
void CStudioAPICmdContextVk::AddPendingMemoryBarriers( CStudioAPICmdListVk* pCmdList, uint32 numMemoryBarriers, const VkMemoryBarrier* pVkMemoryBarrier, VkPipelineStageFlags vkSrcStageMask, VkPipelineStageFlags vkDstStageMask )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// If the context hasn't support of render commands then unset flags which only for a render pipeline
	if ( !HasSupportQueueTypes( STUDIOAPI_VK_QUEUE_FLAG_GRAPHICS ) )
	{
		const VkPipelineStageFlags vkPipelineStagesRenderShaders = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
																   | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT
																   | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT
																   | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT
																   | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

		const VkPipelineStageFlags vkPipelineStagesRender = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
															| VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
															| VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		vkSrcStageMask &= ~vkPipelineStagesRenderShaders;
		vkDstStageMask &= ~vkPipelineStagesRenderShaders;

		if ( vkSrcStageMask & vkPipelineStagesRender )
		{
			vkSrcStageMask |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}
		if ( vkDstStageMask & vkPipelineStagesRender )
		{
			vkDstStageMask |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}
		vkSrcStageMask &= ~vkPipelineStagesRender;
		vkDstStageMask &= ~vkPipelineStagesRender;
	}

	// Flush pending barriers if we haven't enough space
	if ( pendingMemoryBarriers.numPendingBarriers + numMemoryBarriers > STUDIOAPI_VK_MAX_NUM_PENDING_MEMORY_BARRIERS )
	{
		FlushPendingBarriers( pCmdList );
	}

	// Immediately send the barrier down the pipeline if we have barriers more then STUDIOAPI_VK_MAX_NUM_PENDING_MEMORY_BARRIERS
	if ( numMemoryBarriers > STUDIOAPI_VK_MAX_NUM_PENDING_MEMORY_BARRIERS )
	{
		vkCmdPipelineBarrier( pCmdList->GetCmdBuffer()->GetVkCommandBuffer(), vkSrcStageMask, vkDstStageMask, 0, numMemoryBarriers, pVkMemoryBarrier, 0, NULL, 0, NULL );
		return;
	}

	// Otherwise add a new pending barrier
	pendingMemoryBarriers.vkSrcStageMask |= vkSrcStageMask;
	pendingMemoryBarriers.vkDstStageMask |= vkDstStageMask;
	Mem_Memcpy( pendingMemoryBarriers.vkPendingBarriers + pendingMemoryBarriers.numPendingBarriers, pVkMemoryBarrier, sizeof( VkMemoryBarrier ) * numMemoryBarriers );
	pendingMemoryBarriers.numPendingBarriers += numMemoryBarriers;
}

/*
==================
CStudioAPICmdContextVk::FlushPendingBarriers
==================
*/
void CStudioAPICmdContextVk::FlushPendingBarriers( CStudioAPICmdListVk* pCmdList )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	if ( pendingBufferBarriers.numPendingBarriers > 0 || pendingImageBarriers.numPendingBarriers > 0 || pendingMemoryBarriers.numPendingBarriers > 0 )
	{
		// Push a pipeline barrier into the command list
		vkCmdPipelineBarrier( pCmdList->GetCmdBuffer()->GetVkCommandBuffer(),
							  pendingBufferBarriers.vkSrcStageMask | pendingImageBarriers.vkSrcStageMask | pendingMemoryBarriers.vkSrcStageMask,
							  pendingBufferBarriers.vkDstStageMask | pendingImageBarriers.vkDstStageMask | pendingMemoryBarriers.vkDstStageMask,
							  0,
							  pendingMemoryBarriers.numPendingBarriers, pendingMemoryBarriers.vkPendingBarriers,
							  pendingBufferBarriers.numPendingBarriers, pendingBufferBarriers.vkPendingBarriers,
							  pendingImageBarriers.numPendingBarriers, pendingImageBarriers.vkPendingBarriers );

		// Reset pending barriers
		pendingBufferBarriers.vkSrcStageMask	 = VK_PIPELINE_STAGE_NONE;
		pendingBufferBarriers.vkDstStageMask	 = VK_PIPELINE_STAGE_NONE;
		pendingBufferBarriers.numPendingBarriers = 0;
		pendingImageBarriers.vkSrcStageMask		 = VK_PIPELINE_STAGE_NONE;
		pendingImageBarriers.vkDstStageMask		 = VK_PIPELINE_STAGE_NONE;
		pendingImageBarriers.numPendingBarriers	 = 0;
		pendingMemoryBarriers.vkSrcStageMask	 = VK_PIPELINE_STAGE_NONE;
		pendingMemoryBarriers.vkDstStageMask	 = VK_PIPELINE_STAGE_NONE;
		pendingMemoryBarriers.numPendingBarriers = 0;
	}
}

/*
==================
CStudioAPICmdContextVk::HasSupportQueueType
==================
*/
bool CStudioAPICmdContextVk::HasSupportQueueType( studioAPIQueueType_t queueType ) const
{
	return HasSupportQueueTypes( StudioAPIVk_TranslateQueueTypeToQueueFlags( queueType ) );
}
