#ifndef VK_STUDIOAPI_FENCE_H
#define VK_STUDIOAPI_FENCE_H

#include "studiorender/studioapi/vk/vk_studioapi.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPISyncMgrVk;


//-----------------------------------------------------------------------------
// Vulkan StudioAPI fence
//-----------------------------------------------------------------------------
class CStudioAPIFenceVk
{
public:
	friend CStudioAPISyncMgrVk;

	CStudioAPIFenceVk( bool bCreateSignaled = false )
		: state( bCreateSignaled ? FENCE_STATE_SIGNALED : FENCE_STATE_NOT_READY )
		, vkFence( VK_NULL_HANDLE )
	{
		// Create fence
		VkFenceCreateInfo				vkFenceCreateInfo = {};
		vkFenceCreateInfo.sType			= VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		vkFenceCreateInfo.flags			= bCreateSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
		STUDIOAPI_VK_VERIFY_RESULT( vkCreateFence( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkFenceCreateInfo, NULL, &vkFence ) );
	}
	~CStudioAPIFenceVk()
	{
		// TODO yehor.pohuliaka - Implement deferred deletion
		Destroy();
	}

	void Reset();
	bool Wait( uint64 waitTime );
	bool WaitAndReset( uint64 waitTime );

	bool IsSignaled() const;
	VkFence GetVkFence() const;

private:
	enum fenceState_t
	{
		FENCE_STATE_NOT_READY,
		FENCE_STATE_SIGNALED
	};

	void Destroy();

	mutable fenceState_t	state;
	VkFence					vkFence;
};

#include "studiorender/studioapi/vk/vk_studioapi_fence.inl"

#endif // !VK_STUDIOAPI_FENCE_H