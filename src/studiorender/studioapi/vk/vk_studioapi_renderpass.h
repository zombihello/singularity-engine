/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_RENDERPASS_H
#define VK_STUDIOAPI_RENDERPASS_H

#include "studiorender/studioapi/istudioapi_renderpass.h"
#include "studiorender/studioapi/vk/vk_studioapi_device.h"

/**
 * @ingroup studioapi_vk
 * @brief Vulkan studio api render pass
 */
class CStudioAPIRenderPassVk : public TRefCounted<IStudioAPIRenderPass>
{
public:
	/*
	 * @brief Constructor
	 * @param createInfo	Information to create the render pass
	 * @param pDebugName	Debug name
	 */
	CStudioAPIRenderPassVk( const studioAPIRenderPassCreateInfo_t& createInfo, const achar* pDebugName = "" );

	/**
	 * @brief Destructor
	 */
	~CStudioAPIRenderPassVk();

	/**
	 * @brief Get the Vulkan render pass
	 * @return Return the Vulkan render pass. If the frame buffer isn't created return VK_NULL_HANDLE
	 */
	FORCEINLINE VkRenderPass GetVkRenderPass() const
	{
		return vkRenderPass;
	}

private:
	/**
	 * @brief Delegate called when StudioAPI Vulkan is shutdown
	 * @param pUserData		Pointer to CStudioAPIRenderPassVk
	 */
	static void OnStudioAPIVkShutdown( void* pUserData );

	VkRenderPass								vkRenderPass;					/**< Vulkan render pass */
	COnStudioAPIVkShutdown::funcDelegate_t*		pStudioAPIVkShutdownDelegate;	/**< Delegate handle 'on StudioAPI Vulkan shutdown' */
};

#endif // !VK_STUDIOAPI_RENDERPASS_H