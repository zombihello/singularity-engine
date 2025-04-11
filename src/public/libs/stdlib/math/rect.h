/**
 * @file
 * @addtogroup stdlib stdlib
 */

#ifndef RECT_H
#define RECT_H

#include "stdlib/math/math.h"

/**
 * @ingroup stdlib
 * @brief The class for work with rect
 */
template<typename TType>
struct rect_t
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE rect_t()
	{}

	/**
	 * @brief Constructor
	 *
	 * @param left		Coord by X
	 * @param top		Cooed by Y
	 * @param width		Width of rect
	 * @param height	Height of rect
	 */
	FORCEINLINE rect_t( const TType& left, const TType& top, const TType& width, const TType& height )
		: left( left )
		, top( top )
		, width( width )
		, height( height )
	{}

	/**
	 * @brief Constructor of copy
	 * @param copy	Copy of object
	 */
	FORCEINLINE rect_t( const rect_t< TType >& copy )
		: left( copy.left )
		, top( copy.top )
		, width( copy.width )
		, height( copy.height )
	{}


	/**
	 * @brief Override of operator copy
	 */
	FORCEINLINE rect_t<TType>& operator=( const rect_t<TType>& other )
	{
		left = other.left;
		top = other.top;
		width = other.width;
		height = other.height;
		return *this;
	}

	/**
	 * @brief Override of operator copy
	 */
	FORCEINLINE const rect_t<TType>& operator=( const rect_t<TType>& other ) const
	{
		left = other.left;
		top = other.top;
		width = other.width;
		height = other.height;
		return *this;
	}

	TType	left;		/**< Coord by X */
	TType	top;		/**< Cooed by Y */
	TType	width;		/**< Width of rect */
	TType	height;		/**< Height of rect */
};

/**
 * @ingroup stdlib
 * @brief Typedef of rect with type int32
 */
typedef rect_t<int32>		recti_t;

/**
 * @ingroup stdlib
 * @brief Typedef of rect with type float
 */
typedef rect_t<float>		rectf_t;

#endif // !COLOR_H
