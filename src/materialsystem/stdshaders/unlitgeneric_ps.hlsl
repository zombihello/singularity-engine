#include "common.hlsl"

//-----------------------------------------------------------------------------
// Shader binding slots
//-----------------------------------------------------------------------------
#define BINDING_SLOT_SHADER_CB 			( STUDIO_RESOURCE_BINDING_SLOT_FREE_BEGIN + 0 )
#define BINDING_SLOT_BASETEXTURE		( STUDIO_RESOURCE_BINDING_SLOT_FREE_BEGIN + 1 )
#define BINDING_SLOT_BASESAMPLER		( STUDIO_RESOURCE_BINDING_SLOT_FREE_BEGIN + 2 )

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
STUDIOAPI_CONSTANT_BUFFER( 	shaderParams_t, shaderParams, 	BINDING_SLOT_SHADER_CB, 	0 );
STUDIOAPI_TEXTURE_2D( 		baseTexture, 					BINDING_SLOT_BASETEXTURE, 	0 );
STUDIOAPI_SAMPLER_STATE( 	baseSampler, 					BINDING_SLOT_BASESAMPLER, 	0 );

/*
==================
MainPS
==================
*/
float4 MainPS( in float2 texCoord : TEXCOORD0 ) : SV_Target
{
	return baseTexture.Sample( baseSampler, texCoord ) * shaderParams.color;	
}