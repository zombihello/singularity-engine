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
		{ 0, sizeof( modelSimpleVertex_t ), STUDIOAPI_VERTEX_INPUT_RATE_VERTEX }
	};
	studioAPIVertexInputAttribute_t studioAPIVertexInputAttributes[] = {
		{ 0, OFFSET_OF( modelSimpleVertex_t, position ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4, 0 },
		{ 0, OFFSET_OF( modelSimpleVertex_t, texCoord ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT2, 1 },
		{ 0, OFFSET_OF( modelSimpleVertex_t, color ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_BYTE4UN, 2 }
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
