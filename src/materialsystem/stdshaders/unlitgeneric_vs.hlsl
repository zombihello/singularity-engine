#include "common.hlsl"
#include "vertexfactory.hlsl"

/*
==================
MainVS
==================
*/
void MainVS( in vertexFactoryInput_t input, out float4 screenPosition : SV_POSITION, out float2 outTexCoord : TEXCOORD0 )
{
	screenPosition 		  = mul( globalShaderParams.viewProjectionMatrix, VertexFactory_GetWorldPosition( input ) );
	outTexCoord 		  = VertexFactory_GetTexCoord( input );
}
