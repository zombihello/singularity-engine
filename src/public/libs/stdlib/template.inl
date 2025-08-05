#pragma once

/*
==================
Max
==================
*/
template<typename TType>
FORCEINLINE TType Max( const TType a, const TType b )
{
	return ( a >= b ) ? a : b;
}

/*
==================
Min
==================
*/
template<typename TType>
FORCEINLINE TType Min( const TType a, const TType b )
{
	return ( a <= b ) ? a : b;
}

/*
==================
Align
==================
*/
template<typename TType>
FORCEINLINE TType Align( const TType value, uint64 alignment )
{
	return ( TType )( ( ( ptrint )value + alignment - 1 ) & ~( ( ptrint )alignment - 1 ) );
}

/*
==================
Clamp
==================
*/
template<typename TType>
FORCEINLINE TType Clamp( const TType x, const TType min, const TType max )
{
	return x < min ? min : x < max ? x : max;
}

/*
==================
Swap
==================
*/
template<typename TType>
FORCEINLINE void Swap( TType& a, TType& b )
{
	const TType	temp = a;
	a = b;
	b = temp;
}


/*
==================
TGuardValue<TType>::operator*
==================
*/
template<typename TType>
FORCEINLINE const TType& TGuardValue<TType>::operator*() const
{
	return oldValue;
}