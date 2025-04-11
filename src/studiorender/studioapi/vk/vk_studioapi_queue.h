/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_QUEUE_H
#define VK_STUDIOAPI_QUEUE_H

#include <list>

#include "Volk/volk.h"
#include "core/types.h"
#include "core/platform.h"
#include "studiorender/studioapi/vk/vk_helpers.h"

// Forward declarations
class CStudioAPIFenceVk;

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI queue flag
 */
enum studioAPIVkQueueFlag_t
{
	STUDIOAPI_VK_QUEUE_FLAG_NONE		= 0,		/**< No flags */
	STUDIOAPI_VK_QUEUE_FLAG_GRAPHICS	= 1 << 1,	/**< Queue can execute graphics commands */
	STUDIOAPI_VK_QUEUE_FLAG_PRESENT		= 1 << 2,	/**< Queue can present frame */
	STUDIOAPI_VK_QUEUE_FLAG_COMPUTE		= 1 << 3,	/**< Queue can execute compute commands */
	STUDIOAPI_VK_QUEUE_FLAG_TRANSFER	= 1 << 4	/**< Queue can execute transfer commands */
};

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI sharing mode setup
 * Class that helps to setup sharingMode, queueFamilyIndexCount, pQueueFamilyIndices in Vulkan structures
 */
class CStudioAPIQueueSharingModeSetupVk
{
public:
	/**
	 * @brief Constructor
	 * @param queueFamilyIndex0		Queue family index 0
	 * @param queueFamilyIndex1		Queue family index 1
	 * @param queueFamilyIndex2		Queue family index 2
	 * @param queueFamilyIndex3		Queue family index 3
	 */
	CStudioAPIQueueSharingModeSetupVk( uint32 queueFamilyIndex0, uint32 queueFamilyIndex1, uint32 queueFamilyIndex2 = VK_QUEUE_FAMILY_IGNORED, uint32 queueFamilyIndex3 = VK_QUEUE_FAMILY_IGNORED );

	/**
	 * @brief Setup sharing mode
	 * @param vkSharingMode				Sharing mode
	 * @param queueFamilyIndexCount		Queue family index count
	 * @param pQueueFamilyIndices		Queue family indices
	 */
	FORCEINLINE void Setup( VkSharingMode& vkSharingMode, uint32& queueFamilyIndexCount, const uint32*& pQueueFamilyIndices ) const
	{
		vkSharingMode			= !IsExclusive() ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
		queueFamilyIndexCount	= numQueueFamilyIndices;
		pQueueFamilyIndices		= !IsExclusive() ? queueFamilyIndices : NULL;
	}

	/**
	 * @brief Is exclusive mode
	 * @return Return TRUE if the sharing mode is exclusive, otherwise FALSE
	 */
	FORCEINLINE bool IsExclusive() const
	{
		return numQueueFamilyIndices < 2;
	}

private:
	uint32		numQueueFamilyIndices;		/**< Queue family indices number */
	uint32		queueFamilyIndices[4];		/**< Queue family indices */
};

/**
 * @ingroup studioapi_vk
 * @brief Vulkan studio api queue
 */
class CStudioAPIQueueVk
{
public:
	/**
	 * @brief Constructor
	 */
	CStudioAPIQueueVk();

	/**
	 * @brief Destructor
	 */
	~CStudioAPIQueueVk();

	/**
	 * @brief Initialize the queue
	 * @param queueFamilyIndex			Queue family index
	 * @param vkQueueFamilyProperties	Vulkan queue family properties
	 */
	void Init( uint32 queueFamilyIndex, const VkQueueFamilyProperties& vkQueueFamilyProperties );

	/**
	 * @brief Shutdown the queue
	 */
	void Shutdown();

	/**
	 * @brief Submit command
	 * @param pVkSubmitInfos		Vulkan submit info structures
	 * @param numVkSubmitInfos		Vulkan submit info structures number
	 * @param pFence				Fence to signal when the submit will be done
	 */
	void Submit( VkSubmitInfo* pVkSubmitInfos, uint32 numVkSubmitInfos, CStudioAPIFenceVk* pFence = NULL );

	/**
	 * @brief Get queue family index
	 * @return Return queue family index. If queue isn't valid return -1
	 */
	FORCEINLINE uint32 GetQueueFamilyIndex() const
	{
		return queueFamilyIndex;
	}

	/**
	 * @brief Get Vulkan queue
	 * @return Return Vulkan queue, if queue isn't valid return VK_NULL_HANDLE
	 */
	FORCEINLINE VkQueue GetVkQueue() const
	{
		return vkQueue;
	}

	/**
	 * @brief Get Vulkan queue family properties
	 * @return Return Vulkan queue family properties
	 */
	FORCEINLINE const VkQueueFamilyProperties& GetVkQueueFamilyProperties() const
	{
		return vkQueueFamilyProperties;
	}

private:
	uint32						queueFamilyIndex;				/**< Queue family index */
	VkQueue						vkQueue;						/**< Vulkan Queue */
	VkQueueFamilyProperties		vkQueueFamilyProperties;		/**< Vulkan queue family properties */
};

#endif // !VK_STUDIOAPI_QUEUE_H