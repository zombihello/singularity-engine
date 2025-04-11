#include "pch_studiorender.h"
#include "studiorender/studio_vertexdeclarations.h"

// Studio vertex declarations
CStudioVertexDeclarations	g_StudioVertexDeclarations;

/*
==================
CStudioVertexDeclarations::InitStudioAPI
==================
*/
void CStudioVertexDeclarations::InitStudioAPI()
{
	// Create a StudioAPI vertex declaration for studioSimpleElementVertex_t
	{
		studioAPIVertexInputBuffer_t			studioAPIVertexInputBuffers[] =
		{
			{ 0, sizeof( studioSimpleElementVertex_t ), STUDIOAPI_VERTEX_INPUT_RATE_VERTEX }
		};
		studioAPIVertexInputAttribute_t			studioAPIVertexInputAttributes[] =
		{
			{ 0, OFFSET_OF( studioSimpleElementVertex_t, position ),	STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4,		0 },
			{ 0, OFFSET_OF( studioSimpleElementVertex_t, texCoord ),	STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT2,		1 },
			{ 0, OFFSET_OF( studioSimpleElementVertex_t, color ),		STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_BYTE4UN,	2 }
		};
		pStudioAPIVertexDeclarations[STUDIO_VERTEXTYPE_SIMPLEELEMENT] = g_pStudioAPI->CreateVertexDeclaration( studioAPIVertexInputBuffers,		ARRAYSIZE( studioAPIVertexInputBuffers ), 
																											   studioAPIVertexInputAttributes,	ARRAYSIZE( studioAPIVertexInputAttributes ) );
	}

	// Create a StudioAPI vertex declaration for studioModelVertex_t
	{
		studioAPIVertexInputBuffer_t			studioAPIVertexInputBuffers[] =
		{
			{ 0, sizeof( studioModelVertex_t ), STUDIOAPI_VERTEX_INPUT_RATE_VERTEX }
		};
		studioAPIVertexInputAttribute_t			studioAPIVertexInputAttributes[] =
		{
			{ 0, OFFSET_OF( studioModelVertex_t, position ),		STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4,		0 },
			{ 0, OFFSET_OF( studioModelVertex_t, texCoord ),		STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT2,		1 },
			{ 0, OFFSET_OF( studioModelVertex_t, normal ),			STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4,		2 },
			{ 0, OFFSET_OF( studioModelVertex_t, tangent ),			STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4,		3 },
			{ 0, OFFSET_OF( studioModelVertex_t, binormal ),		STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4,		4 }
		};
		pStudioAPIVertexDeclarations[STUDIO_VERTEXTYPE_MODEL] = g_pStudioAPI->CreateVertexDeclaration( studioAPIVertexInputBuffers,		ARRAYSIZE( studioAPIVertexInputBuffers ),
																									   studioAPIVertexInputAttributes,	ARRAYSIZE( studioAPIVertexInputAttributes ) );
	}
}

/*
==================
CStudioVertexDeclarations::ReleaseStudioAPI
==================
*/
void CStudioVertexDeclarations::ReleaseStudioAPI()
{
	for ( uint32 vertexType = 0; vertexType < STUDIO_VERTEX_NUM_TYPES; ++vertexType )
	{
		pStudioAPIVertexDeclarations[vertexType] = NULL;
	}
}