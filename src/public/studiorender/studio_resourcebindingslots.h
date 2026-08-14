#pragma once

//-----------------------------------------------------------------------------
// Studio resource binding slots
//
// NOTES:
//	* All slots below live in descriptor set 0
//	* Must stay in sync with the mirrored `#defines` in src/public/materialsystem/common.hlsl
//-----------------------------------------------------------------------------
enum studioResourceBindingSlot_t
{
	STUDIO_RESOURCE_BINDING_SLOT_GLOBAL_CB		  = 0,	// Global shader parameters (see studioGlobalShaderParams_t)
	STUDIO_RESOURCE_BINDING_SLOT_VERTEXFACTORY_CB = 1,	// Vertex factory shader parameters
	STUDIO_RESOURCE_BINDING_SLOT_FREE_BEGIN		  = 2	// Free slots start here
};
