#pragma once

/*
==================
rect_t::operator=
==================
*/
template<typename TType>
FORCEINLINE rect_t<TType>& rect_t<TType>::operator=( const rect_t<TType>& other )
{
	left	= other.left;
	top		= other.top;
	width	= other.width;
	height	= other.height;
	return *this;
}

/*
==================
rect_t::operator=
==================
*/
template<typename TType>
FORCEINLINE const rect_t<TType>& rect_t<TType>::operator=( const rect_t<TType>& other ) const
{
	left	= other.left;
	top		= other.top;
	width	= other.width;
	height	= other.height;
	return *this;
}
