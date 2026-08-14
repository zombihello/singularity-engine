#include "pch_modelsystem.h"
#include "studiorender/studioapi/istudioapi.h"
#include "modelsystem/vertexfactory_simple.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
CVertexDeclarationSimple g_vertexDeclarationSimple;

/*
==================
CVertexDeclarationSimple::InitStudioAPI
==================
*/
void CVertexDeclarationSimple::InitStudioAPI()
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
CVertexFactorySimple::CVertexFactorySimple
==================
*/
CVertexFactorySimple::CVertexFactorySimple( const char* pDebugName /* = "" */ )
	: CVertexFactoryBase<IVertexFactorySimple>( pDebugName )
{
}

/*
==================
CVertexFactorySimple::GetStudioAPIVertexDeclaration
==================
*/
IStudioAPIVertexDeclaration* CVertexFactorySimple::GetStudioAPIVertexDeclaration() const
{
	return g_vertexDeclarationSimple.GetStudioAPIVertexDeclaration();
}
