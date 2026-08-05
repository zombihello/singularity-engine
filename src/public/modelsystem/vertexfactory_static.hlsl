#include "common.hlsl"

struct vertexFactoryInput_t
{
	STUDIOAPI_VERTEX_INPUT( float4 position, 0 );
	STUDIOAPI_VERTEX_INPUT( float2 texCoord, 1 );
	STUDIOAPI_VERTEX_INPUT( float4 normal,   2 );
	STUDIOAPI_VERTEX_INPUT( float4 tangent,  3 );
	STUDIOAPI_VERTEX_INPUT( float4 binormal, 4 );
};

float4 VertexFactory_GetLocalPosition( vertexFactoryInput_t input )
{
	return input.position;
}

float2 VertexFactory_GetTexCoord( vertexFactoryInput_t input )
{
	return input.texCoord;
}

float4 VertexFactory_GetColor( vertexFactoryInput_t input )
{
	return float4( 1.0, 1.0, 1.0, 1.0 );
}
