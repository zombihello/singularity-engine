#pragma once
#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier1/math/math.h"
#include "tier1/math/color.h"

//-----------------------------------------------------------------------------
// Studio vertex types
//-----------------------------------------------------------------------------
enum studioVertexType_t
{
	STUDIO_VERTEXTYPE_SIMPLEELEMENT,
	STUDIO_VERTEXTYPE_MODEL,
	STUDIO_VERTEX_NUM_TYPES
};

//-----------------------------------------------------------------------------
// Studio simple element vertex
//-----------------------------------------------------------------------------
struct studioSimpleElementVertex_t
{
	FORCEINLINE bool operator==( const studioSimpleElementVertex_t& other ) const
	{
		return position == other.position && texCoord == other.texCoord && color == other.color;
	}

	vector4_t position;
	vector2_t texCoord;
	CColor color;
};

//-----------------------------------------------------------------------------
// Studio model vertex
//-----------------------------------------------------------------------------
struct studioModelVertex_t
{
	FORCEINLINE bool operator==( const studioModelVertex_t& other ) const
	{
		return position == other.position && texCoord == other.texCoord && normal == other.normal && tangent == other.tangent && binormal == other.binormal;
	}

	vector4_t position;
	vector2_t texCoord;
	vector4_t normal;
	vector4_t tangent;
	vector4_t binormal;
};