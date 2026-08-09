#include "common.hlsl"

struct vertexFactoryInput_t
{
	STUDIOAPI_VERTEX_INPUT( float4 position, 0 );
	STUDIOAPI_VERTEX_INPUT( float2 texCoord, 1 );
	STUDIOAPI_VERTEX_INPUT( float4 color,    2 );
};

float4 VertexFactory_GetLocalPosition( vertexFactoryInput_t input )
{
	return input.position;
}

float4 VertexFactory_GetWorldPosition( vertexFactoryInput_t input )
{
	return input.position;
}

float2 VertexFactory_GetTexCoord( vertexFactoryInput_t input )
{
	return input.texCoord;
}

float4 VertexFactory_GetColor( vertexFactoryInput_t input )
{
	return input.color;
}
