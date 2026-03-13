#pragma once

/*
==================
rect_t::rect_t
==================
*/
template<typename TType>
FORCEINLINE rect_t<TType>::rect_t()
{
}

/*
==================
rect_t::rect_t
==================
*/
template<typename TType>
FORCEINLINE rect_t<TType>::rect_t( const TType& x, const TType& y, const TType& width, const TType& height )
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
template<typename TType>
FORCEINLINE rect_t<TType>::rect_t( const rect_t& copy )
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
template<typename TType>
FORCEINLINE rect_t<TType> rect_t<TType>::Make( const TType& x, const TType& y, const TType& width, const TType& height )
{
	return rect_t( x, y, width, height );
}

/*
==================
rect_t::operator=
==================
*/
template<typename TType>
FORCEINLINE rect_t<TType>& rect_t<TType>::operator=( const rect_t& right )
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
template<typename TType>
FORCEINLINE bool rect_t<TType>::operator==( const rect_t& right ) const
{
	return x == right.x && y == right.y && width == right.width && height == right.height;
}

/*
==================
rect_t::operator==
==================
*/
template<typename TType>
FORCEINLINE bool rect_t<TType>::operator!=( const rect_t& right ) const
{
	return x != right.x || y != right.y || width != right.width || height != right.height;
}
