#pragma once

/*
==================
CStudioAPIQueueSharingModeSetupVk::Setup
==================
*/
FORCEINLINE void CStudioAPIQueueSharingModeSetupVk::Setup( VkSharingMode& vkSharingMode, uint32& queueFamilyIndexCount, const uint32*& pQueueFamilyIndices ) const
{
	vkSharingMode = !IsExclusive() ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
	queueFamilyIndexCount = numQueueFamilyIndices;
	pQueueFamilyIndices = !IsExclusive() ? queueFamilyIndices : NULL;
}

/*
==================
CStudioAPIQueueSharingModeSetupVk::IsExclusive
==================
*/
FORCEINLINE bool CStudioAPIQueueSharingModeSetupVk::IsExclusive() const
{
	return numQueueFamilyIndices < 2;
}


/*
==================
CStudioAPIQueueVk::GetQueueFamilyIndex
==================
*/
FORCEINLINE uint32 CStudioAPIQueueVk::GetQueueFamilyIndex() const
{
	return queueFamilyIndex;
}

/*
==================
CStudioAPIQueueVk::GetVkQueue
==================
*/
FORCEINLINE VkQueue CStudioAPIQueueVk::GetVkQueue() const
{
	return vkQueue;
}

/*
==================
CStudioAPIQueueVk::GetVkQueueFamilyProperties
==================
*/
FORCEINLINE const VkQueueFamilyProperties& CStudioAPIQueueVk::GetVkQueueFamilyProperties() const
{
	return vkQueueFamilyProperties;
}
