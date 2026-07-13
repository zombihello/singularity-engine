#ifndef GLOBALCONSTANTS_HLSL
#define GLOBALCONSTANTS_HLSL 0
#include "common.hlsl"

//-----------------------------------------------------------------------------
// Studio global shader params
// NOTE: Layout must match studioGlobalShaderParams_t in src/public/studiorender/studio_globalshaderparams.h
//-----------------------------------------------------------------------------
struct studioGlobalShaderParams_t
{
	column_major float4x4 viewMatrix;
	column_major float4x4 projectionMatrix;
	column_major float4x4 viewProjectionMatrix;
	column_major float4x4 invViewProjectionMatrix;
	float4				  position;				 // xyz = camera world position
	float4				  screenAndBufferSize;	 // xy = screen size, zw = render target size
};
STUDIOAPI_CONSTANT_BUFFER( studioGlobalShaderParams_t, globalShaderParams, STUDIO_RESOURCE_BINDING_SLOT_GLOBAL_CB, 0 );

#endif // GLOBALCONSTANTS_HLSL
