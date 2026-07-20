#pragma once

/*
==================
CStudioAPIPendingComputeStateVk::Reset
==================
*/
FORCEINLINE void CStudioAPIPendingComputeStateVk::Reset()
{
}

/*
==================
CStudioAPIPendingRenderStateVk::SetViewport
==================
*/
FORCEINLINE void CStudioAPIPendingRenderStateVk::SetViewport( float x, float y, float width, float height, float minDepth, float maxDepth )
{
	// Update scissor to default-match the viewport (uses the original, non Y-flipped rectangle)
	SetScissor( false, (int32)x, (int32)y, (uint32)width, (uint32)height );

	// Update viewport
	// Negate height and move the viewport corner so we don't need to have the shaders flip
	// clip space (negative viewport height has been core since Vulkan 1.1)
	vkViewport.x		= x;
	vkViewport.y		= y + height;
	vkViewport.width	= width;
	vkViewport.height	= -height;
	vkViewport.minDepth = minDepth;
	vkViewport.maxDepth = maxDepth;
}

/*
==================
CStudioAPIPendingRenderStateVk::SetScissor
==================
*/
FORCEINLINE void CStudioAPIPendingRenderStateVk::SetScissor( bool bEnable, int32 x, int32 y, uint32 width, uint32 height )
{
	vkScissor.offset.x		= x;
	vkScissor.offset.y		= y;
	vkScissor.extent.width	= width;
	vkScissor.extent.height = height;
	bScissorEnabled			= bEnable;
}

/*
==================
CStudioAPIPendingRenderStateVk::SetPushConstants
==================
*/
FORCEINLINE void CStudioAPIPendingRenderStateVk::SetPushConstants( CStudioAPICmdListVk* pCmdList, byte* pData, uint32 dataSize )
{
	Assert( pData && dataSize > 0 && dataSize <= STUDIOAPI_VK_MAX_PUSH_CONSTANT_SIZE );
	Mem_Memcpy( pushConstants.data, pData, dataSize );
	pushConstants.size	= dataSize;
	bDirtyPushConstants = true;
}

/*
==================
CStudioAPIPendingRenderStateVk::BindRenderPipeline
==================
*/
FORCEINLINE void CStudioAPIPendingRenderStateVk::BindRenderPipeline( CStudioAPICmdListVk* pCmdList )
{
	vkCmdBindPipeline( pCmdList->GetCmdBuffer()->GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pCurrentRenderPipeline->GetVkPipeline() );
	pCmdList->MarkHasPipeline();
}

/*
==================
CStudioAPIPendingRenderStateVk::HasIndexBuffer
==================
*/
FORCEINLINE bool CStudioAPIPendingRenderStateVk::HasIndexBuffer() const
{
	return indexBuffer.pBuffer;
}

/*
==================
CStudioAPIPendingRenderStateVk::vertexBuffer_t::vertexBuffer_t
==================
*/
FORCEINLINE CStudioAPIPendingRenderStateVk::vertexBuffer_t::vertexBuffer_t()
	: offset( 0 )
{
}

/*
==================
CStudioAPIPendingRenderStateVk::vertexBuffer_t::Clear
==================
*/
FORCEINLINE void CStudioAPIPendingRenderStateVk::vertexBuffer_t::Clear()
{
	pBuffer = NULL;
	offset	= 0;
}

/*
==================
CStudioAPIPendingRenderStateVk::indexBuffer_t::indexBuffer_t
==================
*/
FORCEINLINE CStudioAPIPendingRenderStateVk::indexBuffer_t::indexBuffer_t()
	: offset( 0 )
{
}

/*
==================
CStudioAPIPendingRenderStateVk::indexBuffer_t::Clear
==================
*/
FORCEINLINE void CStudioAPIPendingRenderStateVk::indexBuffer_t::Clear()
{
	pBuffer = NULL;
	offset	= 0;
}

/*
==================
CStudioAPIPendingRenderStateVk::pushConstants_t::pushConstants_t
==================
*/
FORCEINLINE CStudioAPIPendingRenderStateVk::pushConstants_t::pushConstants_t()
	: size( 0 )
{
}

/*
==================
CStudioAPIPendingRenderStateVk::pushConstants_t::Clear
==================
*/
FORCEINLINE void CStudioAPIPendingRenderStateVk::pushConstants_t::Clear()
{
	size = 0;
}
