#pragma once
#include <EASTL/vector.h>
#include <EASTL/unordered_set.h>
#include <EASTL/unordered_map.h>

#include "stdlib/defines.h"
#include "studiorender/studioapi_vk/vk_studioapi_fence.h"
#include "studiorender/studioapi_vk/vk_studioapi_semaphore.h"
#include "studiorender/studioapi_vk/vk_studioapi_descriptorpoolsmgr.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPICmdBufferPoolVk;
class CStudioAPIDescriptorPoolSetContainerVk;

//-----------------------------------------------------------------------------
// Vulkan StudioAPI command buffer
//-----------------------------------------------------------------------------
class CStudioAPICmdBufferVk
{
public:
	friend CStudioAPICmdBufferPoolVk;
	friend CStudioAPIQueueVk;

	// Begin/end write commands into the buffer
	void Begin();
	void End();

	bool AcquirePoolSetAndDescriptorsIfNeed( const CStudioAPIDescriptorSetsLayoutVk& descriptorSetsLayout, bool bNeedDescriptorSets, VkDescriptorSet* pDestDescriptorSets );

	FORCEINLINE CStudioAPICmdBufferPoolVk& GetCmdBufferPool() { return cmdBufferPool; }
	FORCEINLINE VkCommandBuffer			   GetVkCommandBuffer() const { return vkCommandBuffer; }

private:
	CStudioAPICmdBufferVk( CStudioAPICmdBufferPoolVk& cmdBufferPool );
	~CStudioAPICmdBufferVk();

	// Allocate/free memory
	void AllocMemory();
	void FreeMemory();

	bool									bIsAllocatedMemory;
	VkCommandBuffer							vkCommandBuffer;
	CStudioAPICmdBufferPoolVk&				cmdBufferPool;
	CStudioAPIDescriptorPoolSetContainerVk* pCurrentDescriptorPoolSetContainer;
};
