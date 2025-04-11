#include "common.hlsl"

struct shaderParams_t
{
	float4 color;
};
STUDIOAPI_CONSTANT_BUFFER( shaderParams_t, shaderParams, 1, 0 );

STUDIOAPI_TEXTURE_2D( baseTexture, 2, 0 );
STUDIOAPI_SAMPLER_STATE( baseSampler, 3, 0 );

float4 MainPS( in float2 texCoord : TEXCOORD0 ) : SV_Target
{
	return baseTexture.Sample( baseSampler, texCoord ) * shaderParams.color;	
}