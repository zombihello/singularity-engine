#pragma once

/*
==================
CColor::Set
==================
*/
FORCEINLINE void CColor::Set( uint8 red, uint8 green, uint8 blue, uint8 alpha /*= 255*/ )
{
	r = red;
	g = green;
	b = blue;
	a = alpha;
}

/*
==================
CColor::Set
==================
*/
FORCEINLINE void CColor::Set( float red, float green, float blue, float alpha /*= 1.f*/ )
{
	r = ( uint8 )( red * 255.f );
	g = ( uint8 )( green * 255.f );
	b = ( uint8 )( blue * 255.f );
	a = ( uint8 )( alpha * 255.f );
}

/*
==================
CColor::Set
==================
*/
FORCEINLINE void CColor::Set( const vec4_t& linearColor )
{
	Set( linearColor.x, linearColor.y, linearColor.z, linearColor.a );
}

/*
==================
CColor::GetUInt32Color
==================
*/
FORCEINLINE uint32& CColor::GetUInt32Color()
{ 
	return *( uint32* )this;
}

/*
==================
CColor::GetUInt32Color
==================
*/
FORCEINLINE const uint32& CColor::GetUInt32Color() const
{ 
	return *( uint32* )this;
}

/*
==================
CColor::AsVector4D
==================
*/
FORCEINLINE vec4_t CColor::AsVector4D() const
{
	return vec4_t( r, g, b, a );
}

/*
==================
CColor::AsNormalizedVector4D
==================
*/
FORCEINLINE vec4_t CColor::AsNormalizedVector4D() const
{
	return vec4_t( r / 255.f, g / 255.f, b / 255.f, a / 255.f );
}

/*
==================
CColor::operator==
==================
*/
FORCEINLINE bool CColor::operator==( const CColor& other ) const
{
	return r == other.r && g == other.g && b == other.b && a == other.a;
}

/*
==================
CColor::operator!=
==================
*/
FORCEINLINE bool CColor::operator!=( const CColor& other ) const
{
	return !( *this == other );
}
