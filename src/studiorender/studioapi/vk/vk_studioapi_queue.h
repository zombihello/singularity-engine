#ifndef VK_STUDIOAPI_QUEUE_H
#define VK_STUDIOAPI_QUEUE_H

#include <list>

#include "Volk/volk.h"
#include "core/types.h"
#include "core/platform.h"
#include "studiorender/studioapi/vk/vk_helpers.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPIFenceVk;


//-----------------------------------------------------------------------------
// Vulkan StudioAPI queue flag
//-----------------------------------------------------------------------------
enum studioAPIVkQueueFlag_t
{
	STUDIOAPI_VK_QUEUE_FLAG_NONE		= 0,
	STUDIOAPI_VK_QUEUE_FLAG_GRAPHICS	= 1 << 1,
	STUDIOAPI_VK_QUEUE_FLAG_PRESENT		= 1 << 2,
	STUDIOAPI_VK_QUEUE_FLAG_COMPUTE		= 1 << 3,
	STUDIOAPI_VK_QUEUE_FLAG_TRANSFER	= 1 << 4
};


//-----------------------------------------------------------------------------
// Class that helps to setup sharingMode, queueFamilyIndexCount, pQueueFamilyIndices in Vulkan structures
//-----------------------------------------------------------------------------
class CStudioAPIQueueSharingModeSetupVk
{
public:
	CStudioAPIQueueSharingModeSetupVk( uint32 queueFamilyIndex0, uint32 queueFamilyIndex1, uint32 queueFamilyIndex2 = VK_QUEUE_FAMILY_IGNORED, uint32 queueFamilyIndex3 = VK_QUEUE_FAMILY_IGNORED );

	void Setup( VkSharingMode& vkSharingMode, uint32& queueFamilyIndexCount, const uint32*& pQueueFamilyIndices ) const;
	bool IsExclusive() const;

private:
	uint32		numQueueFamilyIndices;
	uint32		queueFamilyIndices[4];
};


//-----------------------------------------------------------------------------
// Vulkan Studio API queue
//-----------------------------------------------------------------------------
class CStudioAPIQueueVk
{
public:
	CStudioAPIQueueVk();
	~CStudioAPIQueueVk();

	void Init( uint32 queueFamilyIndex, const VkQueueFamilyProperties& vkQueueFamilyProperties );
	void Shutdown();

	void Submit( VkSubmitInfo* pVkSubmitInfos, uint32 numVkSubmitInfos, CStudioAPIFenceVk* pFence = NULL );

	uint32 GetQueueFamilyIndex() const;
	VkQueue GetVkQueue() const;
	const VkQueueFamilyProperties& GetVkQueueFamilyProperties() const;

private:
	uint32						queueFamilyIndex;
	VkQueue						vkQueue;
	VkQueueFamilyProperties		vkQueueFamilyProperties;
};

#include "studiorender/studioapi/vk/vk_studioapi_queue.inl"

#endif // !VK_STUDIOAPI_QUEUE_H