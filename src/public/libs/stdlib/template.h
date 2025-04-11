/**
 * @file
 * @addtogroup stdlib stdlib
 */

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "stdlib/stdlib.h"

// Undo any defines
#undef ARRAYSIZE
#undef OFFSET_OF

/**
 * @ingroup stdlib
 * @brief Macro for calculate count elements in array
 *
 * @param Array		Array
 */
#define ARRAYSIZE( Array )											( sizeof( Array ) / sizeof( ( Array )[0] ) )

/**
 * @ingroup stdlib
 * @brief Macro for calculate offset to member in struct
 *
 * @param Struc		Struct name
 * @param Member	Member name
 */
#define OFFSET_OF( Struc, Member )									( ( ptrint )&( ( Struc* )0 )->Member )

/**
 * @ingroup stdlib
 * @brief Help macro to pack version into uint32
 * @param Major		Major version (8 bit)
 * @param Minor		Minor version (8 bit)
 * @param Patch		Patch version (16 bit)
 */
#define PACK_VERSION( Major, Minor, Patch )							( ( uint32 )Major << 24 | ( uint32 )Minor << 16 | Patch )

/**
 * @ingroup stdlib
 * @brief Help macro to unpack version from uint32
 * @param Version	Packed version
 * @param Major		Output major version (8 bit)
 * @param Minor		Output minor version (8 bit)
 * @param Patch		Output patch version (16 bit)
 */
#define UNPACK_VERSION( Version, Major, Minor, Patch )				{ Major = ( ( uint32 )Version >> 24 ) & 0xFF; Minor = ( ( uint32 )Version >> 16 ) & 0xFF; Patch = ( uint32 )Version & 0xFFFF; }

/**
 * @ingroup stdlib
 * @brief Find the maximum between two numbers
 *
 * @param a		First value
 * @param b		Second value
 * @return Return maximum value 'a' or 'b'
 */
template<typename TType>
FORCEINLINE TType Max( const TType a, const TType b )
{
	return ( a >= b ) ? a : b;
}

/**
 * @ingroup stdlib
 * @brief Find the minimum between two numbers
 *
 * @param a		First value
 * @param b		Second value
 * @return Return minimum value 'a' or 'b'
 */
template<typename TType>
FORCEINLINE TType Min( const TType a, const TType b )
{
	return ( a <= b ) ? a : b;
}

/**
 * @ingroup stdlib
 * @brief Aligns a value to the nearest higher multiple of 'alignment', which must be a power of two
 *
 * @param value			Value to align
 * @param alignment		Alignment, must be a power of two
 * @return Aligned value
 */
template<typename TType>
FORCEINLINE TType Align( const TType value, uint64 alignment )
{
	return ( TType )( ( ( ptrint )value + alignment - 1 ) & ~( ( ptrint )alignment - 1 ) );
}

/**
 * @ingroup stdlib
 * @brief Clamp value in range Min and Max
 *
 * @param x		Value
 * @param min	Min range
 * @param max	Max range
 * @return Return clamped value
 */
template<typename TType>
FORCEINLINE TType Clamp( const TType x, const TType min, const TType max )
{
	return x < min ? min : x < max ? x : max;
}

/**
 * @ingroup stdlib
 * @brief Swap A and B
 *
 * @param a		Value A
 * @param b		Value B
 */
template<typename TType>
FORCEINLINE void Swap( TType& a, TType& b )
{
	const TType	temp = a;
	a = b;
	b = temp;
}

/**
 * @ingroup stdlib
 * @brief Inherit from this class to prevent your class from being copied
 */
class CNonCopyable
{
protected:
	/**
	 * @brief Constructor
	 */
	CNonCopyable() {}

	/**
	 * @brief Constructor of move
	 * @param other		Other object
	 */
	CNonCopyable( CNonCopyable&& other ) {}

	/**
	 * @brief Destructor
	 */
	~CNonCopyable() {}

	/**
	 * @brief Override operator = for move
	 * @param other		Other object
	 */
	CNonCopyable& operator=( CNonCopyable&& other ) { return *this; }

private:
	/**
	 * @brief Constructor of copy
	 * @param other		Other object
	 */
	CNonCopyable( const CNonCopyable& other ) {}

	/**
	 * @brief Override operator =
	 * @param other		Other object
	 */
	CNonCopyable& operator=( const CNonCopyable& other ) { return *this; }
};

/**
 * @ingroup stdlib
 * @brief Exception-safe guard around saving/restoring a value
 */
template<typename TType>
class TGuardValue : private CNonCopyable
{
	/**
	 * @brief Constructor
	 * @param refValue		Value to change
	 * @param newValue		New value to change
	 */
	TGuardValue( TType& refValue, const TType& newValue )
		: value( refValue )
		, oldValue( newValue )
	{
		value = newValue;
	}

	/**
	 * @brief Constructor
	 * @param refValue	Value to change
	 */
	TGuardValue( TType& refValue )
		: value( refValue )
		, oldValue( refValue )
	{}

	/**
	 * @brief Destructor
	 */
	~TGuardValue()
	{
		value = oldValue;
	}

	/**
	 * @brief Override dereference operator
	 * @return Return a const reference to the original data value
	 */
	FORCEINLINE const TType& operator*() const
	{
		return oldValue;
	}

private:
	TType&	value;
	TType	oldValue;
};

#endif // !TEMPLATE_H