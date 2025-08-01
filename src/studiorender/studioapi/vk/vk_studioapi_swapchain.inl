#pragma once

/*
==================
CStudioAPISwapChainImageVk::UpdateSyncState
==================
*/
FORCEINLINE void CStudioAPISwapChainImageVk::UpdateSyncState( VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	VK_UpdateSyncStateImage( vkDstImageLayout, vkDstAccessMask, vkDstStageMask, dstQueueFamilyIndex, syncState );
}

/*
==================
CStudioAPISwapChainImageVk::GetVkImage
==================
*/
FORCEINLINE VkImage CStudioAPISwapChainImageVk::GetVkImage() const
{
	return vkImage;
}

/*
==================
CStudioAPISwapChainImageVk::GetVkImageView
==================
*/
FORCEINLINE VkImageView CStudioAPISwapChainImageVk::GetVkImageView() const
{
	return vkImageView;
}

/*
==================
CStudioAPISwapChainImageVk::GetSyncState
==================
*/
FORCEINLINE const studioAPISyncStateImageVk_t& CStudioAPISwapChainImageVk::GetSyncState() const
{
	return syncState;
}


/*
==================
CStudioAPISwapChainVk::GetVkSurfaceFormat
==================
*/
FORCEINLINE VkSurfaceFormatKHR CStudioAPISwapChainVk::GetVkSurfaceFormat() const
{
	return vkSurfaceFormat;
}

/*
==================
CStudioAPISwapChainVk::GetVkSwapChain
==================
*/
FORCEINLINE VkSwapchainKHR CStudioAPISwapChainVk::GetVkSwapChain() const
{
	return vkSwapChain;
}

/*
==================
CStudioAPISwapChainVk::IsCreated
==================
*/
FORCEINLINE bool CStudioAPISwapChainVk::IsCreated() const
{
	return vkSwapChain != VK_NULL_HANDLE;
}

/*
==================
CStudioAPISwapChainVk::GetImageAvailableSemaphore
==================
*/
FORCEINLINE CStudioAPISemaphoreVk* CStudioAPISwapChainVk::GetImageAvailableSemaphore() const
{
	return imageAvailableSemaphores[currentSemaphoreIndex];
}

/*
==================
CStudioAPISwapChainVk::GetRenderFinishedSemaphore
==================
*/
FORCEINLINE CStudioAPISemaphoreVk* CStudioAPISwapChainVk::GetRenderFinishedSemaphore() const
{
	return renderFinishedSemaphores[currentSemaphoreIndex];
}
