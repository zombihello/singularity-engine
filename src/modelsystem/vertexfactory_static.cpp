#include "pch_modelsystem.h"
#include "studiorender/studioapi/istudioapi.h"
#include "modelsystem/vertexfactory_static.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
CVertexDeclarationStatic g_vertexDeclarationStatic;

/*
==================
CVertexDeclarationStatic::InitStudioAPI
==================
*/
void CVertexDeclarationStatic::InitStudioAPI()
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
	pStudioAPIVertexDeclaration = g_pStudioAPI->CreateVertexDeclaration( studioAPIVertexInputBuffers, ARRAYSIZE( studioAPIVertexInputBuffers ),
																		 studioAPIVertexInputAttributes, ARRAYSIZE( studioAPIVertexInputAttributes ) );
}

/*
==================
CVertexFactoryStatic::GetVertexType
==================
*/
modelVertexType_t CVertexFactoryStatic::GetVertexType() const
{
	return MODEL_VERTEXTYPE_STATIC;
}

/*
==================
CVertexFactoryStatic::GetStudioAPIVertexDeclaration
==================
*/
IStudioAPIVertexDeclaration* CVertexFactoryStatic::GetStudioAPIVertexDeclaration() const
{
	return g_vertexDeclarationStatic.GetStudioAPIVertexDeclaration();
}
