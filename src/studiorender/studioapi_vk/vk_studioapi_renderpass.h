#pragma once
#include "studiorender/studioapi/istudioapi_renderpass.h"
#include "studiorender/studioapi_vk/vk_studioapi_device.h"

//-----------------------------------------------------------------------------
// Vulkan Studio API render pass
//-----------------------------------------------------------------------------
class CStudioAPIRenderPassVk : public CRefCounted<IStudioAPIRenderPass>
{
public:
	CStudioAPIRenderPassVk( const studioAPIRenderPassCreateInfo_t& createInfo, const char* pDebugName = "" );
	~CStudioAPIRenderPassVk();

	FORCEINLINE VkRenderPass GetVkRenderPass() const
	{
		return vkRenderPass;
	}

private:
	static void OnStudioAPIVkShutdown( void* pUserData );

	VkRenderPass					 vkRenderPass;
	COnStudioAPIVkShutdown::handle_t onStudioAPIVkShutdownHandle;
};
