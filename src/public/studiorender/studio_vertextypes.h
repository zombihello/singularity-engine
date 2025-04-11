/**
 * @file
 * @addtogroup studiorender studiorender
 */

#ifndef STUDIO_VERTEXTYPES_H
#define STUDIO_VERTEXTYPES_H

#include "core/types.h"
#include "core/platform.h"
#include "stdlib/math/math.h"
#include "stdlib/math/color.h"

/**
 * @ingroup studiorender
 * @brief Studio vertex types
 */
enum studioVertexType_t
{
	STUDIO_VERTEXTYPE_SIMPLEELEMENT,	/**< Simple element */
	STUDIO_VERTEXTYPE_MODEL,			/**< Model */
	STUDIO_VERTEX_NUM_TYPES				/**< Vertex types number */
};

/**
 * @ingroup studiorender
 * @brief Studio simple element vertex
 */
struct studioSimpleElementVertex_t
{
	/**
	 * @brief Overload operator ==
	 * @param other		Other simple element to compare
	 * @return Return TRUE if both elements are equal, otherwise FALSE
	 */
	FORCEINLINE bool operator==( const studioSimpleElementVertex_t& other ) const
	{
		return 
			position == other.position &&
			texCoord == other.texCoord &&
			color == other.color;
	}

	vec4_t		position;		/**< Position vertex */
	vec2_t		texCoord;		/**< Texture coords */
	CColor		color;			/**< Color */
};

/**
 * @ingroup studiorender
 * @brief Studio model vertex
 */
struct studioModelVertex_t
{
	/**
	 * @brief Overload operator ==
	 * @param other		Other model vertex to compare
	 * @return Return TRUE if both elements are equal, otherwise FALSE
	 */
	FORCEINLINE bool operator==( const studioModelVertex_t& other ) const
	{
		return 
			position == other.position &&
			texCoord == other.texCoord && 
			normal == other.normal &&
			tangent == other.tangent &&
			binormal == other.binormal;
	}

	vec4_t		position;		/**< Position vertex */
	vec2_t		texCoord;		/**< Texture coords */
	vec4_t		normal;			/**< Normal */
	vec4_t		tangent;		/**< Tangent */
	vec4_t		binormal;		/**< Binormal */
};

#endif // !STUDIO_VERTEXTYPES_H