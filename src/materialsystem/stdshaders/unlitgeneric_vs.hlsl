#include "common.hlsl"

struct vertexInput_t
{	
	STUDIOAPI_VERTEX_INPUT( float4 position, 	0 );
	STUDIOAPI_VERTEX_INPUT( float2 texCoord, 	1 );
	STUDIOAPI_VERTEX_INPUT( float4 color, 		2 );
};

void MainVS( in vertexInput_t vertex, out float4 screenPosition : SV_POSITION, out float2 outTexCoord : TEXCOORD0 )
{
	screenPosition 	= vertex.position;
	outTexCoord 	= vertex.texCoord;
}