#pragma once
#include "stdlib/stdlib.h"

//-----------------------------------------------------------------------------
// Template functions
//-----------------------------------------------------------------------------
template<typename TType>
TType Max( const TType a, const TType b );

template<typename TType>
TType Min( const TType a, const TType b );

template<typename TType>
TType Align( const TType value, uint64 alignment );

template<typename TType>
TType Clamp( const TType x, const TType min, const TType max );

template<typename TType>
void Swap( TType& a, TType& b );

//-----------------------------------------------------------------------------
// Template classes
//-----------------------------------------------------------------------------
// Inherit from this class to prevent your class from being copied
class CNonCopyable
{
protected:
	CNonCopyable() {}
	CNonCopyable( CNonCopyable&& other ) {}
	~CNonCopyable() {}

	CNonCopyable& operator=( CNonCopyable&& other ) { return *this; }

private:
	CNonCopyable( const CNonCopyable& other ) {}
	CNonCopyable& operator=( const CNonCopyable& other ) { return *this; }
};

// Exception-safe guard around saving/restoring a value
template<typename TType>
class TGuardValue : private CNonCopyable
{
	TGuardValue( TType& refValue, const TType& newValue )
		: value( refValue )
		, oldValue( newValue )
	{
		value = newValue;
	}
	TGuardValue( TType& refValue )
		: value( refValue )
		, oldValue( refValue )
	{
	}
	~TGuardValue()
	{
		value = oldValue;
	}

	const TType& operator*() const;

private:
	TType& value;
	TType  oldValue;
};

#include "stdlib/template.inl"