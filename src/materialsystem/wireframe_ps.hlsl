#include "common.hlsl"

struct shaderParams_t
{
	float4 color;
};
STUDIOAPI_CONSTANT_BUFFER( shaderParams_t, shaderParams, 1, 0 );

void MainPS( out float4 retColor : SV_TARGET0 )
{
	retColor = shaderParams.color;
}