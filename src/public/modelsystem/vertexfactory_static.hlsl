#include "common.hlsl"

struct vertexFactoryInput_t
{
	STUDIOAPI_VERTEX_INPUT( float4 position, 		0 );
	STUDIOAPI_VERTEX_INPUT( float2 texCoord, 		1 );
	STUDIOAPI_VERTEX_INPUT( float4 normal,   		2 );
	STUDIOAPI_VERTEX_INPUT( float4 tangent,  		3 );
	STUDIOAPI_VERTEX_INPUT( float4 binormal, 		4 );
	STUDIOAPI_VERTEX_INPUT( float3 localToWorld0, 	5 );
	STUDIOAPI_VERTEX_INPUT( float3 localToWorld1, 	6 );
	STUDIOAPI_VERTEX_INPUT( float3 localToWorld2, 	7 );
	STUDIOAPI_VERTEX_INPUT( float3 localToWorld3, 	8 );
};

float3x4 VertexFactory_GetLocalToWorld( vertexFactoryInput_t input )
{
	return float3x4( 
		float4( input.localToWorld0.x, input.localToWorld1.x, input.localToWorld2.x, input.localToWorld3.x ),
		float4( input.localToWorld0.y, input.localToWorld1.y, input.localToWorld2.y, input.localToWorld3.y ),
		float4( input.localToWorld0.z, input.localToWorld1.z, input.localToWorld2.z, input.localToWorld3.z )
	 );
}

float4 VertexFactory_GetLocalPosition( vertexFactoryInput_t input )
{
	return input.position;
}

float4 VertexFactory_GetWorldPosition( vertexFactoryInput_t input )
{
	return float4( mul( VertexFactory_GetLocalToWorld( input ), input.position ), 1.f );
}

float2 VertexFactory_GetTexCoord( vertexFactoryInput_t input )
{
	return input.texCoord;
}

float4 VertexFactory_GetColor( vertexFactoryInput_t input )
{
	return float4( 1.0, 1.0, 1.0, 1.0 );
}
