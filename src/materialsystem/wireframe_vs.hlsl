#include "common.hlsl"
#include "vertexfactory.hlsl"

/*
==================
MainVS
==================
*/
void MainVS( in vertexFactoryInput_t input, out float4 screenPosition : SV_POSITION )
{
	screenPosition 		  	= mul( globalShaderParams.viewProjectionMatrix, VertexFactory_GetWorldPosition( input ) );
}