#pragma once
#include "tier1/math/math.h"

//-----------------------------------------------------------------------------
// The class for work with a color
//-----------------------------------------------------------------------------
class CColor
{
public:
	CColor()
		: r( 0 )
		, g( 0 )
		, b( 0 )
		, a( 255 )
	{
	}
	CColor( uint8 red, uint8 green, uint8 blue, uint8 alpha = 255 )
	{
		Set( red, green, blue, alpha );
	}
	CColor( const vec4_t& linearColor )
	{
		Set( linearColor );
	}
	CColor( uint32 color )
	{
		GetUInt32Color() = color;
	}

	void Set( uint8 red, uint8 green, uint8 blue, uint8 alpha = 255 );
	void Set( float red, float green, float blue, float alpha = 1.f );
	void Set( const vec4_t& linearColor );

	uint32&		  GetUInt32Color();
	const uint32& GetUInt32Color() const;
	vec4_t		  AsVector4D() const;
	vec4_t		  AsNormalizedVector4D() const;

	bool operator==( const CColor& other ) const;
	bool operator!=( const CColor& other ) const;

	uint8 r;
	uint8 g;
	uint8 b;
	uint8 a;

	static CColor black;
	static CColor white;
	static CColor red;
};

#include "tier1/math/color.inl"
