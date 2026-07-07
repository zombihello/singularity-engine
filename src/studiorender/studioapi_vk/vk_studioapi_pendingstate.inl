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
	// Update viewport
	vkViewport.x		= x;
	vkViewport.y		= y;
	vkViewport.width	= width;
	vkViewport.height	= height;
	vkViewport.minDepth = minDepth;
	vkViewport.maxDepth = maxDepth;

	// Update scissor
	SetScissor( false, (int32)x, (int32)y, (uint32)width, (uint32)height );
}

/*
==================
CStudioAPIPendingRenderStateVk::SetScissor
==================
*/
FORCEINLINE void CStudioAPIPendingRenderStateVk::SetScissor( bool bEnable, int32 x, int32 y, uint32 width, uint32 height )
{
	if ( bEnable )
	{
		vkScissor.offset.x		= x;
		vkScissor.offset.y		= y;
		vkScissor.extent.width	= width;
		vkScissor.extent.height = height;
	}
	else
	{
		vkScissor.offset.x		= (int32)vkViewport.x;
		vkScissor.offset.y		= (int32)vkViewport.y;
		vkScissor.extent.width	= (uint32)vkViewport.width;
		vkScissor.extent.height = (uint32)vkViewport.height;
	}

	bScissorEnabled = bEnable;
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
CStudioAPIPendingRenderStateVk::indexBuffer_t::Clear
==================
*/
FORCEINLINE void CStudioAPIPendingRenderStateVk::indexBuffer_t::Clear()
{
	pBuffer = NULL;
	offset	= 0;
}
