#ifndef VK_STUDIOAPI_DELEGATES_H
#define VK_STUDIOAPI_DELEGATES_H

#include "stdlib/delegate.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPIRenderPipelineVk;
class CStudioAPIDescriptorSetsLayoutVk;


//-----------------------------------------------------------------------------
// Vulkan delegates
//-----------------------------------------------------------------------------
DECLARE_MULTICAST_DELEGATE( COnStudioAPIVkShutdown );
DECLARE_MULTICAST_DELEGATE( COnStudioAPIRenderPipelineDeletedVk, CStudioAPIRenderPipelineVk* /* pRenderPipeline */ );
DECLARE_MULTICAST_DELEGATE( COnStudioAPIDescriptorSetsLayoutDeletedVk, CStudioAPIDescriptorSetsLayoutVk* /* pDescriptorSetsLayout */ );

#endif // !VK_STUDIOAPI_DELEGATES_H