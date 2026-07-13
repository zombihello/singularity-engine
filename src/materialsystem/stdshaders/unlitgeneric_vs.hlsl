#include "common.hlsl"
#include "vertexfactory.hlsl"

/*
==================
MainVS
==================
*/
void MainVS( in vertexFactoryInput_t input, out float4 screenPosition : SV_POSITION, out float2 outTexCoord : TEXCOORD0 )
{
	float4 localPosition = VertexFactory_GetLocalPosition( input );
	screenPosition 		  = MulMatrix( globalShaderParams.viewProjectionMatrix, float4( localPosition.xyz, 1.0 ) );
	outTexCoord 		  = VertexFactory_GetTexCoord( input );
}
