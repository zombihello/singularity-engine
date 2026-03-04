#pragma once
#include "tier1/math/math.h"

template<typename T>
struct rect_t
{
public:
	rect_t();
	rect_t( const T& x, const T& y, const T& width, const T& height );
	rect_t( const rect_t& copy );

	static rect_t Make( const T& x, const T& y, const T& width, const T& height );

	rect_t& operator=( const rect_t& right );
	bool	operator==( const rect_t& right ) const;
	bool	operator!=( const rect_t& right ) const;

	T x;
	T y;
	T width;
	T height;
};

#include "tier1/math/rect.inl"
