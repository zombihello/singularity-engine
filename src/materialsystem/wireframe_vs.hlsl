#include "common.hlsl"

struct vertexInput_t
{	
	STUDIOAPI_VERTEX_INPUT( float4 position, 0 );
};

void MainVS( in vertexInput_t vertex, out float4 screenPosition : SV_POSITION )
{
	screenPosition = vertex.position;
}