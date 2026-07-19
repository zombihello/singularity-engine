#include "common.hlsl"

#if USE_VERTEXFACTORY
#include "vertexfactory.hlsl"

/*
==================
MainVS
==================
*/
void MainVS( in vertexFactoryInput_t input, out float4 screenPosition : SV_POSITION, out float2 texCoord : TEXCOORD0 )
{
	screenPosition = VertexFactory_GetLocalPosition( input );
	texCoord	   = VertexFactory_GetTexCoord( input );
}
#else
/*
==================
MainVS
==================
*/
void MainVS(in uint vertexID : SV_VertexID, out float4 screenPosition : SV_POSITION, out float2 texCoord : TEXCOORD0)
{
    texCoord		= float2( ( vertexID << 1 ) & 2, vertexID & 2 );
    screenPosition	= float4( texCoord * float2( 2.f, -2.f ) + float2( -1.f, 1.f ), 0.f, 1.f );
}
#endif // USE_VERTEXFACTORY
