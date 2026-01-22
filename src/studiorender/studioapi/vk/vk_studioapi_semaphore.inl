#pragma once

/*
==================
CStudioAPISemaphoreVk::Signal
==================
*/
FORCEINLINE void CStudioAPISemaphoreVk::Signal()
{
	Assert( !bSignaled );
	bSignaled = true;
}

/*
==================
CStudioAPISemaphoreVk::Unsignal
==================
*/
FORCEINLINE void CStudioAPISemaphoreVk::Unsignal()
{
	Assert( bSignaled );
	bSignaled = false;
}

/*
==================
CStudioAPISemaphoreVk::IsSignaled
==================
*/
FORCEINLINE bool CStudioAPISemaphoreVk::IsSignaled() const
{
	return bSignaled;
}

/*
==================
CStudioAPISemaphoreVk::GetVkSemaphore
==================
*/
FORCEINLINE VkSemaphore CStudioAPISemaphoreVk::GetVkSemaphore() const
{
	return vkSemaphore;
}

/*
==================
CStudioAPISemaphoreVk::Destroy
==================
*/
FORCEINLINE void CStudioAPISemaphoreVk::Destroy()
{
	// Destroy the Vulkan semaphore
	if ( vkSemaphore != VK_NULL_HANDLE )
	{
		vkDestroySemaphore( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkSemaphore, NULL );
		vkSemaphore = VK_NULL_HANDLE;
	}

	// Reset signaled flag
	bSignaled = false;
}

/*
==================
CStudioAPINamedSemaphoreVk::operator CStudioAPISemaphoreVk*
==================
*/
FORCEINLINE CStudioAPINamedSemaphoreVk::operator CStudioAPISemaphoreVk*() const
{
	return GetCurrentSemaphore();
}

/*
==================
CStudioAPINamedSemaphoreVk::operator->
==================
*/
FORCEINLINE CStudioAPISemaphoreVk* CStudioAPINamedSemaphoreVk::operator->() const
{
	return GetCurrentSemaphore();
}

/*
==================
CStudioAPINamedSemaphoreVk::GetCurrentSemaphore
==================
*/
FORCEINLINE CStudioAPISemaphoreVk* CStudioAPINamedSemaphoreVk::GetCurrentSemaphore() const
{
	return pSemaphores[g_StudioAPIVk.GetCurrentFrameInFlight()];
}

/*
==================
CStudioAPINamedSemaphoreVk::GetName
==================
*/
FORCEINLINE const eastl::string& CStudioAPINamedSemaphoreVk::GetName() const
{
	return name;
}
