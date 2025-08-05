#pragma once
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