/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_DEVICE_H
#define VK_STUDIOAPI_DEVICE_H

#include "stdlib/delegate.h"
#include "studiorender/studioapi/vk/vk_studioapi_queue.h"

/**
 * @ingroup studioapi_vk
 * @brief Vulkan studio api device
 */
class CStudioAPIDeviceVk
{
public:
	/**
	 * @brief Constructor
	 */
	CStudioAPIDeviceVk();

	/**
	 * @brief Destructor
	 */
	~CStudioAPIDeviceVk();

	/**
	 * @brief Init device
	 * @param engineMajorVersion	Engine major version
	 * @param engineMinorVersion	Engine minor version
	 * @param enginePatchVersion	Engine patch version
	 */
	void Init( uint32 engineMajorVersion, uint32 engineMinorVersion, uint32 enginePatchVersion );

	/**
	 * @brief Shutdown device
	 */
	void Shutdown();

	/**
	 * @brief Get Vulkan instance
	 * @return Return Vulkan instance, if it is not created return VK_NULL_HANDLE
	 */
	FORCEINLINE VkInstance GetVkInstance() const
	{
		return vkInstance;
	}

	/**
	 * @brief Get Vulkan physical device
	 * @return Return Vulkan physical device, if it is not found return VK_NULL_HANDLE
	 */
	FORCEINLINE VkPhysicalDevice GetVkPhysicalDevice() const
	{
		return vkPhysicalDevice;
	}

	/**
	 * @brief Get Vulkan logical device
	 * @return Return Vulkan logical device, if it is not created return VK_NULL_HANDLE
	 */
	FORCEINLINE VkDevice GetVkLogicalDevice() const
	{
		return vkLogicalDevice;
	}

	/**
	 * @brief Get the graphics queue
	 * @return Return the graphics queue
	 */
	FORCEINLINE CStudioAPIQueueVk& GetGraphicsQueue() const
	{
		return ( CStudioAPIQueueVk& )graphicsQueue;
	}

	/**
	 * @brief Get the present queue
	 * @return Return the present queue
	 */
	FORCEINLINE CStudioAPIQueueVk& GetPresentQueue() const
	{
		return ( CStudioAPIQueueVk& )presentQueue;
	}

	/**
	 * @brief Get the transfer queue
	 * @return Return the transfer queue
	 */
	FORCEINLINE CStudioAPIQueueVk& GetTransferQueue() const
	{
		return ( CStudioAPIQueueVk& )transferQueue;
	}

	/**
	 * @brief Get the compute queue
	 * @return Return the compute queue
	 */
	FORCEINLINE CStudioAPIQueueVk& GetComputeQueue() const
	{
		return ( CStudioAPIQueueVk& )computeQueue;
	}

	/**
	 * @brief Get the physical device memory information
	 * @return Return the physical device memory information
	 */
	FORCEINLINE const VkPhysicalDeviceMemoryProperties& GetVkMemoryInfo() const
	{
		return vkMemoryInfo;
	}

	/**
	 * @brief Get the physical device information
	 * @return Return the physical device information
	 */
	FORCEINLINE const VkPhysicalDeviceProperties& GetVkDeviceInfo() const
	{
		return vkDeviceInfo;
	}

	/**
	 * @brief Get the physical device limits
	 * @return Return the physical device limits
	 */
	FORCEINLINE const VkPhysicalDeviceLimits& GetVkDeviceLimits() const
	{
		return vkDeviceInfo.limits;
	}

	/**
	 * @brief Get the GPU vendor ID
	 * @return Return the GPU vendor ID
	 */
	FORCEINLINE studioAPIGPUVendorId_t GetGPUVendorId() const
	{
		return gpuVendorId;
	}

private:
	/**
	 * @brief Vulkan queue family indeces
	 */
	struct queueFamilyIndices_t
	{
		/**
		 * @brief Constructor
		 */
		queueFamilyIndices_t()
			: graphicsFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
			, presentFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
			, transferFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
			, computeFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
		{}

		/**
		 * @brief Clear queue family indeces
		 */
		FORCEINLINE void Clear()
		{
			graphicsFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			presentFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
			transferFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			computeFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
		}

		/**
		 * @brief Is valid queue family indices
		 * @param queueFamilyTypes	Queue family types to check (see studioAPIQueueVkFlags_t)
		 * @return Return TRUE if the queue family indices are valid, otherwise FALSE
		 */
		FORCEINLINE bool IsValid( uint32 queueFamilyTypes ) const
		{
			// Check the graphics queue family index
			bool	bInvalid = false;
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

		uint32						graphicsFamilyIndex;		/**< Graphics family index */
		uint32						presentFamilyIndex;			/**< Present family index */
		uint32						transferFamilyIndex;		/**< Transfer family index */
		uint32						computeFamilyIndex;			/**< Compute family index */
		VkQueueFamilyProperties		graphicsFamilyProperties;	/**< Graphics family properties */
		VkQueueFamilyProperties		presentFamilyProperties;	/**< Present family properties */
		VkQueueFamilyProperties		transferFamilyProperties;	/**< Transfer family properties */
		VkQueueFamilyProperties		computeFamilyProperties;	/**< Compute family properties */
	};

	/**
	 * @brief Create Vulkan device
	 */
	void CreateVkDevice();

	/**
	 * @brief Find queue family indices
	 * @param vkPhysicalDevice			Physical device
	 * @param vkQueueFamiliesProperties	Queue families properties array of the device
	 * @param queueFamilyTypes			Queue family types to find (see studioAPIQueueVkFlags_t)
	 * @return Return struct with founded queue family indices. If something wouldn't be founded the index will be VK_QUEUE_FAMILY_IGNORED
	 */
	queueFamilyIndices_t FindQueueFamilyIndices( const VkPhysicalDevice& vkPhysicalDevice, const std::vector<VkQueueFamilyProperties>& vkQueueFamiliesProperties, uint32 queueFamilyTypes ) const;

	VkInstance								vkInstance;					/**< Vulkan instance */
	VkPhysicalDevice						vkPhysicalDevice;			/**< Vulkan physical device */
	VkDevice								vkLogicalDevice;			/**< Vulkan logical device */
	CStudioAPIQueueVk						graphicsQueue;				/**< Graphics queue of the logical device */
	CStudioAPIQueueVk						presentQueue;				/**< Present queue of the logical device */
	CStudioAPIQueueVk						transferQueue;				/**< Transfer queue of the logical device */
	CStudioAPIQueueVk						computeQueue;				/**< Compute queue of the logical device */
	VkPhysicalDeviceMemoryProperties		vkMemoryInfo;				/**< Physical device memory information */
	VkPhysicalDeviceProperties				vkDeviceInfo;				/**< Physical device information */
	studioAPIGPUVendorId_t					gpuVendorId;				/**< GPU vendor ID */
#if !RETAIL
	VkDebugUtilsMessengerEXT				vkDebugMessenger;			/**< Vulkan debug messenger */
#endif // !RETAIL
};

#endif // !VK_STUDIOAPI_DEVICE_H