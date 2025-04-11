/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_CMDBUFFERPOOL_H
#define VK_STUDIOAPI_CMDBUFFERPOOL_H

#include "studiorender/studioapi/vk/vk_studioapi_queue.h"

// Forward declarations
class CStudioAPICmdBufferVk;
class CStudioAPIQueueVk;

/**
 * @ingroup studioapi_vk
 * @brief Vulkan studioAPI command buffer pool
 */
class CStudioAPICmdBufferPoolVk
{
public:
	/**
	 * @brief Constructor
	 * @param queue		Queue which corresponds the pool
	 */
	CStudioAPICmdBufferPoolVk( CStudioAPIQueueVk& queue );

	/**
	 * @brief Destructor
	 */
	~CStudioAPICmdBufferPoolVk();

	/**
	 * @brief Initialize the command pool
	 */
	void Init();

	/**
	 * @brief Shutdown the command pool
	 */
	void Shutdown();

	/**
	 * @brief Create a command buffer
	 * @return Return created the command buffer
	 */
	CStudioAPICmdBufferVk* CreateCmdBuffer();

	/**
	 * @brief Free unused command buffers
	 */
	void FreeUnusedCmdBuffers();

	/**
	 * @brief Is valid the pool
	 * @return Return TRUE if the pool is valid, otherwise return FALSE
	 */
	FORCEINLINE bool IsValid() const
	{
		return vkCommandPool != VK_NULL_HANDLE;
	}

	/**
	 * @brief Get the queue which corresponds the pool
	 * @return Return the queue which corresponds the pool
	 */
	FORCEINLINE CStudioAPIQueueVk& GetQueue()
	{
		return queue;
	}

	/**
	 * @brief Get the Vulkan command pool
	 * @return Return the Vulkan command pool. If the pool hasn't been initialized return VK_NULL_HANDLE
	 */
	FORCEINLINE VkCommandPool GetVkCommandPool() const
	{
		return vkCommandPool;
	}

private:
	VkCommandPool						vkCommandPool;		/**< Vulkan command pool */
	CStudioAPIQueueVk&					queue;				/**< Queue which corresponds the pool */
	std::list<CStudioAPICmdBufferVk*>	cmdBuffers;			/**< Command buffers in use */
	std::list<CStudioAPICmdBufferVk*>	freeCmdBuffers;		/**< Free command buffers */
};

#endif // !VK_STUDIOAPI_CMDBUFFERPOOL_H