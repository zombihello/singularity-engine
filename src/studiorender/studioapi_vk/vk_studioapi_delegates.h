#pragma once
#include "tier1/event.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPIRenderPipelineVk;
class CStudioAPIDescriptorSetsLayoutVk;

//-----------------------------------------------------------------------------
// Vulkan delegates
//-----------------------------------------------------------------------------
DECLARE_EVENT( COnStudioAPIVkShutdown );
DECLARE_EVENT( COnStudioAPIRenderPipelineDeletedVk, CStudioAPIRenderPipelineVk* /* pRenderPipeline */ );
DECLARE_EVENT( COnStudioAPIDescriptorSetsLayoutDeletedVk, CStudioAPIDescriptorSetsLayoutVk* /* pDescriptorSetsLayout */ );
