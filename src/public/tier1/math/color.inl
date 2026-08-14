#pragma once

/*
==================
CLinearColor::CLinearColor
==================
*/
FORCEINLINE CLinearColor::CLinearColor()
{
}

/*
==================
CLinearColor::CLinearColor
==================
*/
FORCEINLINE CLinearColor::CLinearColor( float r, float g, float b, float a /*= 1.f*/ )
	: r( r )
	, g( g )
	, b( b )
	, a( a )
{
}

/*
==================
CLinearColor::CLinearColor
==================
*/
FORCEINLINE CLinearColor::CLinearColor( const CLinearColor& copy )
	: r( copy.r )
	, g( copy.g )
	, b( copy.b )
	, a( copy.a )
{
}

/*
==================
CLinearColor::Make
==================
*/
FORCEINLINE CLinearColor CLinearColor::Make( float r, float g, float b, float a /*= 1.f*/ )
{
	return CLinearColor( r, g, b, a );
}

/*
==================
CLinearColor::ToVector4
==================
*/
FORCEINLINE vector4_t CLinearColor::ToVector4() const
{
	return vector4_t( r, g, b, a );
}

/*
==================
CLinearColor::operator=
==================
*/
FORCEINLINE CLinearColor& CLinearColor::operator=( const CLinearColor& right )
{
	r = right.r;
	g = right.g;
	b = right.b;
	a = right.a;
	return *this;
}

/*
==================
CLinearColor::operator==
==================
*/
FORCEINLINE bool CLinearColor::operator==( const CLinearColor& right ) const
{
	return r == right.r && g == right.g && b == right.b && a == right.a;
}

/*
==================
CLinearColor::operator!=
==================
*/
FORCEINLINE bool CLinearColor::operator!=( const CLinearColor& right ) const
{
	return r != right.r || g != right.g || b != right.b || a != right.a;
}

/*
==================
CColor::CColor
==================
*/
FORCEINLINE CColor::CColor()
{
}

/*
==================
CColor::CColor
==================
*/
FORCEINLINE CColor::CColor( uint8 r, uint8 g, uint8 b, uint8 a /* = 255 */ )
	: r( r )
	, g( g )
	, b( b )
	, a( a )
{
}

/*
==================
CColor::CColor
==================
*/
FORCEINLINE CColor::CColor( const CLinearColor& linearColor )
	: r( (uint8)S_Clamp( linearColor.r * 255.f, 0.f, 255.f ) )
	, g( (uint8)S_Clamp( linearColor.g * 255.f, 0.f, 255.f ) )
	, b( (uint8)S_Clamp( linearColor.b * 255.f, 0.f, 255.f ) )
	, a( (uint8)S_Clamp( linearColor.a * 255.f, 0.f, 255.f ) )
{
}

/*
==================
CColor::CColor
==================
*/
FORCEINLINE CColor::CColor( uint32 color )
	: r( (uint8)( color & 0xFF ) )
	, g( (uint8)( ( color >> 8 ) & 0xFF ) )
	, b( (uint8)( ( color >> 16 ) & 0xFF ) )
	, a( (uint8)( ( color >> 24 ) & 0xFF ) )
{
}

/*
==================
CColor::CColor
==================
*/
FORCEINLINE CColor::CColor( const CColor& copy )
	: r( copy.r )
	, g( copy.g )
	, b( copy.b )
	, a( copy.a )
{
}

/*
==================
CColor::Make
==================
*/
FORCEINLINE CColor CColor::Make( uint8 r, uint8 g, uint8 b, uint8 a /* = 255 */ )
{
	return CColor( r, g, b, a );
}

/*
==================
CColor::Make
==================
*/
FORCEINLINE CColor CColor::Make( const CLinearColor& linearColor )
{
	return CColor( linearColor );
}

/*
==================
CColor::Make
==================
*/
FORCEINLINE CColor CColor::Make( uint32 color )
{
	return CColor( color );
}

/*
==================
CColor::ToUint32
==================
*/
FORCEINLINE uint32 CColor::ToUint32() const
{
	return *(uint32*)this;
}

/*
==================
CColor::ToLinearColor
==================
*/
FORCEINLINE CLinearColor CColor::ToLinearColor() const
{
	return CLinearColor( r / 255.f, g / 255.f, b / 255.f, a / 255.f );
}

/*
==================
CColor::operator=
==================
*/
FORCEINLINE CColor& CColor::operator=( const CColor& right )
{
	*(uint32*)this = right.ToUint32();
	return *this;
}

/*
==================
CColor::operator==
==================
*/
FORCEINLINE bool CColor::operator==( const CColor& right ) const
{
	return r == right.r && g == right.g && b == right.b && a == right.a;
}

/*
==================
CColor::operator!=
==================
*/
FORCEINLINE bool CColor::operator!=( const CColor& right ) const
{
	return r != right.r || g != right.g || b != right.b || a != right.a;
}
