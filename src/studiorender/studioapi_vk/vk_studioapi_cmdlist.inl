#pragma once

/*
==================
CStudioAPICmdListVk::MarkHasDynamicStates
==================
*/
FORCEINLINE void CStudioAPICmdListVk::MarkHasDynamicStates()
{
	bNeedsDynamicStateSet = false;
}

/*
==================
CStudioAPICmdListVk::MarkHasPipeline
==================
*/
FORCEINLINE void CStudioAPICmdListVk::MarkHasPipeline()
{
	bHasPipeline = true;
}

/*
==================
CStudioAPICmdListVk::MarkHasViewport
==================
*/
FORCEINLINE void CStudioAPICmdListVk::MarkHasViewport( VkViewport vkViewport )
{
	vkCurrentViewport = vkViewport;
	bHasViewport	  = true;
}

/*
==================
CStudioAPICmdListVk::MarkHasScissor
==================
*/
FORCEINLINE void CStudioAPICmdListVk::MarkHasScissor( VkRect2D vkScissor )
{
	vkCurrentScissor = vkScissor;
	bHasScissor		 = true;
}

/*
==================
CStudioAPICmdListVk::IsNeedsDynamicStateSet
==================
*/
FORCEINLINE bool CStudioAPICmdListVk::IsNeedsDynamicStateSet() const
{
	return bNeedsDynamicStateSet;
}

/*
==================
CStudioAPICmdListVk::HasPipeline
==================
*/
FORCEINLINE bool CStudioAPICmdListVk::HasPipeline() const
{
	return bHasPipeline;
}

/*
==================
CStudioAPICmdListVk::GetVkCurrentViewport
==================
*/
FORCEINLINE VkViewport CStudioAPICmdListVk::GetVkCurrentViewport() const
{
	return vkCurrentViewport;
}

/*
==================
CStudioAPICmdListVk::HasViewport
==================
*/
FORCEINLINE bool CStudioAPICmdListVk::HasViewport() const
{
	return bHasViewport;
}

/*
==================
CStudioAPICmdListVk::GetVkCurrentScissor
==================
*/
FORCEINLINE VkRect2D CStudioAPICmdListVk::GetVkCurrentScissor() const
{
	return vkCurrentScissor;
}

/*
==================
CStudioAPICmdListVk::HasScissor
==================
*/
FORCEINLINE bool CStudioAPICmdListVk::HasScissor() const
{
	return bHasScissor;
}

/*
==================
CStudioAPICmdListVk::GetState
==================
*/
FORCEINLINE studioAPICmdListStateVk_t CStudioAPICmdListVk::GetState() const
{
	return state;
}

/*
==================
CStudioAPICmdListVk::GetCmdBuffer
==================
*/
FORCEINLINE CStudioAPICmdBufferVk* CStudioAPICmdListVk::GetCmdBuffer() const
{
	return pCmdBuffer;
}
