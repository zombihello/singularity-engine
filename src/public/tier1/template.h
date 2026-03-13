#pragma once
#include "tier1/tier1.h"

//-----------------------------------------------------------------------------
// Inherit from this class to prevent your class from being copied
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Exception-safe guard around saving/restoring a value
//-----------------------------------------------------------------------------
template<typename TType>
class CGuardValue : private CNonCopyable
{
public:
	CGuardValue( TType& refValue, const TType& newValue )
		: value( refValue )
		, oldValue( refValue )
	{
		value = newValue;
	}
	CGuardValue( TType& refValue )
		: value( refValue )
		, oldValue( refValue )
	{
	}
	~CGuardValue()
	{
		value = oldValue;
	}

	const TType& operator*() const;

private:
	TType& value;
	TType  oldValue;
};

#include "tier1/template.inl"
