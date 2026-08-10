#pragma once

/*
==================
Studio_GetRenderPassName
==================
*/
FORCEINLINE const char* Studio_GetRenderPassName( studioRenderPassType_t renderPassType )
{
	static const char* s_pRenderPassNames[] = {
		"scene",   // STUDIO_RENDERPASS_TYPE_SCENE
		"present"  // STUDIO_RENDERPASS_TYPE_PRESENT
	};
	static_assert( STUDIO_RENDERPASS_NUM_TYPES == ARRAYSIZE( s_pRenderPassNames ), "Array size 's_pRenderPassNames' must be equal to STUDIO_RENDERPASS_NUM_TYPES" );

	Assert( renderPassType < STUDIO_RENDERPASS_NUM_TYPES );
	return s_pRenderPassNames[(uint32)renderPassType];
}
