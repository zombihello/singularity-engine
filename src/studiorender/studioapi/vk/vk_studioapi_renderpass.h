#ifndef VK_STUDIOAPI_RENDERPASS_H
#define VK_STUDIOAPI_RENDERPASS_H

#include "studiorender/studioapi/istudioapi_renderpass.h"
#include "studiorender/studioapi/vk/vk_studioapi_device.h"

//-----------------------------------------------------------------------------
// Vulkan Studio API render pass
//-----------------------------------------------------------------------------
class CStudioAPIRenderPassVk : public TRefCounted<IStudioAPIRenderPass>
{
public:
	CStudioAPIRenderPassVk( const studioAPIRenderPassCreateInfo_t& createInfo, const achar* pDebugName = "" );
	~CStudioAPIRenderPassVk();

	FORCEINLINE VkRenderPass GetVkRenderPass() const
	{
		return vkRenderPass;
	}

private:
	static void OnStudioAPIVkShutdown( void* pUserData );

	VkRenderPass								vkRenderPass;
	COnStudioAPIVkShutdown::funcDelegate_t*		pStudioAPIVkShutdownDelegate;
};

#endif // !VK_STUDIOAPI_RENDERPASS_H