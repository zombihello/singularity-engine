#pragma once
#include "tier1/math/math.h"

//-----------------------------------------------------------------------------
// The class for work with rect
//-----------------------------------------------------------------------------
template<typename TType>
struct rect_t
{
public:
	rect_t()
	{
	}
	rect_t( const TType& left, const TType& top, const TType& width, const TType& height )
		: left( left )
		, top( top )
		, width( width )
		, height( height )
	{
	}
	rect_t( const rect_t<TType>& copy )
		: left( copy.left )
		, top( copy.top )
		, width( copy.width )
		, height( copy.height )
	{
	}

	rect_t<TType>&		 operator=( const rect_t<TType>& other );
	const rect_t<TType>& operator=( const rect_t<TType>& other ) const;

	TType left;
	TType top;
	TType width;
	TType height;
};

//-----------------------------------------------------------------------------
// Some rect types
//-----------------------------------------------------------------------------
typedef rect_t<int32> recti_t;
typedef rect_t<float> rectf_t;

#include "tier1/math/rect.inl"
