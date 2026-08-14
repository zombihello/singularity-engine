#pragma once
#include "tier1/math/math_types.h"

//-----------------------------------------------------------------------------
// Studio global shader params
// NOTE: Layout must match studioGlobalShaderParams_t in src/public/materialsystem/globalconstants.hlsl
//-----------------------------------------------------------------------------
ALIGN16_PREDECL struct studioGlobalShaderParams_t
{
	matrix4x4_t viewMatrix;
	matrix4x4_t projectionMatrix;
	matrix4x4_t viewProjectionMatrix;
	matrix4x4_t invViewProjectionMatrix;
	vector4_t	position;			  // xyz = camera world position
	vector4_t	screenAndBufferSize;  // xy = screen size, zw = render target size
} ALIGN16_POSTDECL;
