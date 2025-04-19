#ifndef VK_STUDIOAPI_CMDBUFFERPOOL_H
#define VK_STUDIOAPI_CMDBUFFERPOOL_H

#include "studiorender/studioapi/vk/vk_studioapi_queue.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPICmdBufferVk;
class CStudioAPIQueueVk;


//-----------------------------------------------------------------------------
// Vulkan studioAPI command buffer pool
//-----------------------------------------------------------------------------
class CStudioAPICmdBufferPoolVk
{
public:
	CStudioAPICmdBufferPoolVk( CStudioAPIQueueVk& queue );
	~CStudioAPICmdBufferPoolVk();

	void Init();
	void Shutdown();

	CStudioAPICmdBufferVk* CreateCmdBuffer();
	void FreeUnusedCmdBuffers();

	FORCEINLINE bool IsValid() const						{ return vkCommandPool != VK_NULL_HANDLE; }
	FORCEINLINE CStudioAPIQueueVk& GetQueue()				{ return queue; }
	FORCEINLINE VkCommandPool GetVkCommandPool() const		{ return vkCommandPool; }

private:
	VkCommandPool						vkCommandPool;
	CStudioAPIQueueVk&					queue;
	std::list<CStudioAPICmdBufferVk*>	cmdBuffers;
	std::list<CStudioAPICmdBufferVk*>	freeCmdBuffers;
};

#endif // !VK_STUDIOAPI_CMDBUFFERPOOL_H