/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_CMDBUFFER_H
#define VK_STUDIOAPI_CMDBUFFER_H

#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "core/platform.h"
#include "studiorender/studioapi/vk/vk_studioapi_fence.h"
#include "studiorender/studioapi/vk/vk_studioapi_semaphore.h"
#include "studiorender/studioapi/vk/vk_studioapi_descriptorpoolsmgr.h"

// Forward declarations
class CStudioAPICmdBufferPoolVk;
class CStudioAPIDescriptorPoolSetContainerVk;

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI command buffer
 */
class CStudioAPICmdBufferVk
{
public:
	friend CStudioAPICmdBufferPoolVk;
	friend CStudioAPIQueueVk;

	/**
	 * @brief Begin write commands into the buffer
	 */
	void Begin();

	/**
	 * @brief End write commands into the buffer
	 */
	void End();

	/**
	 * @brief Acquire pool set and descriptors if need
	 * @param descriptorSetsLayout			Descriptor sets layout for a pool set
	 * @param bNeedDescriptorSets			Is need allocate descriptor sets
	 * @param pDestDescriptorSets			Destination descriptor sets. If bNeedDescriptors is FALSE the param will be ignored
	 * @return Return TRUE if the descriptor sets have been allocated, otherwise FALSE
	 */
	bool AcquirePoolSetAndDescriptorsIfNeed( const CStudioAPIDescriptorSetsLayoutVk& descriptorSetsLayout, bool bNeedDescriptorSets, VkDescriptorSet* pDestDescriptorSets );

	/**
	 * @brief Get the command buffer pool where the buffer is
	 * @return Return the command buffer pool where the buffer is
	 */
	FORCEINLINE CStudioAPICmdBufferPoolVk& GetCmdBufferPool()
	{
		return cmdBufferPool;
	}

	/**
	 * @brief Get the Vulkan command buffer
	 * @return Return the Vulkan command buffer. If isn't allocated return VK_NULL_HANDLE
	 */
	FORCEINLINE VkCommandBuffer GetVkCommandBuffer() const
	{
		return vkCommandBuffer;
	}

private:
	/**
	 * @brief Constructor
	 * @param cmdBufferPool		Command buffer pool where the buffer is
	 */
	CStudioAPICmdBufferVk( CStudioAPICmdBufferPoolVk& cmdBufferPool );

	/**
	 * @brief Destructor
	 */
	~CStudioAPICmdBufferVk();

	/**
	 * @brief Allocate memory
	 */
	void AllocMemory();

	/**
	 * @brief Free allocated memory
	 */
	void FreeMemory();

	bool									bIsAllocatedMemory;					/**< Is allocated memory for the buffer */
	VkCommandBuffer							vkCommandBuffer;					/**< Vulkan command buffer */
	CStudioAPICmdBufferPoolVk&				cmdBufferPool;						/**< Command buffer pool where the buffer is */	
	CStudioAPIDescriptorPoolSetContainerVk* pCurrentDescriptorPoolSetContainer;	/**< Current descriptor pool set container */
};

#endif // !VK_STUDIOAPI_CMDBUFFER_H