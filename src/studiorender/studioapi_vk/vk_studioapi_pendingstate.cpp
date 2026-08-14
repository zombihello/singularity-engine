#include "pch_studioapi_vk.h"
#include "studiorender/studioapi_vk/vk_studioapi_shader.h"
#include "studiorender/studioapi_vk/vk_studioapi_pendingstate.h"

/*
==================
CStudioAPIPendingComputeStateVk::CStudioAPIPendingComputeStateVk
==================
*/
CStudioAPIPendingComputeStateVk::CStudioAPIPendingComputeStateVk( CStudioAPICmdContextVk& cmdContext )
	: cmdContext( cmdContext )
{
}

/*
==================
CStudioAPIPendingComputeStateVk::PrepareForDispatch
==================
*/
void CStudioAPIPendingComputeStateVk::PrepareForDispatch( CStudioAPICmdListVk* pCmdList )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Flush all pending barriers
	cmdContext.FlushPendingBarriers( pCmdList );
	AssertMsg( false, "Need implement" );
}

/*
==================
CStudioAPIPendingRenderStateVk::CStudioAPIPendingRenderStateVk
==================
*/
CStudioAPIPendingRenderStateVk::CStudioAPIPendingRenderStateVk( CStudioAPICmdContextVk& cmdContext )
	: bScissorEnabled( false )
	, bDirtyVertexBuffers( false )
	, bDirtyIndexBuffer( false )
	, bDirtyPushConstants( false )
	, cmdContext( cmdContext )
	, pCurrentRenderPipeline( NULL )
	, pCurrentRenderDescriptorState( NULL )
{
	// Reset pending states
	Reset();
}

/*
==================
CStudioAPIPendingRenderStateVk::~CStudioAPIPendingRenderStateVk
==================
*/
CStudioAPIPendingRenderStateVk::~CStudioAPIPendingRenderStateVk()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	for ( auto it = descriptorStatesDict.begin(), itEnd = descriptorStatesDict.end(); it != itEnd; ++it )
	{
		delete it->second.pRenderDescriptorState;
		it->first->OnRenderPipelineDeleted().Unsubscribe( it->second.onRenderPipelineDeletedHandle );
	}
}

/*
==================
CStudioAPIPendingRenderStateVk::Reset
==================
*/
void CStudioAPIPendingRenderStateVk::Reset()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	pCurrentRenderPipeline		  = NULL;
	pCurrentRenderDescriptorState = NULL;
	bScissorEnabled				  = false;
	bDirtyVertexBuffers			  = false;
	bDirtyIndexBuffer			  = false;
	bDirtyPushConstants			  = false;
	indexBuffer.Clear();
	pushConstants.Clear();

	for ( uint32 index = 0; index < ARRAYSIZE( vertexBuffers ); ++index )
	{
		vertexBuffers[index].Clear();
	}
	Mem_Memzero( &vkViewport, sizeof( VkViewport ) );
	Mem_Memzero( &vkScissor, sizeof( VkRect2D ) );
}

/*
==================
CStudioAPIPendingRenderStateVk::SetVertexBuffer
==================
*/
void CStudioAPIPendingRenderStateVk::SetVertexBuffer( CStudioAPICmdListVk* pCmdList, uint32 slot, CStudioAPIBufferVk* pBuffer, uint64 offset )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( slot < ARRAYSIZE( vertexBuffers ) );
	vertexBuffer_t& vertexBuffer = vertexBuffers[slot];
	vertexBuffer_t	newVertexBuffer( pBuffer->GetVkBuffer(), (VkDeviceSize)( pBuffer->GetOffset() + offset ) );
	if ( vertexBuffer != newVertexBuffer )
	{
		// Update a synchronization state of the buffer
		pBuffer->UpdateSyncState( VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, cmdContext.GetQueue().GetQueueFamilyIndex() );

		// Update the vertex buffer state
		vertexBuffer		= newVertexBuffer;
		bDirtyVertexBuffers = true;
	}
}

/*
==================
CStudioAPIPendingRenderStateVk::SetVertexBufferUP
==================
*/
void CStudioAPIPendingRenderStateVk::SetVertexBufferUP( CStudioAPICmdListVk* pCmdList, uint32 slot, VkBuffer vkBuffer, VkDeviceSize offset )
{
	// The temp memory is host-written and persistently mapped, so there is no buffer object to track
	// a synchronization state on
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( slot < ARRAYSIZE( vertexBuffers ) );
	vertexBuffer_t& vertexBuffer = vertexBuffers[slot];
	vertexBuffer_t	newVertexBuffer( vkBuffer, offset );
	if ( vertexBuffer != newVertexBuffer )
	{
		// Update the vertex buffer state
		vertexBuffer		= newVertexBuffer;
		bDirtyVertexBuffers = true;
	}
}

/*
==================
CStudioAPIPendingRenderStateVk::SetIndexBuffer
==================
*/
void CStudioAPIPendingRenderStateVk::SetIndexBuffer( CStudioAPICmdListVk* pCmdList, CStudioAPIBufferVk* pBuffer, uint64 offset )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	indexBuffer_t newIndexBuffer( pBuffer->GetVkBuffer(), (VkDeviceSize)( pBuffer->GetOffset() + offset ), pBuffer->GetVkIndexType() );
	if ( indexBuffer != newIndexBuffer )
	{
		// Update a synchronization state of the buffer
		pBuffer->UpdateSyncState( VK_ACCESS_INDEX_READ_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, cmdContext.GetQueue().GetQueueFamilyIndex() );

		// Update the index buffer state
		indexBuffer		  = newIndexBuffer;
		bDirtyIndexBuffer = true;
	}
}

/*
==================
CStudioAPIPendingRenderStateVk::SetIndexBufferUP
==================
*/
void CStudioAPIPendingRenderStateVk::SetIndexBufferUP( CStudioAPICmdListVk* pCmdList, VkBuffer vkBuffer, VkDeviceSize offset, VkIndexType vkIndexType )
{
	// The temp memory is host-written and persistently mapped, so there is no buffer object to track
	// a synchronization state on
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	indexBuffer_t newIndexBuffer( vkBuffer, offset, vkIndexType );
	if ( indexBuffer != newIndexBuffer )
	{
		// Update the index buffer state
		indexBuffer		  = newIndexBuffer;
		bDirtyIndexBuffer = true;
	}
}

/*
==================
CStudioAPIPendingRenderStateVk::SetConstantBuffer
==================
*/
void CStudioAPIPendingRenderStateVk::SetConstantBuffer( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPIBufferVk* pConstantBuffer )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pCurrentRenderDescriptorState );
	pCurrentRenderDescriptorState->SetConstantBuffer( pCmdList, set, slot, pConstantBuffer );
}

/*
==================
CStudioAPIPendingRenderStateVk::SetTexture
==================
*/
void CStudioAPIPendingRenderStateVk::SetTexture( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPITextureVk* pTexture )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pCurrentRenderDescriptorState );
	pCurrentRenderDescriptorState->SetTexture( pCmdList, set, slot, pTexture );
}

/*
==================
CStudioAPIPendingRenderStateVk::SetSampler
==================
*/
void CStudioAPIPendingRenderStateVk::SetSampler( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPISamplerVk* pSampler )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pCurrentRenderDescriptorState );
	pCurrentRenderDescriptorState->SetSampler( pCmdList, set, slot, pSampler );
}

/*
==================
CStudioAPIPendingRenderStateVk::SetRenderPipeline
==================
*/
bool CStudioAPIPendingRenderStateVk::SetRenderPipeline( CStudioAPIRenderPipelineVk* pRenderPipeline, bool bForceReset )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Change a pipeline if different
	bool bChanged = bForceReset;
	if ( pCurrentRenderPipeline != pRenderPipeline )
	{
		pCurrentRenderPipeline = pRenderPipeline;

		// Find already created a descriptor state for the new pipeline
		auto it = descriptorStatesDict.find( pRenderPipeline );
		if ( it != descriptorStatesDict.end() )
		{
			pCurrentRenderDescriptorState = it->second.pRenderDescriptorState;
			Assert( pCurrentRenderDescriptorState->GetRenderPipeline() == pRenderPipeline );
		}
		// Otherwise allocate a new and subscribe on event OnRenderPipelineDeleted
		else
		{
			descriptorStateCache_t descriptorStateCache;
			descriptorStateCache.pRenderDescriptorState		   = new CStudioAPIDescriptorStateRenderVk( cmdContext, pRenderPipeline );
			descriptorStateCache.onRenderPipelineDeletedHandle = pRenderPipeline->OnRenderPipelineDeleted().Subscribe( &CStudioAPIPendingRenderStateVk::OndRenderPipelineDeleted, this );
			pCurrentRenderDescriptorState					   = descriptorStateCache.pRenderDescriptorState;
			descriptorStatesDict.insert( eastl::make_pair( pRenderPipeline, descriptorStateCache ) );
		}

		// Pipeline was changed
		bChanged = true;
	}

	// Reset the descriptor state if the pipeline was changed or bForceReset is TRUE
	if ( bChanged || bForceReset )
	{
		pCurrentRenderDescriptorState->Reset();
	}

	return bChanged;
}

/*
==================
CStudioAPIPendingRenderStateVk::UpdateDynamicStates
==================
*/
void CStudioAPIPendingRenderStateVk::UpdateDynamicStates( CStudioAPICmdListVk* pCmdList )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Update viewport if it need
	bool bNeedUpdateViewport = !pCmdList->HasViewport() || Mem_Memcmp( &vkViewport, &pCmdList->GetVkCurrentViewport(), sizeof( VkViewport ) ) != 0;
	if ( bNeedUpdateViewport )
	{
		vkCmdSetViewport( pCmdList->GetCmdBuffer()->GetVkCommandBuffer(), 0, 1, &vkViewport );
		pCmdList->MarkHasViewport( vkViewport );
	}

	// Update scissor
	bool bNeedUpdateScissor = !pCmdList->HasScissor() || Mem_Memcmp( &vkScissor, &pCmdList->GetVkCurrentScissor(), sizeof( VkRect2D ) ) != 0;
	if ( bNeedUpdateScissor )
	{
		vkCmdSetScissor( pCmdList->GetCmdBuffer()->GetVkCommandBuffer(), 0, 1, &vkScissor );
		pCmdList->MarkHasScissor( vkScissor );
	}

	// Mark that the buffer has dynamic states
	pCmdList->MarkHasDynamicStates();
}

/*
==================
CStudioAPIPendingRenderStateVk::PrepareForDraw
==================
*/
void CStudioAPIPendingRenderStateVk::PrepareForDraw( CStudioAPICmdListVk* pCmdList )
{
	// Update dynamic states
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	UpdateDynamicStates( pCmdList );

	// Update descriptor sets and bind it
	if ( pCurrentRenderDescriptorState->UpdateDescriptorSets( pCmdList ) )
	{
		pCurrentRenderDescriptorState->BindDescriptorSets( pCmdList );
	}

	// Push constants if the bound pipeline layout does declare a push-constant range
	if ( bDirtyPushConstants )
	{
		const CStudioAPIDescriptorSetsLayoutVk&	  descriptorSetsLayout = pCurrentRenderPipeline->GetBoundShaderState()->GetDescriptorSetsLayout();
		const eastl::vector<VkPushConstantRange>& vkPushConstantRanges = descriptorSetsLayout.GetVkPushConstantRanges();
		for ( uint32 index = 0, count = (uint32)vkPushConstantRanges.size(); index < count; ++index )
		{
			const VkPushConstantRange& vkPushConstantRange = vkPushConstantRanges[index];
			Assert( vkPushConstantRange.offset + vkPushConstantRange.size <= pushConstants.size );
			vkCmdPushConstants( pCmdList->GetCmdBuffer()->GetVkCommandBuffer(), descriptorSetsLayout.GetVkPipelineLayout(), vkPushConstantRange.stageFlags, vkPushConstantRange.offset, vkPushConstantRange.size, pushConstants.data + vkPushConstantRange.offset );
		}

		bDirtyPushConstants = false;
	}

	// Update vertex buffers if it need
	if ( bDirtyVertexBuffers )
	{
		VkBuffer	 vkVertexBuffers[STUDIOAPI_VK_MAX_VERTEX_ELEMENT_COUNT];
		VkDeviceSize vkVertexOffsets[STUDIOAPI_VK_MAX_VERTEX_ELEMENT_COUNT];
		uint32		 numUsedSlots = 0;
		for ( uint32 index = 0; index < STUDIOAPI_VK_MAX_VERTEX_ELEMENT_COUNT; ++index )
		{
			// Verify the vertex buffer is set
			const vertexBuffer_t& vertexBuffer = vertexBuffers[index];
			if ( vertexBuffer.IsValid() )
			{
				vkVertexBuffers[numUsedSlots] = vertexBuffer.vkBuffer;
				vkVertexOffsets[numUsedSlots] = vertexBuffer.offset;
				++numUsedSlots;
			}
		}

		if ( numUsedSlots > 0 )
		{
			// Bindings are expected to be in ascending order with no index gaps in between:
			// Correct:		0, 1, 2, 3
			// Incorrect:	1, 0, 2, 3
			// Incorrect:	0, 2, 3, 5
			vkCmdBindVertexBuffers( pCmdList->GetCmdBuffer()->GetVkCommandBuffer(), 0, numUsedSlots, vkVertexBuffers, vkVertexOffsets );
		}

		bDirtyVertexBuffers = false;
	}

	// Update index buffer if it need
	if ( bDirtyIndexBuffer )
	{
		vkCmdBindIndexBuffer( pCmdList->GetCmdBuffer()->GetVkCommandBuffer(), indexBuffer.vkBuffer, indexBuffer.offset, indexBuffer.vkIndexType );
		bDirtyIndexBuffer = false;
	}
}

/*
==================
CStudioAPIPendingRenderStateVk::OndRenderPipelineDeleted
==================
*/
void CStudioAPIPendingRenderStateVk::OndRenderPipelineDeleted( void* pUserData, CStudioAPIRenderPipelineVk* pRenderPipeline )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	CStudioAPIPendingRenderStateVk* pPendingRenderState = (CStudioAPIPendingRenderStateVk*)pUserData;
	auto							it					= pPendingRenderState->descriptorStatesDict.find( pRenderPipeline );
	if ( it != pPendingRenderState->descriptorStatesDict.end() )
	{
		delete it->second.pRenderDescriptorState;
		pPendingRenderState->descriptorStatesDict.erase( it );
	}

	if ( pPendingRenderState->pCurrentRenderPipeline == pRenderPipeline )
	{
		pPendingRenderState->pCurrentRenderPipeline		   = NULL;
		pPendingRenderState->pCurrentRenderDescriptorState = NULL;
	}
}
