#pragma once
#include "stdlib/delegate.h"
#include "studiorender/studioapi/vk/vk_studioapi_queue.h"

//-----------------------------------------------------------------------------
// Vulkan Studio API device
//-----------------------------------------------------------------------------
class CStudioAPIDeviceVk
{
public:
	CStudioAPIDeviceVk();
	~CStudioAPIDeviceVk();

	void Init( uint32 engineMajorVersion, uint32 engineMinorVersion, uint32 enginePatchVersion );
	void Shutdown();

	CStudioAPIQueueVk& GetGraphicsQueue() const;
	CStudioAPIQueueVk& GetPresentQueue() const;
	CStudioAPIQueueVk& GetTransferQueue() const;
	CStudioAPIQueueVk& GetComputeQueue() const;

	VkInstance								GetVkInstance() const;
	VkPhysicalDevice						GetVkPhysicalDevice() const;
	VkDevice								GetVkLogicalDevice() const;
	const VkPhysicalDeviceMemoryProperties& GetVkMemoryInfo() const;
	const VkPhysicalDeviceProperties&		GetVkDeviceInfo() const;
	const VkPhysicalDeviceLimits&			GetVkDeviceLimits() const;
	studioAPIGPUVendorId_t					GetGPUVendorId() const;

private:
	struct queueFamilyIndices_t
	{
		queueFamilyIndices_t()
			: graphicsFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
			, presentFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
			, transferFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
			, computeFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
		{
		}

		void Clear();
		bool IsValid( uint32 queueFamilyTypes ) const;

		uint32					graphicsFamilyIndex;
		uint32					presentFamilyIndex;
		uint32					transferFamilyIndex;
		uint32					computeFamilyIndex;
		VkQueueFamilyProperties graphicsFamilyProperties;
		VkQueueFamilyProperties presentFamilyProperties;
		VkQueueFamilyProperties transferFamilyProperties;
		VkQueueFamilyProperties computeFamilyProperties;
	};

	void				 CreateVkDevice();
	queueFamilyIndices_t FindQueueFamilyIndices( const VkPhysicalDevice& vkPhysicalDevice, const eastl::vector<VkQueueFamilyProperties>& vkQueueFamiliesProperties, uint32 queueFamilyTypes ) const;

	VkInstance						 vkInstance;
	VkPhysicalDevice				 vkPhysicalDevice;
	VkDevice						 vkLogicalDevice;
	CStudioAPIQueueVk				 graphicsQueue;
	CStudioAPIQueueVk				 presentQueue;
	CStudioAPIQueueVk				 transferQueue;
	CStudioAPIQueueVk				 computeQueue;
	VkPhysicalDeviceMemoryProperties vkMemoryInfo;
	VkPhysicalDeviceProperties		 vkDeviceInfo;
	studioAPIGPUVendorId_t			 gpuVendorId;
#if !RETAIL
	VkDebugUtilsMessengerEXT vkDebugMessenger;
#endif	// !RETAIL
};

#include "studiorender/studioapi/vk/vk_studioapi_device.inl"