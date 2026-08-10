#pragma once

/*
==================
CStudioAPIDeviceVk::GetVkInstance
==================
*/
FORCEINLINE VkInstance CStudioAPIDeviceVk::GetVkInstance() const
{
	return vkInstance;
}

/*
==================
CStudioAPIDeviceVk::GetVkPhysicalDevice
==================
*/
FORCEINLINE VkPhysicalDevice CStudioAPIDeviceVk::GetVkPhysicalDevice() const
{
	return vkPhysicalDevice;
}

/*
==================
CStudioAPIDeviceVk::GetVkLogicalDevice
==================
*/
FORCEINLINE VkDevice CStudioAPIDeviceVk::GetVkLogicalDevice() const
{
	return vkLogicalDevice;
}

/*
==================
CStudioAPIDeviceVk::GetGraphicsQueue
==================
*/
FORCEINLINE CStudioAPIQueueVk& CStudioAPIDeviceVk::GetGraphicsQueue() const
{
	return (CStudioAPIQueueVk&)graphicsQueue;
}

/*
==================
CStudioAPIDeviceVk::GetPresentQueue
==================
*/
FORCEINLINE CStudioAPIQueueVk& CStudioAPIDeviceVk::GetPresentQueue() const
{
	return (CStudioAPIQueueVk&)presentQueue;
}

/*
==================
CStudioAPIDeviceVk::GetTransferQueue
==================
*/
FORCEINLINE CStudioAPIQueueVk& CStudioAPIDeviceVk::GetTransferQueue() const
{
	return (CStudioAPIQueueVk&)transferQueue;
}

/*
==================
CStudioAPIDeviceVk::GetComputeQueue
==================
*/
FORCEINLINE CStudioAPIQueueVk& CStudioAPIDeviceVk::GetComputeQueue() const
{
	return (CStudioAPIQueueVk&)computeQueue;
}

/*
==================
CStudioAPIDeviceVk::GetVkMemoryInfo
==================
*/
FORCEINLINE const VkPhysicalDeviceMemoryProperties& CStudioAPIDeviceVk::GetVkMemoryInfo() const
{
	return vkMemoryInfo;
}

/*
==================
CStudioAPIDeviceVk::GetVkDeviceInfo
==================
*/
FORCEINLINE const VkPhysicalDeviceProperties& CStudioAPIDeviceVk::GetVkDeviceInfo() const
{
	return vkDeviceInfo;
}

/*
==================
CStudioAPIDeviceVk::GetVkDeviceLimits
==================
*/
FORCEINLINE const VkPhysicalDeviceLimits& CStudioAPIDeviceVk::GetVkDeviceLimits() const
{
	return vkDeviceInfo.limits;
}

/*
==================
CStudioAPIDeviceVk::GetGPUVendorId
==================
*/
FORCEINLINE studioAPIGPUVendorId_t CStudioAPIDeviceVk::GetGPUVendorId() const
{
	return gpuVendorId;
}

/*
==================
CStudioAPIDeviceVk::IsDebug
==================
*/
FORCEINLINE bool CStudioAPIDeviceVk::IsDebug() const
{
#if !RETAIL
	return bDebug;
#else
	return false;
#endif	// !RETAIL
}

/*
==================
CStudioAPIDeviceVk::queueFamilyIndices_t::Clear
==================
*/
FORCEINLINE void CStudioAPIDeviceVk::queueFamilyIndices_t::Clear()
{
	graphicsFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	presentFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
	transferFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	computeFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
}

/*
==================
CStudioAPIDeviceVk::queueFamilyIndices_t::IsValid
==================
*/
FORCEINLINE bool CStudioAPIDeviceVk::queueFamilyIndices_t::IsValid( uint32 queueFamilyTypes ) const
{
	// Check the graphics queue family index
	bool bInvalid = false;
	if ( queueFamilyTypes & STUDIOAPI_VK_QUEUE_FLAG_GRAPHICS )
	{
		bInvalid |= graphicsFamilyIndex == VK_QUEUE_FAMILY_IGNORED;
	}

	// Check the present queue family index
	if ( queueFamilyTypes & STUDIOAPI_VK_QUEUE_FLAG_PRESENT )
	{
		bInvalid |= presentFamilyIndex == VK_QUEUE_FAMILY_IGNORED;
	}

	// Check the transfer queue family index
	if ( queueFamilyTypes & STUDIOAPI_VK_QUEUE_FLAG_TRANSFER )
	{
		bInvalid |= transferFamilyIndex == VK_QUEUE_FAMILY_IGNORED;
	}

	// Check the compute queue family index
	if ( queueFamilyTypes & STUDIOAPI_VK_QUEUE_FLAG_COMPUTE )
	{
		bInvalid |= computeFamilyIndex == VK_QUEUE_FAMILY_IGNORED;
	}

	// We are done!
	return !bInvalid;
}
