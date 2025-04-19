#ifndef STUDIO_VERTEXTYPES_H
#define STUDIO_VERTEXTYPES_H

#include "core/types.h"
#include "core/platform.h"
#include "stdlib/math/math.h"
#include "stdlib/math/color.h"

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
		return 
			position == other.position &&
			texCoord == other.texCoord &&
			color == other.color;
	}

	vec4_t		position;
	vec2_t		texCoord;
	CColor		color;
};


//-----------------------------------------------------------------------------
// Studio model vertex
//-----------------------------------------------------------------------------
struct studioModelVertex_t
{
	FORCEINLINE bool operator==( const studioModelVertex_t& other ) const
	{
		return 
			position == other.position &&
			texCoord == other.texCoord && 
			normal == other.normal &&
			tangent == other.tangent &&
			binormal == other.binormal;
	}

	vec4_t		position;
	vec2_t		texCoord;
	vec4_t		normal;
	vec4_t		tangent;
	vec4_t		binormal;
};

#endif // !STUDIO_VERTEXTYPES_H