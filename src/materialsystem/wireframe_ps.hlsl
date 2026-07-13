#include "common.hlsl"

//-----------------------------------------------------------------------------
// Shader binding slots
//-----------------------------------------------------------------------------
#define BINDING_SLOT_SHADER_CB 			STUDIO_RESOURCE_BINDING_SLOT_FREE_BEGIN

//-----------------------------------------------------------------------------
// Shader parameters
//-----------------------------------------------------------------------------
struct shaderParams_t
{
	float4 color;
};

//-----------------------------------------------------------------------------
// Shader resources
//-----------------------------------------------------------------------------
STUDIOAPI_CONSTANT_BUFFER( shaderParams_t, shaderParams, BINDING_SLOT_SHADER_CB, 0 );

/*
==================
MainPS
==================
*/
void MainPS( out float4 retColor : SV_TARGET0 )
{
	retColor = shaderParams.color;
}