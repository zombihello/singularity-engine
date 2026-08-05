#include "common.hlsl"
#include "vertexfactory.hlsl"

/*
==================
MainVS
==================
*/
void MainVS( in vertexFactoryInput_t input, out float4 color : COLOR0, out float4 screenPosition : SV_POSITION )
{
	float4 localPosition	= VertexFactory_GetLocalPosition( input );
	screenPosition 			= MulMatrix( globalShaderParams.viewProjectionMatrix, float4( localPosition.xyz, 1.0 ) );
    color					= VertexFactory_GetColor(input);
}
