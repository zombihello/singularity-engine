/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_RENDERPIPELINE_H
#define VK_STUDIOAPI_RENDERPIPELINE_H

#include "studiorender/studioapi/istudioapi_renderpipeline.h"
#include "studiorender/studioapi/vk/vk_studioapi_delegates.h"
#include "studiorender/studioapi/vk/vk_studioapi_device.h"
#include "studiorender/studioapi/vk/vk_studioapi_renderpass.h"

/**
 * @ingroup studioapi_vk
 * @brief Vulkan studio api render pipeline
 */
class CStudioAPIRenderPipelineVk : public TRefCounted<IStudioAPIRenderPipeline>
{
public:
	/*
	 * @brief Constructor
	 * @param createInfo	Information to create the render pipeline
	 * @param pDebugName	Debug name
	 */
	CStudioAPIRenderPipelineVk( const studioAPIRenderPipelineCreateInfo_t& createInfo, const achar* pDebugName = "" );

	/**
	 * @brief Destructor
	 */
	~CStudioAPIRenderPipelineVk();

	/**
	 * @brief Get the Vulkan pipeline
	 * @return Return the Vulkan pipeline. If the render pipeline isn't created return VK_NULL_HANDLE
	 */
	FORCEINLINE VkPipeline GetVkPipeline() const
	{
		return vkPipeline;
	}

	/**
	 * @brief Get the bound shader state
	 * @return Return the bound shader state. If the render pipeline isn't created return NULL
	 */
	FORCEINLINE CStudioAPIBoundShaderStateVk* GetBoundShaderState() const
	{
		return pBoundShaderState;
	}

	/**
	 * @brief Get event manager 'on the render pipeline is deleted'
	 * @return Return event manager 'on the render pipeline is deleted'
	 */
	FORCEINLINE COnStudioAPIRenderPipelineDeletedVk& OnRenderPipelineDeleted() const
	{
		return onRenderPipelineDeleted;
	}

private:
	/**
	 * @brief Delegate called when StudioAPI Vulkan is shutdown
	 * @param pUserData		Pointer to CStudioAPIRenderPipelineVk
	 */
	static void OnStudioAPIVkShutdown( void* pUserData );

	VkPipeline										vkPipeline;						/**< Vulkan pipeline */
	COnStudioAPIVkShutdown::funcDelegate_t*			pStudioAPIVkShutdownDelegate;	/**< Delegate handle 'on StudioAPI Vulkan shutdown' */
	TRefPtr<CStudioAPIBoundShaderStateVk>			pBoundShaderState;				/**< Bound shader state */
	mutable COnStudioAPIRenderPipelineDeletedVk		onRenderPipelineDeleted;		/**< Event on the render pipeline is deleted */
};

#endif // !VK_STUDIOAPI_RENDERPIPELINE_H