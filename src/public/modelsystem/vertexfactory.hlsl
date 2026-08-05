#ifndef VERTEXFACTORY_HLSL
#define VERTEXFACTORY_HLSL 0

// This is an abstract vertex factory interface. The shader compiler substitutes this file with a
// concrete implementation (see vertexfactory_*.hlsl) for each vertex factory permutation a shader is compiled for.
//
// A concrete implementation must define:
//   struct vertexFactoryInput_t { ... };
//   float4 VertexFactory_GetLocalPosition( vertexFactoryInput_t input );
//   float2 VertexFactory_GetTexCoord( vertexFactoryInput_t input );
//   float4 VertexFactory_GetColor( vertexFactoryInput_t input );

#error "vertexfactory.hlsl must be substituted with a concrete vertex factory implementation"

#endif // VERTEXFACTORY_HLSL
