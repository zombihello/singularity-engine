#include "pch_studioapi_vk.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdcontext.h"
#include "studiorender/studioapi_vk/vk_studioapi_renderpass.h"
#include "studiorender/studioapi_vk/vk_studioapi_framebuffer.h"
#include "studiorender/studioapi_vk/vk_studioapi_renderpipeline.h"
#include "studiorender/studioapi_vk/vk_studioapi_buffer.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdbuffer.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdlist.h"
#include "studiorender/studioapi_vk/vk_studioapi_swapchain.h"
#include "studiorender/studioapi_vk/vk_studioapi_texture.h"

/*
==================
VK_TranslateBufferStateToVkAccessFlags
==================
*/
VkAccessFlags VK_TranslateBufferStateToVkAccessFlags( uint32 bufferState )
{
	Assert( ( bufferState & ~( STUDIOAPI_VK_BUFFER_KNOWN_STATES ) ) == 0 );
	VkAccessFlags vkAccessFlags = VK_ACCESS_NONE;

	// Shader resource
	if ( bufferState & STUDIOAPI_BUFFER_STATE_SHADER_RESOURCE )
	{
		AssertMsg( !( bufferState & STUDIOAPI_BUFFER_STATE_SHADER_RESOURCE_READONLY ), "Must be set either STUDIOAPI_BUFFER_STATE_SHADER_RESOURCE or STUDIOAPI_BUFFER_STATE_SHADER_RESOURCE_READONLY" );
		vkAccessFlags |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
	}

	// Shader resource read only
	if ( bufferState & STUDIOAPI_BUFFER_STATE_SHADER_RESOURCE_READONLY )
	{
		vkAccessFlags |= VK_ACCESS_SHADER_READ_BIT;
	}

	// Transfer source
	if ( bufferState & STUDIOAPI_BUFFER_STATE_TRANSFER_SRC )
	{
		vkAccessFlags |= VK_ACCESS_TRANSFER_READ_BIT;
	}

	// Transfer destination
	if ( bufferState & STUDIOAPI_BUFFER_STATE_TRANSFER_DST )
	{
		vkAccessFlags |= VK_ACCESS_TRANSFER_WRITE_BIT;
	}

	// Vertex buffer
	if ( bufferState & STUDIOAPI_BUFFER_STATE_VERTEX_BUFFER )
	{
		vkAccessFlags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	}

	// Index buffer
	if ( bufferState & STUDIOAPI_BUFFER_STATE_INDEX_BUFFER )
	{
		vkAccessFlags |= VK_ACCESS_INDEX_READ_BIT;
	}

	// Constant buffer
	if ( bufferState & STUDIOAPI_BUFFER_STATE_CONSTANT_BUFFER )
	{
		vkAccessFlags |= VK_ACCESS_UNIFORM_READ_BIT;
	}

	// Structured buffer
	if ( bufferState & STUDIOAPI_BUFFER_STATE_STRUCTURED_BUFFER )
	{
		vkAccessFlags |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
	}

	// We are done
	return vkAccessFlags;
}

/*
==================
CStudioAPICmdListVk::CStudioAPICmdListVk
==================
*/
CStudioAPICmdListVk::CStudioAPICmdListVk( CStudioAPICmdContextVk* pCmdContext )
	: bNeedsDynamicStateSet( true )
	, bHasPipeline( false )
	, bHasViewport( false )
	, bHasScissor( false )
	, state( STUDIOAPI_VK_CMDLIST_STATE_READY_FOR_RECORD )
	, pCmdContext( pCmdContext )
	, pCmdBuffer( NULL )
{
}

/*
==================
CStudioAPICmdListVk::~CStudioAPICmdListVk
==================
*/
CStudioAPICmdListVk::~CStudioAPICmdListVk()
{
}

/*
==================
CStudioAPICmdListVk::BeginRecord
==================
*/
void CStudioAPICmdListVk::BeginRecord()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Check the command list state, must be ready for begin
	Assert( state == STUDIOAPI_VK_CMDLIST_STATE_READY_FOR_RECORD );

	// Create a new command buffer and to begin record
	pCmdBuffer = pCmdContext->GetCmdBufferPool().CreateCmdBuffer();
	pCmdBuffer->Begin();

	// Mark that the command buffer needs dynamic state to set
	state				  = STUDIOAPI_VK_CMDLIST_STATE_HAS_BEGUN_RECORD;
	bNeedsDynamicStateSet = true;
	bHasPipeline		  = false;
	bHasViewport		  = false;
	bHasScissor			  = false;
	Mem_Memzero( &vkCurrentViewport, sizeof( VkViewport ) );
	Mem_Memzero( &vkCurrentScissor, sizeof( VkRect2D ) );
}

/*
==================
CStudioAPICmdListVk::EndRecord
==================
*/
void CStudioAPICmdListVk::EndRecord()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Flush all pending barriers and finish recording
	Assert( state == STUDIOAPI_VK_CMDLIST_STATE_HAS_BEGUN_RECORD );
	pCmdContext->FlushPendingBarriers( this );
	pCmdBuffer->End();

	// Reset the command list state
	state = STUDIOAPI_VK_CMDLIST_STATE_HAS_ENDED_RECORD;
}

/*
==================
CStudioAPICmdListVk::BeginRenderPass
==================
*/
void CStudioAPICmdListVk::BeginRenderPass( IStudioAPIRenderPass* pRenderPass, IStudioAPIFrameBuffer* pFrameBuffer )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	AssertMsg( state == STUDIOAPI_VK_CMDLIST_STATE_HAS_BEGUN_RECORD, "Can't to begin a new render pass as already inside one (pCmdList: 0x%p, state: 0x%X)", this, state );
	state = STUDIOAPI_VK_CMDLIST_STATE_IS_INSIDE_RENDER_PASS;

	// Flush all pending barriers before a render pass, because inside of one you can't make any barriers
	pCmdContext->FlushPendingBarriers( this );

	CStudioAPIRenderPassVk*	 pStudioAPIRenderPass  = (CStudioAPIRenderPassVk*)pRenderPass;
	CStudioAPIFrameBufferVk* pStudioAPIFrameBuffer = (CStudioAPIFrameBufferVk*)pFrameBuffer;
	vector2i_t					 frameBufferSize	   = pStudioAPIFrameBuffer->GetSize();
	VkRenderPassBeginInfo	 vkRenderPassBeginInfo = {};
	vkRenderPassBeginInfo.sType					   = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	vkRenderPassBeginInfo.renderPass			   = pStudioAPIRenderPass->GetVkRenderPass();
	vkRenderPassBeginInfo.framebuffer			   = pStudioAPIFrameBuffer->GetVkFrameBuffer();
	vkRenderPassBeginInfo.renderArea.offset		   = { 0, 0 };
	vkRenderPassBeginInfo.renderArea.extent.width  = frameBufferSize.x;
	vkRenderPassBeginInfo.renderArea.extent.height = frameBufferSize.y;
	vkRenderPassBeginInfo.clearValueCount		   = pStudioAPIFrameBuffer->GetNumClearValues();
	vkRenderPassBeginInfo.pClearValues			   = pStudioAPIFrameBuffer->GetVkClearValues();
	vkCmdBeginRenderPass( pCmdBuffer->GetVkCommandBuffer(), &vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE );
}

/*
==================
CStudioAPICmdListVk::EndRenderPass
==================
*/
void CStudioAPICmdListVk::EndRenderPass()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	AssertMsg( state == STUDIOAPI_VK_CMDLIST_STATE_IS_INSIDE_RENDER_PASS, "No rendering passes have begun yet (pCmdList: 0x%p, state: 0x%X)", this, state );
	vkCmdEndRenderPass( pCmdBuffer->GetVkCommandBuffer() );
	state = STUDIOAPI_VK_CMDLIST_STATE_HAS_BEGUN_RECORD;
}

/*
==================
CStudioAPICmdListVk::SetRenderPipeline
==================
*/
void CStudioAPICmdListVk::SetRenderPipeline( IStudioAPIRenderPipeline* pRenderPipeline )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	pCmdContext->SetRenderPipeline( this, (CStudioAPIRenderPipelineVk*)pRenderPipeline );
}

/*
==================
CStudioAPICmdListVk::SetVertexBuffer
==================
*/
void CStudioAPICmdListVk::SetVertexBuffer( uint32 slot, IStudioAPIBuffer* pVertexBuffer, uint64 offset )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	pCmdContext->SetVertexBuffer( this, slot, (CStudioAPIBufferVk*)pVertexBuffer, offset );
}

/*
==================
CStudioAPICmdListVk::SetConstantBuffer
==================
*/
void CStudioAPICmdListVk::SetConstantBuffer( uint32 set, uint32 slot, IStudioAPIBuffer* pConstantBuffer )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	pCmdContext->SetConstantBuffer( this, set, slot, (CStudioAPIBufferVk*)pConstantBuffer );
}

/*
==================
CStudioAPICmdListVk::SetViewport
==================
*/
void CStudioAPICmdListVk::SetViewport( float minX, float minY, float minZ, float maxX, float maxY, float maxZ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	pCmdContext->SetViewport( this, minX, minY, minZ, maxX, maxY, maxZ );
}

/*
==================
CStudioAPICmdListVk::SetScissor
==================
*/
void CStudioAPICmdListVk::SetScissor( int32 x, int32 y, uint32 width, uint32 height )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	pCmdContext->SetScissor( this, x, y, width, height );
}

/*
==================
CStudioAPICmdListVk::SetTexture
==================
*/
void CStudioAPICmdListVk::SetTexture( uint32 set, uint32 slot, IStudioAPITexture* pTexture )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	pCmdContext->SetTexture( this, set, slot, (CStudioAPITextureVk*)pTexture );
}

/*
==================
CStudioAPICmdListVk::SetSampler
==================
*/
void CStudioAPICmdListVk::SetSampler( uint32 set, uint32 slot, IStudioAPISampler* pSampler )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	pCmdContext->SetSampler( this, set, slot, (CStudioAPISamplerVk*)pSampler );
}

/*
==================
CStudioAPICmdListVk::Draw
==================
*/
void CStudioAPICmdListVk::Draw( uint32 baseVertexIndex, uint32 numVertices, uint32 numInstances /* = 1 */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	pCmdContext->Draw( this, baseVertexIndex, numVertices, numInstances );
}

/*
==================
CStudioAPICmdListVk::DrawIndexed
==================
*/
void CStudioAPICmdListVk::DrawIndexed( IStudioAPIBuffer* pIndexBuffer, uint32 baseVertexIndex, uint32 baseIndex, uint32 numIndices, uint32 numInstances /* = 1 */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	pCmdContext->DrawIndexed( this, (CStudioAPIBufferVk*)pIndexBuffer, baseVertexIndex, baseIndex, numIndices, numInstances );
}

/*
==================
CStudioAPICmdListVk::Barrier
==================
*/
void CStudioAPICmdListVk::Barrier( const studioAPIBarrier_t* pBarriers, uint32 numBarriers, bool bFlush /* = false */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	AssertMsg( state != STUDIOAPI_VK_CMDLIST_STATE_IS_INSIDE_RENDER_PASS, "Can't make a barrier inside of a render pass" );
	Assert( pBarriers );
	for ( uint32 index = 0; index < numBarriers; ++index )
	{
		// Skip a barrier if it isn't valid
		const studioAPIBarrier_t& studioAPIBarrier = pBarriers[index];
		if ( studioAPIBarrier.type == STUDIOAPI_BARRIER_TYPE_BUFFER && !studioAPIBarrier.buffer.pBuffer )
		{
			continue;
		}
		else if ( studioAPIBarrier.type == STUDIOAPI_BARRIER_TYPE_TEXTURE && !studioAPIBarrier.texture.pTexture )
		{
			continue;
		}

		// Make the barrier
		switch ( studioAPIBarrier.type )
		{
			// Memory barrier
		case STUDIOAPI_BARRIER_TYPE_MEMORY:
		{
			VkMemoryBarrier vkMemoryBarrier = {};
			vkMemoryBarrier.sType			= VK_STRUCTURE_TYPE_MEMORY_BARRIER;
			vkMemoryBarrier.srcAccessMask	= VK_ACCESS_MEMORY_WRITE_BIT;
			vkMemoryBarrier.dstAccessMask	= VK_ACCESS_MEMORY_READ_BIT;
			pCmdContext->AddPendingMemoryBarriers( this, 1, &vkMemoryBarrier, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT );
			break;
		}

			// Buffer barrier
		case STUDIOAPI_BARRIER_TYPE_BUFFER:
		{
			CStudioAPIBufferVk*	 pBuffer			 = (CStudioAPIBufferVk*)studioAPIBarrier.buffer.pBuffer;
			uint32				 dstQueueFamilyIndex = ( (CStudioAPICmdContextVk*)g_StudioAPIVk.GetImmediateCmdContext( studioAPIBarrier.buffer.ownerQueueType ) )->GetQueue().GetQueueFamilyIndex();
			VkAccessFlags		 vkDstAccessMask	 = VK_TranslateBufferStateToVkAccessFlags( studioAPIBarrier.buffer.state );
			VkPipelineStageFlags vkDstStageMask		 = VK_TranslateVkAccessFlagsToVkPipelineStageFlags( vkDstAccessMask );
			pBuffer->UpdateSyncStateWithBarrier( this, vkDstAccessMask, vkDstStageMask, dstQueueFamilyIndex );
			break;
		}

			// Texture barrier
		case STUDIOAPI_BARRIER_TYPE_TEXTURE:
		{
			uint32				 dstQueueFamilyIndex = ( (CStudioAPICmdContextVk*)g_StudioAPIVk.GetImmediateCmdContext( studioAPIBarrier.texture.ownerQueueType ) )->GetQueue().GetQueueFamilyIndex();
			VkImageLayout		 vkNewImageLayout	 = VK_TranslateTextureLayout( studioAPIBarrier.texture.layout );
			VkAccessFlags		 vkDstAccessMask	 = VK_TranslateVkImageLayoutToVkAccessFlags( vkNewImageLayout );
			VkPipelineStageFlags vkDstStageMask		 = VK_TranslateVkAccessFlagsToVkPipelineStageFlags( vkDstAccessMask );

			// An texture barrier
			if ( !studioAPIBarrier.texture.bSwapChain )
			{
				CStudioAPITextureVk*			  pTexture = (CStudioAPITextureVk*)studioAPIBarrier.texture.pTexture;
				CStudioAPIImageSubresourceRangeVk subresourceRange;
				if ( studioAPIBarrier.texture.mip == -1 && studioAPIBarrier.texture.layer == -1 )
				{
					subresourceRange = CStudioAPIImageSubresourceRangeVk::CreateForAllSubresources();
				}
				else
				{
					bool bAllMips	 = studioAPIBarrier.texture.mip == -1;
					bool bAllLayers	 = studioAPIBarrier.texture.layer == -1;
					subresourceRange = CStudioAPIImageSubresourceRangeVk::CreateForSubresources( !bAllMips ? studioAPIBarrier.texture.mip : 0,
																								 !bAllMips ? 1 : pTexture->GetNumMips(),
																								 !bAllLayers ? studioAPIBarrier.texture.layer : 0,
																								 !bAllLayers ? 1 : pTexture->GetNumLayers() );
				}
				pTexture->UpdateSyncStateWithBarrier( this, subresourceRange, vkNewImageLayout, vkDstAccessMask, vkDstStageMask, dstQueueFamilyIndex );
			}
			// Otherwise it is an swap chain image barrier
			else
			{
				CStudioAPISwapChainImageVk* pSwapChainImage = (CStudioAPISwapChainImageVk*)studioAPIBarrier.texture.pImage;
				pSwapChainImage->UpdateSyncStateWithBarrier( this, vkNewImageLayout, vkDstAccessMask, vkDstStageMask, dstQueueFamilyIndex );
			}
			break;
		}

		default:
			AssertMsg( false, "Unknown StudioAPI barrier type 0x%X", studioAPIBarrier.type );
			break;
		}
	}

	// Flush all pending barriers if it need
	if ( bFlush )
	{
		pCmdContext->FlushPendingBarriers( this );
	}
}

/*
==================
CStudioAPICmdListVk::CopyBuffer
==================
*/
void CStudioAPICmdListVk::CopyBuffer( IStudioAPIBuffer* pSrcBuffer, uint64 srcOffset, IStudioAPIBuffer* pDstBuffer, uint64 dstOffset, uint64 size )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	CStudioAPIBufferVk* pStudioAPISrcBuffer = (CStudioAPIBufferVk*)pSrcBuffer;
	CStudioAPIBufferVk* pStudioAPIDstBuffer = (CStudioAPIBufferVk*)pDstBuffer;
	AssertMsg( state != STUDIOAPI_VK_CMDLIST_STATE_IS_INSIDE_RENDER_PASS, "Can't copy inside of a render pass" );
	Assert( pStudioAPISrcBuffer->GetUsageFlags() & STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_SRC );
	Assert( pStudioAPIDstBuffer->GetUsageFlags() & STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST );

	// Flush all pending barriers before copy
	pCmdContext->FlushPendingBarriers( this );

	VkBufferCopy vkBufferCopy = {};
	vkBufferCopy.srcOffset	  = pStudioAPISrcBuffer->GetOffset() + srcOffset;
	vkBufferCopy.dstOffset	  = pStudioAPIDstBuffer->GetOffset() + dstOffset;
	vkBufferCopy.size		  = size;
	vkCmdCopyBuffer( pCmdBuffer->GetVkCommandBuffer(), pStudioAPISrcBuffer->GetVkBuffer(), pStudioAPIDstBuffer->GetVkBuffer(), 1, &vkBufferCopy );

	pStudioAPISrcBuffer->UpdateSyncState( VK_ACCESS_TRANSFER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, pCmdContext->GetQueue().GetQueueFamilyIndex() );
	pStudioAPIDstBuffer->UpdateSyncState( VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, pCmdContext->GetQueue().GetQueueFamilyIndex() );
	pStudioAPIDstBuffer->SwapCurrentBufferIndex();
}

/*
==================
CStudioAPICmdListVk::CopyBufferToTexture
==================
*/
void CStudioAPICmdListVk::CopyBufferToTexture( IStudioAPIBuffer* pSrcBuffer, uint64 srcOffset, IStudioAPITexture* pDstTexture, uint32 dstMip, const vector3i_t& dstOffset, const vector3i_t& dstSize, uint32 dstStartLayer /* = 0 */, uint32 dstNumLayers /* = -1 */, uint32 srcRowLength /* = 0 */, uint32 srcTextureHeight /* = 0 */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	CStudioAPIBufferVk*	 pStudioAPISrcBuffer  = (CStudioAPIBufferVk*)pSrcBuffer;
	CStudioAPITextureVk* pStudioAPIDstTexture = (CStudioAPITextureVk*)pDstTexture;
	AssertMsg( state != STUDIOAPI_VK_CMDLIST_STATE_IS_INSIDE_RENDER_PASS, "Can't copy inside of a render pass" );
	Assert( pStudioAPISrcBuffer->GetUsageFlags() & STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_SRC );
	Assert( pStudioAPIDstTexture->GetUsageFlags() & STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_DST );

	// Flush all pending barriers before copy
	pCmdContext->FlushPendingBarriers( this );

	VkBufferImageCopy vkBufferImageCopy				  = {};
	vkBufferImageCopy.bufferOffset					  = pStudioAPISrcBuffer->GetOffset() + srcOffset;
	vkBufferImageCopy.bufferRowLength				  = srcRowLength;
	vkBufferImageCopy.bufferImageHeight				  = srcTextureHeight;
	vkBufferImageCopy.imageSubresource.aspectMask	  = pStudioAPIDstTexture->GetVkImageAspectFlags();
	vkBufferImageCopy.imageSubresource.mipLevel		  = dstMip;
	vkBufferImageCopy.imageSubresource.baseArrayLayer = dstStartLayer;
	vkBufferImageCopy.imageSubresource.layerCount	  = dstNumLayers != -1 ? dstNumLayers : pStudioAPIDstTexture->GetNumLayers();
	vkBufferImageCopy.imageOffset.x					  = dstOffset.x;
	vkBufferImageCopy.imageOffset.y					  = dstOffset.y;
	vkBufferImageCopy.imageOffset.z					  = dstOffset.z;
	vkBufferImageCopy.imageExtent.width				  = dstSize.x;
	vkBufferImageCopy.imageExtent.height			  = dstSize.y;
	vkBufferImageCopy.imageExtent.depth				  = dstSize.z;
	vkCmdCopyBufferToImage( pCmdBuffer->GetVkCommandBuffer(), pStudioAPISrcBuffer->GetVkBuffer(), pStudioAPIDstTexture->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vkBufferImageCopy );

	pStudioAPISrcBuffer->UpdateSyncState( VK_ACCESS_TRANSFER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, pCmdContext->GetQueue().GetQueueFamilyIndex() );
	pStudioAPIDstTexture->UpdateSyncState( CStudioAPIImageSubresourceRangeVk::CreateForSubresources( vkBufferImageCopy.imageSubresource.mipLevel, 1,
																									 vkBufferImageCopy.imageSubresource.baseArrayLayer,
																									 vkBufferImageCopy.imageSubresource.layerCount ),
										   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
										   pCmdContext->GetQueue().GetQueueFamilyIndex() );
}

/*
==================
CStudioAPICmdListVk::CopyTextureToBuffer
==================
*/
void CStudioAPICmdListVk::CopyTextureToBuffer( IStudioAPITexture* pSrcTexture, uint32 srcMip, const vector3i_t& srcOffset, const vector3i_t& srcSize, IStudioAPIBuffer* pDstBuffer, uint64 dstOffset, uint32 srcStartLayer /* = 0 */, uint32 srcNumLayers /* = -1 */, uint32 dstRowLength /* = 0 */, uint32 dstTextureHeight /* = 0 */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	CStudioAPITextureVk* pStudioAPISrcTexture = (CStudioAPITextureVk*)pSrcTexture;
	CStudioAPIBufferVk*	 pStudioAPIDstBuffer  = (CStudioAPIBufferVk*)pDstBuffer;
	AssertMsg( state != STUDIOAPI_VK_CMDLIST_STATE_IS_INSIDE_RENDER_PASS, "Can't copy inside of a render pass" );
	Assert( pStudioAPISrcTexture->GetUsageFlags() & STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_DST );
	Assert( pStudioAPIDstBuffer->GetUsageFlags() & STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_SRC );

	// Flush all pending barriers before copy
	pCmdContext->FlushPendingBarriers( this );

	VkBufferImageCopy vkBufferImageCopy				  = {};
	vkBufferImageCopy.bufferOffset					  = pStudioAPIDstBuffer->GetOffset() + dstOffset;
	vkBufferImageCopy.bufferRowLength				  = dstRowLength;
	vkBufferImageCopy.bufferImageHeight				  = dstTextureHeight;
	vkBufferImageCopy.imageSubresource.aspectMask	  = pStudioAPISrcTexture->GetVkImageAspectFlags();
	vkBufferImageCopy.imageSubresource.mipLevel		  = srcMip;
	vkBufferImageCopy.imageSubresource.baseArrayLayer = srcStartLayer;
	vkBufferImageCopy.imageSubresource.layerCount	  = srcNumLayers != -1 ? srcNumLayers : pStudioAPISrcTexture->GetNumLayers();
	vkBufferImageCopy.imageOffset.x					  = srcOffset.x;
	vkBufferImageCopy.imageOffset.y					  = srcOffset.y;
	vkBufferImageCopy.imageOffset.z					  = srcOffset.z;
	vkBufferImageCopy.imageExtent.width				  = srcSize.x;
	vkBufferImageCopy.imageExtent.height			  = srcSize.y;
	vkBufferImageCopy.imageExtent.depth				  = srcSize.z;
	vkCmdCopyImageToBuffer( pCmdBuffer->GetVkCommandBuffer(), pStudioAPISrcTexture->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pStudioAPIDstBuffer->GetVkBuffer(), 1, &vkBufferImageCopy );

	pStudioAPISrcTexture->UpdateSyncState( CStudioAPIImageSubresourceRangeVk::CreateForSubresources( vkBufferImageCopy.imageSubresource.mipLevel, 1,
																									 vkBufferImageCopy.imageSubresource.baseArrayLayer,
																									 vkBufferImageCopy.imageSubresource.layerCount ),
										   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_ACCESS_TRANSFER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
										   pCmdContext->GetQueue().GetQueueFamilyIndex() );
	pStudioAPIDstBuffer->UpdateSyncState( VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, pCmdContext->GetQueue().GetQueueFamilyIndex() );
}

/*
==================
CStudioAPICmdListVk::CopyTexture
==================
*/
void CStudioAPICmdListVk::CopyTexture( IStudioAPITexture* pSrcTexture, uint32 srcMip, const vector3i_t srcOffset, IStudioAPITexture* pDstTexture, uint32 dstMip, const vector3i_t dstOffset, const vector3i_t size, uint32 srcStartLayer /* = 0 */, uint32 dstStartLayer /* = 0 */, uint32 numLayers /* = 1 */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	CStudioAPITextureVk* pStudioAPISrcTexture = (CStudioAPITextureVk*)pSrcTexture;
	CStudioAPITextureVk* pStudioAPIDstTexture = (CStudioAPITextureVk*)pDstTexture;
	AssertMsg( state != STUDIOAPI_VK_CMDLIST_STATE_IS_INSIDE_RENDER_PASS, "Can't copy inside of a render pass" );
	Assert( pStudioAPISrcTexture->GetUsageFlags() & STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_SRC );
	Assert( pStudioAPIDstTexture->GetUsageFlags() & STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_DST );

	// Flush all pending barriers before copy
	pCmdContext->FlushPendingBarriers( this );

	VkImageCopy vkImageCopy					  = {};
	vkImageCopy.srcSubresource.aspectMask	  = pStudioAPISrcTexture->GetVkImageAspectFlags();
	vkImageCopy.srcSubresource.mipLevel		  = srcMip;
	vkImageCopy.srcSubresource.baseArrayLayer = srcStartLayer;
	vkImageCopy.srcSubresource.layerCount	  = numLayers;
	vkImageCopy.srcOffset.x					  = srcOffset.x;
	vkImageCopy.srcOffset.y					  = srcOffset.y;
	vkImageCopy.srcOffset.z					  = srcOffset.z;
	vkImageCopy.dstSubresource.aspectMask	  = pStudioAPIDstTexture->GetVkImageAspectFlags();
	vkImageCopy.dstSubresource.mipLevel		  = dstMip;
	vkImageCopy.dstSubresource.baseArrayLayer = dstStartLayer;
	vkImageCopy.dstSubresource.layerCount	  = numLayers;
	vkImageCopy.dstOffset.x					  = dstOffset.x;
	vkImageCopy.dstOffset.y					  = dstOffset.y;
	vkImageCopy.dstOffset.z					  = dstOffset.z;
	vkImageCopy.extent.width				  = size.x;
	vkImageCopy.extent.height				  = size.y;
	vkImageCopy.extent.depth				  = size.z;
	vkCmdCopyImage( pCmdBuffer->GetVkCommandBuffer(), pStudioAPISrcTexture->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pStudioAPIDstTexture->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vkImageCopy );

	pStudioAPISrcTexture->UpdateSyncState( CStudioAPIImageSubresourceRangeVk::CreateForSubresources( vkImageCopy.srcSubresource.mipLevel, 1,
																									 vkImageCopy.srcSubresource.baseArrayLayer,
																									 vkImageCopy.srcSubresource.layerCount ),
										   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_ACCESS_TRANSFER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
										   pCmdContext->GetQueue().GetQueueFamilyIndex() );
	pStudioAPIDstTexture->UpdateSyncState( CStudioAPIImageSubresourceRangeVk::CreateForSubresources( vkImageCopy.dstSubresource.mipLevel, 1,
																									 vkImageCopy.dstSubresource.baseArrayLayer,
																									 vkImageCopy.dstSubresource.layerCount ),
										   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
										   pCmdContext->GetQueue().GetQueueFamilyIndex() );
}

/*
==================
CStudioAPICmdListVk::GetCmdContext
==================
*/
IStudioAPICmdContext* CStudioAPICmdListVk::GetCmdContext() const
{
	return pCmdContext;
}

/*
==================
CStudioAPICmdListBatchVk::CStudioAPICmdListBatchVk
==================
*/
CStudioAPICmdListBatchVk::CStudioAPICmdListBatchVk( CStudioAPICmdContextVk* pCmdContext )
	: syncSwapChainFlags( 0 )
	, pSyncSwapChain( NULL )
	, pCmdContext( pCmdContext )
{
}

/*
==================
CStudioAPICmdListBatchVk::~CStudioAPICmdListBatchVk
==================
*/
CStudioAPICmdListBatchVk::~CStudioAPICmdListBatchVk()
{
	RemoveAllSyncSignals();
	RemoveAllSyncWaits();
}

/*
==================
CStudioAPICmdListBatchVk::AddCmdList
==================
*/
void CStudioAPICmdListBatchVk::AddCmdList( IStudioAPICmdList* pCmdList )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	CStudioAPICmdListVk* pStudioAPICmdList = (CStudioAPICmdListVk*)pCmdList;
	Assert( pStudioAPICmdList->GetCmdContext() == pCmdContext );
	Assert( eastl::find( cmdLists.begin(), cmdLists.end(), pStudioAPICmdList ) == cmdLists.end() );
	cmdLists.emplace_back( pStudioAPICmdList );
}

/*
==================
CStudioAPICmdListBatchVk::RemoveCmdList
==================
*/
void CStudioAPICmdListBatchVk::RemoveCmdList( IStudioAPICmdList* pCmdList )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	CStudioAPICmdListVk* pStudioAPICmdList = (CStudioAPICmdListVk*)pCmdList;
	Assert( pStudioAPICmdList->GetCmdContext() == pCmdContext );
	for ( uint32 index = 0, count = (uint32)cmdLists.size(); index < count; ++index )
	{
		CStudioAPICmdListVk* pCurrentCmdList = cmdLists[index];
		if ( pCurrentCmdList == pCmdList )
		{
			cmdLists.erase( cmdLists.begin() + index );
			return;
		}
	}
}

/*
==================
CStudioAPICmdListBatchVk::RemoveAllCmdLists
==================
*/
void CStudioAPICmdListBatchVk::RemoveAllCmdLists()
{
	cmdLists.clear();
}

/*
==================
CStudioAPICmdListBatchVk::SyncSignal
==================
*/
void CStudioAPICmdListBatchVk::SyncSignal( const char* pSyncName )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pSyncName && pSyncName != "" );
	syncSignals.emplace_back( g_StudioAPIVk.GetSyncMgr().FindOrCreateNamedSemaphore( pSyncName ) );
}

/*
==================
CStudioAPICmdListBatchVk::SyncWait
==================
*/
void CStudioAPICmdListBatchVk::SyncWait( const char* pSyncName )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pSyncName && pSyncName != "" );
	syncWaits.emplace_back( g_StudioAPIVk.GetSyncMgr().FindOrCreateNamedSemaphore( pSyncName ) );
}

/*
==================
CStudioAPICmdListBatchVk::SyncSwapChain
==================
*/
void CStudioAPICmdListBatchVk::SyncSwapChain( IStudioAPISwapChain* pSwapChain, uint8 syncFlags )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	if ( pSwapChain )
	{
		AssertMsg( syncFlags != 0, "Can't be zero if pSwapChain isn't NULL" );
		pSyncSwapChain	   = (CStudioAPISwapChainVk*)pSwapChain;
		syncSwapChainFlags = syncFlags;
	}
	else
	{
		pSwapChain = NULL;
	}
}

/*
==================
CStudioAPICmdListBatchVk::RemoveSyncSignal
==================
*/
void CStudioAPICmdListBatchVk::RemoveSyncSignal( const char* pSyncName )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	for ( auto it = syncSignals.begin(), itEnd = syncSignals.end(); it != itEnd; ++it )
	{
		CStudioAPINamedSemaphoreVk* pNamedSemaphore = *it;
		if ( pNamedSemaphore->GetName() == pSyncName )
		{
			g_StudioAPIVk.GetSyncMgr().ReleaseNamedSemaphore( pNamedSemaphore );
			syncSignals.erase( it );
			return;
		}
	}
}

/*
==================
CStudioAPICmdListBatchVk::RemoveSyncWait
==================
*/
void CStudioAPICmdListBatchVk::RemoveSyncWait( const char* pSyncName )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	for ( auto it = syncWaits.begin(), itEnd = syncWaits.end(); it != itEnd; ++it )
	{
		CStudioAPINamedSemaphoreVk* pNamedSemaphore = *it;
		if ( pNamedSemaphore->GetName() == pSyncName )
		{
			g_StudioAPIVk.GetSyncMgr().ReleaseNamedSemaphore( pNamedSemaphore );
			syncWaits.erase( it );
			return;
		}
	}
}

/*
==================
CStudioAPICmdListBatchVk::RemoveSyncSwapChain
==================
*/
void CStudioAPICmdListBatchVk::RemoveSyncSwapChain()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	pSyncSwapChain	   = NULL;
	syncSwapChainFlags = 0;
}

/*
==================
CStudioAPICmdListBatchVk::RemoveAllSyncSignals
==================
*/
void CStudioAPICmdListBatchVk::RemoveAllSyncSignals()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	for ( auto it = syncSignals.begin(), itEnd = syncSignals.end(); it != itEnd; ++it )
	{
		g_StudioAPIVk.GetSyncMgr().ReleaseNamedSemaphore( *it );
	}
	syncSignals.clear();
}

/*
==================
CStudioAPICmdListBatchVk::RemoveAllSyncWaits
==================
*/
void CStudioAPICmdListBatchVk::RemoveAllSyncWaits()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	for ( auto it = syncWaits.begin(), itEnd = syncWaits.end(); it != itEnd; ++it )
	{
		g_StudioAPIVk.GetSyncMgr().ReleaseNamedSemaphore( *it );
	}
	syncWaits.clear();
}

/*
==================
CStudioAPICmdListBatchVk::GetCmdContext
==================
*/
IStudioAPICmdContext* CStudioAPICmdListBatchVk::GetCmdContext() const
{
	return pCmdContext;
}

/*
==================
CStudioAPICmdListBatchVk::GetNumCmdLists
==================
*/
uint32 CStudioAPICmdListBatchVk::GetNumCmdLists() const
{
	return (uint32)cmdLists.size();
}

/*
==================
CStudioAPICmdListBatchVk::GetCmdLists
==================
*/
IStudioAPICmdList** CStudioAPICmdListBatchVk::GetCmdLists() const
{
	return (IStudioAPICmdList**)cmdLists.data();
}

/*
==================
CStudioAPICmdListBatchMgrVk::CStudioAPICmdListBatchMgrVk
==================
*/
CStudioAPICmdListBatchMgrVk::CStudioAPICmdListBatchMgrVk( CStudioAPICmdContextVk& cmdContext )
	: cmdContext( cmdContext )
{
}

/*
==================
CStudioAPICmdListBatchMgrVk::~CStudioAPICmdListBatchMgrVk
==================
*/
CStudioAPICmdListBatchMgrVk::~CStudioAPICmdListBatchMgrVk()
{
	Shutdown();
}

/*
==================
CStudioAPICmdListBatchMgrVk::Init
==================
*/
void CStudioAPICmdListBatchMgrVk::Init()
{
}

/*
==================
CStudioAPICmdListBatchMgrVk::Shutdown
==================
*/
void CStudioAPICmdListBatchMgrVk::Shutdown()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Clear all submitted batches at current frame in-flight
	ClearSubmittedBatches();
}

/*
==================
CStudioAPICmdListBatchMgrVk::Submit
==================
*/
void CStudioAPICmdListBatchMgrVk::Submit( CStudioAPICmdListBatchVk* pCmdListBatch, bool bWait /* = false */, uint64 waitTime /* = 0 */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Check that the batch has same queue type as the command context
	Assert( pCmdListBatch->GetCmdContext() == &cmdContext );

	// Grab from the batch all synchronization signals, waits and command buffers
	eastl::vector<VkSemaphore>		  vkSignalSemaphores;
	eastl::vector<VkSemaphore>		  vkWaitSemaphores;
	eastl::vector<VkPipelineStageFlags> vkWaitStageMasks;
	eastl::vector<VkCommandBuffer>	  vkCommandBuffers;
	GrabVkSyncSignalsFromBatch( pCmdListBatch, vkSignalSemaphores );
	GrabVkSyncWaitsFromBatch( pCmdListBatch, vkWaitSemaphores, vkWaitStageMasks );
	GrabVkCmdBuffersFromBatch( pCmdListBatch, vkCommandBuffers );

	// Initialize VkSubmitInfo and submit the batch into GPU
	CStudioAPIFenceVk* pFence		  = g_StudioAPIVk.GetSyncMgr().CreateFence();
	VkSubmitInfo	   vkSubmitInfo	  = {};
	vkSubmitInfo.sType				  = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	vkSubmitInfo.waitSemaphoreCount	  = (uint32)vkWaitSemaphores.size();
	vkSubmitInfo.pWaitSemaphores	  = vkWaitSemaphores.data();
	vkSubmitInfo.pWaitDstStageMask	  = vkWaitStageMasks.data();
	vkSubmitInfo.commandBufferCount	  = (uint32)vkCommandBuffers.size();
	vkSubmitInfo.pCommandBuffers	  = vkCommandBuffers.data();
	vkSubmitInfo.signalSemaphoreCount = (uint32)vkSignalSemaphores.size();
	vkSubmitInfo.pSignalSemaphores	  = vkSignalSemaphores.data();
	cmdContext.GetQueue().Submit( &vkSubmitInfo, 1, pFence );

	// Wait the batch if it need
	bool bWaitResult = false;
	if ( bWait )
	{
		bWaitResult = pFence->Wait( waitTime != 0 ? ( uint64 )( waitTime * 1e9 ) : UINT64_MAX );
		if ( bWaitResult )
		{
			g_StudioAPIVk.GetSyncMgr().ReleaseFence( pFence );
			pFence = NULL;
		}
	}

	// Otherwise if no need wait the batch or the result is false
	// add the fence into the dictionary
	if ( !bWait || !bWaitResult )
	{
		// Add the fence frame in-flight if need it
		if ( pCmdListBatch->pSyncSwapChain && pCmdListBatch->syncSwapChainFlags & STUDIOAPI_SYNC_SWAPCHAIN_FLAGS_PRESENT_TO_IMAGE )
		{
			g_StudioAPIVk.GetSyncMgr().AddFrameInFlightFence( pFence, g_StudioAPIVk.GetCurrentFrameInFlight() );
		}

		// Add the fence into the dictionary
		submittedBatchesDict.insert( eastl::make_pair( pCmdListBatch, pFence ) );
	}
}

/*
==================
CStudioAPICmdListBatchMgrVk::Wait
==================
*/
bool CStudioAPICmdListBatchMgrVk::Wait( CStudioAPICmdListBatchVk* pCmdListBatch, uint64 waitTime /* = 0 */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_WAIT );
	auto it = submittedBatchesDict.find( pCmdListBatch );
	if ( it != submittedBatchesDict.end() )
	{
		bool bResult = it->second->Wait( waitTime != 0 ? ( uint64 )( waitTime * 1e9 ) : UINT64_MAX );
		if ( bResult )
		{
			g_StudioAPIVk.GetSyncMgr().ReleaseFence( it->second );
			submittedBatchesDict.erase( it );
		}
		return bResult;
	}
	return true;
}

/*
==================
CStudioAPICmdListBatchMgrVk::ClearSubmittedBatches
==================
*/
void CStudioAPICmdListBatchMgrVk::ClearSubmittedBatches()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Release all submitted fences
	for ( auto it = submittedBatchesDict.begin(), itEnd = submittedBatchesDict.end(); it != itEnd; ++it )
	{
		g_StudioAPIVk.GetSyncMgr().ReleaseFence( it->second );
	}
	submittedBatchesDict.clear();
}

/*
==================
CStudioAPICmdListBatchMgrVk::GrabVkSyncSignalsFromBatch
==================
*/
void CStudioAPICmdListBatchMgrVk::GrabVkSyncSignalsFromBatch( CStudioAPICmdListBatchVk* pCmdListBatch, eastl::vector<VkSemaphore>& vkSyncSignals ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Add general synchronization signals
	for ( auto it = pCmdListBatch->syncSignals.begin(), itEnd = pCmdListBatch->syncSignals.end(); it != itEnd; ++it )
	{
		CStudioAPISemaphoreVk* pSyncSignal = ( *it )->GetCurrentSemaphore();
		Assert( !pSyncSignal->IsSignaled() );
		vkSyncSignals.emplace_back( pSyncSignal->GetVkSemaphore() );
		pSyncSignal->Signal();
	}

	// Add a swap chain synchronization signal if the batch has one
	if ( pCmdListBatch->pSyncSwapChain && pCmdListBatch->syncSwapChainFlags & STUDIOAPI_SYNC_SWAPCHAIN_FLAGS_PRESENT_TO_IMAGE )
	{
		CStudioAPISemaphoreVk* pRenderFinishedSemaphore = pCmdListBatch->pSyncSwapChain->GetRenderFinishedSemaphore();
		Assert( !pRenderFinishedSemaphore->IsSignaled() );
		vkSyncSignals.emplace_back( pRenderFinishedSemaphore->GetVkSemaphore() );
		pRenderFinishedSemaphore->Signal();
	}
}

/*
==================
CStudioAPICmdListBatchMgrVk::GrabVkSyncWaitsFromBatch
==================
*/
void CStudioAPICmdListBatchMgrVk::GrabVkSyncWaitsFromBatch( CStudioAPICmdListBatchVk* pCmdListBatch, eastl::vector<VkSemaphore>& vkSyncWaits, eastl::vector<VkPipelineStageFlags>& vkSyncWaitStageMasks ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Add wait of general synchronization signals
	for ( auto it = pCmdListBatch->syncWaits.begin(), itEnd = pCmdListBatch->syncWaits.end(); it != itEnd; ++it )
	{
		CStudioAPISemaphoreVk* pSyncWait = ( *it )->GetCurrentSemaphore();
		Assert( pSyncWait->IsSignaled() );
		vkSyncWaits.emplace_back( pSyncWait->GetVkSemaphore() );
		vkSyncWaitStageMasks.emplace_back( VK_PIPELINE_STAGE_ALL_COMMANDS_BIT );
		pSyncWait->Unsignal();
	}

	// Add wait of a swap chain synchronization signal if the batch has one
	if ( pCmdListBatch->pSyncSwapChain && pCmdListBatch->syncSwapChainFlags & STUDIOAPI_SYNC_SWAPCHAIN_FLAGS_ACQUIRE_NEXT_IMAGE )
	{
		CStudioAPISemaphoreVk* pImageAvailableSemaphore = pCmdListBatch->pSyncSwapChain->GetImageAvailableSemaphore();
		Assert( pImageAvailableSemaphore->IsSignaled() );
		vkSyncWaits.emplace_back( pImageAvailableSemaphore->GetVkSemaphore() );
		vkSyncWaitStageMasks.emplace_back( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );
		pImageAvailableSemaphore->Unsignal();
	}
}

/*
==================
CStudioAPICmdListBatchMgrVk::GrabVkCmdBuffersFromBatch
==================
*/
void CStudioAPICmdListBatchMgrVk::GrabVkCmdBuffersFromBatch( CStudioAPICmdListBatchVk* pCmdListBatch, eastl::vector<VkCommandBuffer>& vkCmdBuffers ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Grab all command buffers from the batch
	vkCmdBuffers.resize( pCmdListBatch->cmdLists.size() );
	for ( uint32 index = 0, count = (uint32)vkCmdBuffers.size(); index < count; ++index )
	{
		CStudioAPICmdListVk* pCmdList = pCmdListBatch->cmdLists[index];
		vkCmdBuffers[index]			  = pCmdList->GetCmdBuffer()->GetVkCommandBuffer();
		pCmdList->state				  = STUDIOAPI_VK_CMDLIST_STATE_READY_FOR_RECORD;
	}
}