#include "pch_modelsystem.h"
#include "studiorender/studioapi/istudioapi.h"
#include "modelsystem/vertexdeclarations.h"

/*
==================
CVertexDeclarations::InitStudioAPI
==================
*/
void CVertexDeclarations::InitStudioAPI()
{
	// Create a vertex declaration for modelSimpleVertex_t
	{
		studioAPIVertexInputBuffer_t studioAPIVertexInputBuffers[] = {
			{ 0, sizeof( modelSimpleVertex_t ), STUDIOAPI_VERTEX_INPUT_RATE_VERTEX }
		};
		studioAPIVertexInputAttribute_t studioAPIVertexInputAttributes[] = {
			{ 0, OFFSET_OF( modelSimpleVertex_t, position ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4, 0 },
			{ 0, OFFSET_OF( modelSimpleVertex_t, texCoord ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT2, 1 },
			{ 0, OFFSET_OF( modelSimpleVertex_t, color ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_BYTE4UN, 2 }
		};
		pStudioAPIVertexDeclarations[MODEL_VERTEXTYPE_SIMPLE] = g_pStudioAPI->CreateVertexDeclaration( studioAPIVertexInputBuffers, ARRAYSIZE( studioAPIVertexInputBuffers ),
																									   studioAPIVertexInputAttributes, ARRAYSIZE( studioAPIVertexInputAttributes ) );
	}

	// Create a vertex declaration for modelStaticVertex_t
	{
		studioAPIVertexInputBuffer_t studioAPIVertexInputBuffers[] = {
			{ 0, sizeof( modelStaticVertex_t ), STUDIOAPI_VERTEX_INPUT_RATE_VERTEX }
		};
		studioAPIVertexInputAttribute_t studioAPIVertexInputAttributes[] = {
			{ 0, OFFSET_OF( modelStaticVertex_t, position ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4, 0 },
			{ 0, OFFSET_OF( modelStaticVertex_t, texCoord ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT2, 1 },
			{ 0, OFFSET_OF( modelStaticVertex_t, normal ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4, 2 },
			{ 0, OFFSET_OF( modelStaticVertex_t, tangent ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4, 3 },
			{ 0, OFFSET_OF( modelStaticVertex_t, binormal ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4, 4 }
		};
		pStudioAPIVertexDeclarations[MODEL_VERTEXTYPE_STATIC] = g_pStudioAPI->CreateVertexDeclaration( studioAPIVertexInputBuffers, ARRAYSIZE( studioAPIVertexInputBuffers ),
																									   studioAPIVertexInputAttributes, ARRAYSIZE( studioAPIVertexInputAttributes ) );
	}
}

/*
==================
CVertexDeclarations::ReleaseStudioAPI
==================
*/
void CVertexDeclarations::ReleaseStudioAPI()
{
	for ( uint32 vertexType = 0; vertexType < MODEL_VERTEX_NUM_TYPES; ++vertexType )
	{
		pStudioAPIVertexDeclarations[vertexType] = NULL;
	}
}