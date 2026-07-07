#include "common.hlsl"

struct vertexInput_t
{
#if VERTEX_TYPE == 0
	STUDIOAPI_VERTEX_INPUT( float4 position, 	0 );
	STUDIOAPI_VERTEX_INPUT( float2 texCoord, 	1 );
	STUDIOAPI_VERTEX_INPUT( float4 color, 		2 );
#elif VERTEX_TYPE == 1
   	STUDIOAPI_VERTEX_INPUT( float4 position, 	0 );
   	STUDIOAPI_VERTEX_INPUT( float2 texCoord, 	1 );
   	STUDIOAPI_VERTEX_INPUT( float4 normal, 		2 );
   	STUDIOAPI_VERTEX_INPUT( float4 tangent, 	3 );
   	STUDIOAPI_VERTEX_INPUT( float4 binormal, 	4 );
#else
   	#error Unknown vertex type
#endif // VERTEX_TYPE
};

void MainVS( in vertexInput_t vertex, out float4 screenPosition : SV_POSITION, out float2 outTexCoord : TEXCOORD0 )
{
	screenPosition 	= vertex.position;
	outTexCoord 	= vertex.texCoord;
}