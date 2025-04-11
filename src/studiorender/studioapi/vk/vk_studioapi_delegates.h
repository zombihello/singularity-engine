/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_DELEGATES_H
#define VK_STUDIOAPI_DELEGATES_H

#include "stdlib/delegate.h"

// Forward declarations
class CStudioAPIRenderPipelineVk;
class CStudioAPIDescriptorSetsLayoutVk;

/**
 * @ingroup studioapi_vk
 * @brief Event called when StudioAPI Vulkan is shutdown
 */
DECLARE_MULTICAST_DELEGATE( COnStudioAPIVkShutdown );

/**
 * @ingroup studioapi_vk
 * @brief Event called when a StuidoAPI Vulkan render pipeline is deleted
 */
DECLARE_MULTICAST_DELEGATE( COnStudioAPIRenderPipelineDeletedVk, CStudioAPIRenderPipelineVk* /* pRenderPipeline */ );

/**
 * @ingroup studioapi_vk
 * @brief Event called when a StudioAPI Vulkan descriptor sets layout is deleted
 */
DECLARE_MULTICAST_DELEGATE( COnStudioAPIDescriptorSetsLayoutDeletedVk, CStudioAPIDescriptorSetsLayoutVk* /* pDescriptorSetsLayout */ );

#endif // !VK_STUDIOAPI_DELEGATES_H