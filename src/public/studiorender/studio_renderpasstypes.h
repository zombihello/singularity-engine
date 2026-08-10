#pragma once

//-----------------------------------------------------------------------------
// Studio render pass type
//-----------------------------------------------------------------------------
enum studioRenderPassType_t
{
	STUDIO_RENDERPASS_TYPE_SCENE,
	STUDIO_RENDERPASS_TYPE_PRESENT,
	STUDIO_RENDERPASS_NUM_TYPES
};

// Function returns render pass name by type
const char* Studio_GetRenderPassName( studioRenderPassType_t renderPassType );
#include "studiorender/studio_renderpasstypes.inl"
