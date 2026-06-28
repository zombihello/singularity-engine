#pragma once

/*
==================
CVertexDeclarations::GetStudioAPIVertexDeclaration
==================
*/
FORCEINLINE IStudioAPIVertexDeclaration* CVertexDeclarations::GetStudioAPIVertexDeclaration( modelVertexType_t vertexType ) const
{
	Assert( vertexType < MODEL_VERTEX_NUM_TYPES );
	return pStudioAPIVertexDeclarations[vertexType];
}