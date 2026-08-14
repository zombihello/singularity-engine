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
		{ STREAM_SLOT_VERTEX, sizeof( modelStaticVertex_t ), STUDIOAPI_VERTEX_INPUT_RATE_VERTEX },
		{ STREAM_SLOT_INSTANCE, sizeof( modelStaticInstance_t ), STUDIOAPI_VERTEX_INPUT_RATE_INSTANCE }
	};
	studioAPIVertexInputAttribute_t studioAPIVertexInputAttributes[] = {
		{ STREAM_SLOT_VERTEX, OFFSET_OF( modelStaticVertex_t, position ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4, 0 },
		{ STREAM_SLOT_VERTEX, OFFSET_OF( modelStaticVertex_t, texCoord ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT2, 1 },
		{ STREAM_SLOT_VERTEX, OFFSET_OF( modelStaticVertex_t, normal ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4, 2 },
		{ STREAM_SLOT_VERTEX, OFFSET_OF( modelStaticVertex_t, tangent ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4, 3 },
		{ STREAM_SLOT_VERTEX, OFFSET_OF( modelStaticVertex_t, binormal ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4, 4 },
		{ STREAM_SLOT_INSTANCE, OFFSET_OF( modelStaticInstance_t, localToWorld ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT3, 5 },
		{ STREAM_SLOT_INSTANCE, OFFSET_OF( modelStaticInstance_t, localToWorld ) + sizeof( vector3_t ), STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT3, 6 },
		{ STREAM_SLOT_INSTANCE, OFFSET_OF( modelStaticInstance_t, localToWorld ) + sizeof( vector3_t ) * 2, STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT3, 7 },
		{ STREAM_SLOT_INSTANCE, OFFSET_OF( modelStaticInstance_t, localToWorld ) + sizeof( vector3_t ) * 3, STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT3, 8 }
	};
	pStudioAPIVertexDeclaration = g_pStudioAPI->CreateVertexDeclaration( studioAPIVertexInputBuffers, ARRAYSIZE( studioAPIVertexInputBuffers ),
																		 studioAPIVertexInputAttributes, ARRAYSIZE( studioAPIVertexInputAttributes ) );
}

/*
==================
CVertexFactoryStatic::CVertexFactoryStatic
==================
*/
CVertexFactoryStatic::CVertexFactoryStatic( const char* pDebugName /* = "" */ )
	: CVertexFactoryBase<IVertexFactoryStatic, CVertexDeclarationStatic::STREAM_SLOT_INSTANCE>( pDebugName )
{
}

/*
==================
CVertexFactoryStatic::R_UpdateInstanceBuffer
==================
*/
void CVertexFactoryStatic::R_UpdateInstanceBuffer( byte* pData, const byte* pInstances, uint32 numInstances ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Mem_Memcpy( pData, pInstances, numInstances * sizeof( modelStaticInstance_t ) );
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
