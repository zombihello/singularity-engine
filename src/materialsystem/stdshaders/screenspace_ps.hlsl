#include "common.hlsl"

//-----------------------------------------------------------------------------
// Shader binding slots
//-----------------------------------------------------------------------------
#define BINDING_SLOT_BASETEXTURE ( STUDIO_RESOURCE_BINDING_SLOT_FREE_BEGIN + 0 )
#define BINDING_SLOT_BASESAMPLER ( STUDIO_RESOURCE_BINDING_SLOT_FREE_BEGIN + 1 )

//-----------------------------------------------------------------------------
// Shader resources
//-----------------------------------------------------------------------------
STUDIOAPI_TEXTURE_2D( 	 baseTexture, BINDING_SLOT_BASETEXTURE, 0 );
STUDIOAPI_SAMPLER_STATE( baseSampler, BINDING_SLOT_BASESAMPLER, 0 );

/*
==================
MainPS
==================
*/
float4 MainPS( in float2 texCoord : TEXCOORD0 ) : SV_Target
{
    return baseTexture.Sample( baseSampler, texCoord );
}
