#pragma once
#include "tier1/math/math.h"
#include "tier1/math/color.h"

//-----------------------------------------------------------------------------
// Model vertex and index types
//-----------------------------------------------------------------------------
enum modelVertexType_t
{
	MODEL_VERTEXTYPE_SIMPLE,
	MODEL_VERTEXTYPE_STATIC,
	MODEL_VERTEX_NUM_TYPES
};

enum modelIndexType_t
{
	MODEL_INDEXTYPE_UINT16,
	MODEL_INDEXTYPE_UINT32,
	MODEL_INDEX_NUM_TYPES
};

//-----------------------------------------------------------------------------
// Model surface
//-----------------------------------------------------------------------------
struct modelSurface_t
{
	uint32 materialId;
	uint32 baseVertexIndex;
	uint32 baseIndex;
	uint32 numIndices;
};

//-----------------------------------------------------------------------------
// Model simple vertex
//-----------------------------------------------------------------------------
struct modelSimpleVertex_t
{
	bool operator==( const modelSimpleVertex_t& other ) const;

	vector4_t position;
	vector2_t texCoord;
	CColor	  color;
};

//-----------------------------------------------------------------------------
// Model static vertex
//-----------------------------------------------------------------------------
struct modelStaticVertex_t
{
	bool operator==( const modelStaticVertex_t& other ) const;

	vector4_t position;
	vector2_t texCoord;
	vector4_t normal;
	vector4_t tangent;
	vector4_t binormal;
};

#include "modelsystem/modeltypes.inl"