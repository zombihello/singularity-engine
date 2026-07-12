#include "common.hlsl"
#include "vertexfactory.hlsl"

void MainVS( in vertexFactoryInput_t input, out float4 screenPosition : SV_POSITION, out float2 outTexCoord : TEXCOORD0 )
{
	screenPosition 	= VertexFactory_GetLocalPosition( input );
	outTexCoord 	= VertexFactory_GetTexCoord( input );
}
