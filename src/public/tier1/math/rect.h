#pragma once
#include "tier1/math/math.h"

template<typename TType>
struct rect_t
{
public:
	rect_t();
	rect_t( const TType& x, const TType& y, const TType& width, const TType& height );
	rect_t( const rect_t& copy );

	static rect_t Make( const TType& x, const TType& y, const TType& width, const TType& height );

	rect_t& operator=( const rect_t& right );
	bool	operator==( const rect_t& right ) const;
	bool	operator!=( const rect_t& right ) const;

	TType x;
	TType y;
	TType width;
	TType height;
};

#include "tier1/math/rect.inl"
