#pragma once

/*
==================
rect_t::rect_t
==================
*/
template<typename T>
FORCEINLINE rect_t<T>::rect_t()
{
}

/*
==================
rect_t::rect_t
==================
*/
template<typename T>
FORCEINLINE rect_t<T>::rect_t( const T& x, const T& y, const T& width, const T& height )
	: x( x )
	, y( y )
	, width( width )
	, height( height )
{
}

/*
==================
rect_t::rect_t
==================
*/
template<typename T>
FORCEINLINE rect_t<T>::rect_t( const rect_t& copy )
	: x( copy.x )
	, y( copy.y )
	, width( copy.width )
	, height( copy.height )
{
}

/*
==================
rect_t::Make
==================
*/
template<typename T>
FORCEINLINE rect_t<T> rect_t<T>::Make( const T& x, const T& y, const T& width, const T& height )
{
	return rect_t( x, y, width, height );
}

/*
==================
rect_t::operator=
==================
*/
template<typename T>
FORCEINLINE rect_t<T>& rect_t<T>::operator=( const rect_t& right )
{
	x	   = right.x;
	y	   = right.y;
	width  = right.width;
	height = right.height;
	return *this;
}

/*
==================
rect_t::operator==
==================
*/
template<typename T>
FORCEINLINE bool rect_t<T>::operator==( const rect_t& right ) const
{
	return x == right.x && y == right.y && width == right.width && height == right.height;
}

/*
==================
rect_t::operator==
==================
*/
template<typename T>
FORCEINLINE bool rect_t<T>::operator!=( const rect_t& right ) const
{
	return x != right.x || y != right.y || width != right.width || height != right.height;
}
