/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_SEMAPHORE_H
#define VK_STUDIOAPI_SEMAPHORE_H

#include "studiorender/studioapi/vk/vk_studioapi.h"

// Forward declarations
class CStudioAPISyncMgrVk;

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI semaphore
 */
class CStudioAPISemaphoreVk
{
public:
	friend CStudioAPISyncMgrVk;

	/**
	 * @brief Constructor
	 */
	CStudioAPISemaphoreVk()
		: bSignaled( false )
		, vkSemaphore( VK_NULL_HANDLE )
	{
		// Create semaphore
		VkSemaphoreCreateInfo			vkSemaphoreCreateInfo = {};
		vkSemaphoreCreateInfo.sType		= VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		STUDIOAPI_VK_VERIFY_RESULT( vkCreateSemaphore( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkSemaphoreCreateInfo, NULL, &vkSemaphore ) );
	}

	/**
	 * @brief Destructor
	 */
	~CStudioAPISemaphoreVk()
	{
		// TODO yehor.pohuliaka - Implement deferred deletion
		Destroy();
	}

	/**
	 * @brief Signal the semaphore
	 */
	FORCEINLINE void Signal()
	{
		Assert( !bSignaled );
		bSignaled = true;
	}

	/**
	 * @brief Unsignal the semaphore
	 */
	FORCEINLINE void Unsignal()
	{
		Assert( bSignaled );
		bSignaled = false;
	}

	/**
	 * @brief Is the semaphore signaled
	 * @return Return TRUE if the semaphore is signaled, otherwise FALSE
	 */
	FORCEINLINE bool IsSignaled() const
	{
		return bSignaled;
	}

	/**
	 * @brief Get Vulkan semaphore
	 * @return Return Vulkan semaphore
	 */
	FORCEINLINE VkSemaphore GetVkSemaphore() const
	{
		return vkSemaphore;
	}

private:
	/**
	 * @brief Destroy
	 */
	FORCEINLINE void Destroy()
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

	bool			bSignaled;			/**< Is signaled the semaphore */
	VkSemaphore		vkSemaphore;		/**< Vulkan semaphore */
};

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI named semaphore
 */
class CStudioAPINamedSemaphoreVk
{
public:
	friend CStudioAPISyncMgrVk;

	/**
	 * @brief Overload cast to CStudioAPISemaphoreVk*
	 * @return Return a semaphore for the current frame in-flight
	 */
	FORCEINLINE operator CStudioAPISemaphoreVk*() const
	{
		return GetCurrentSemaphore();
	}

	/**
	 * @brief Overload operator ->
	 * @return Return a semaphore for the current frame in-flight
	 */
	FORCEINLINE CStudioAPISemaphoreVk* operator->() const
	{
		return GetCurrentSemaphore();
	}

	/**
	 * @brief Get a semaphore for the current frame in-flight
	 * @return Return a semaphore for the current frame in-flight
	 */
	FORCEINLINE CStudioAPISemaphoreVk* GetCurrentSemaphore() const
	{
		return pSemaphores[g_StudioAPIVk.GetCurrentFrameInFlight()];
	}

	/**
	 * @brief Get semaphore name
	 * @return Return semaphore name
	 */
	FORCEINLINE const std::string& GetName() const
	{
		return name;
	}

private:
	/**
	 * @brief Constructor
	 * The class can be allocated only by CStudioAPISyncMgr
	 */
	CStudioAPINamedSemaphoreVk()
		: countReferences( 1 )
	{}

	/**
	 * @brief Destructor
	 * The class can be destroyed only by CStudioAPISyncMgr
	 */
	~CStudioAPINamedSemaphoreVk()
	{}

	uint32						countReferences;									/**< Count references on the object */
	CStudioAPISemaphoreVk*		pSemaphores[STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT];		/**< Semaphore for each a frame in-flight */
	std::string					name;												/**< Name */
};

#endif // !VK_STUDIOAPI_SEMAPHORE_H