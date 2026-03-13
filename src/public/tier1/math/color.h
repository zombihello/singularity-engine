#pragma once
#include "tier1/math/math.h"

//-----------------------------------------------------------------------------
// Linear color
//-----------------------------------------------------------------------------
class CLinearColor
{
public:
	CLinearColor();
	CLinearColor( float r, float g, float b, float a = 1.f );
	CLinearColor( const CLinearColor& copy );

	static CLinearColor Make( float r, float g, float b, float a = 1.f );
	vector4_t			ToVector4() const;

	CLinearColor& operator=( const CLinearColor& right );
	bool		  operator==( const CLinearColor& right ) const;
	bool		  operator!=( const CLinearColor& right ) const;

	float r;
	float g;
	float b;
	float a;
};

//-----------------------------------------------------------------------------
// RGBA color
//-----------------------------------------------------------------------------
class CColor
{
public:
	CColor();
	CColor( uint8 r, uint8 g, uint8 b, uint8 a = 255 );
	CColor( const CLinearColor& linearColor );
	CColor( uint32 color );
	CColor( const CColor& copy );

	static CColor Make( uint8 r, uint8 g, uint8 b, uint8 a = 255 );
	static CColor Make( const CLinearColor& linearColor );
	static CColor Make( uint32 color );

	uint32		 ToUint32() const;
	CLinearColor ToLinearColor() const;

	CColor& operator=( const CColor& right );
	bool	operator==( const CColor& right ) const;
	bool	operator!=( const CColor& right ) const;

	uint8 r;
	uint8 g;
	uint8 b;
	uint8 a;
};

#include "tier1/math/color.inl"
