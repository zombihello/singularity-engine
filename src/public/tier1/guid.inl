#pragma once

/*
==================
CGuid::CGuid
==================
*/
FORCEINLINE CGuid::CGuid()
	: a( 0 )
	, b( 0 )
	, c( 0 )
	, d( 0 )
{
}

/*
==================
CGuid::CGuid
==================
*/
FORCEINLINE CGuid::CGuid( uint32 a, uint32 b, uint32 c, uint32 d )
	: a( a )
	, b( b )
	, c( c )
	, d( d )
{
}

/*
==================
CGuid::CGuid
==================
*/
FORCEINLINE CGuid::CGuid( const CGuid& other )
	: a( other.a )
	, b( other.b )
	, c( other.c )
	, d( other.d )
{
}

/*
==================
CGuid::keyFunc_t::operator()
==================
*/
FORCEINLINE size CGuid::keyFunc_t::operator()( const CGuid& guid ) const
{
	return guid.GetHash();
}

/*
==================
CGuid::keyFunc_t::operator()
==================
*/
FORCEINLINE bool CGuid::keyFunc_t::operator()( const CGuid& a, const CGuid& b ) const
{
	return a < b;
}

/*
==================
CGuid::Generate
==================
*/
FORCEINLINE CGuid CGuid::Generate()
{
	CGuid guid;
	Generate( guid );
	return guid;
}

/*
==================
CGuid::Set
==================
*/
FORCEINLINE void CGuid::Set( uint32 a, uint32 b, uint32 c, uint32 d )
{
	a = a;
	b = b;
	c = c;
	d = d;
}

/*
==================
CGuid::Set
==================
*/
FORCEINLINE bool CGuid::Set( const char* pString )
{
	// Size matches, try to parse it
	bool bResult = false;
	if ( S_Strlen( pString ) == 32 )
	{
		S_Sscanf( pString, "%08X%08X%08X%08X", &a, &b, &c, &d );
		bResult = true;
	}
	// Size mis-match, clear the guid
	else
	{
		Clear();
	}

	return bResult;
}

/*
==================
CGuid::Set
==================
*/
FORCEINLINE bool CGuid::Set( const wchar_t* pString )
{
	// Size matches, try to parse it
	bool bResult = false;
	if ( S_Strlen( pString ) == 32 )
	{
		S_Sscanf( pString, L"%08X%08X%08X%08X", &a, &b, &c, &d );
		bResult = true;
	}
	// Size mis-match, clear the guid
	else
	{
		Clear();
	}

	return bResult;
}

/*
==================
CGuid::Clear
==================
*/
FORCEINLINE void CGuid::Clear()
{
	a = b = c = d = 0;
}

/*
==================
CGuid::IsValid
==================
*/
FORCEINLINE bool CGuid::IsValid() const
{
	return ( a | b | c | d ) != 0;
}

/*
==================
CGuid::AsString
==================
*/
FORCEINLINE eastl::string CGuid::ToString() const
{
	return S_Sprintf( "%08X%08X%08X%08X", a, b, c, d );
}

/*
==================
CGuid::GetHash
==================
*/
FORCEINLINE hash CGuid::GetHash() const
{
	return FastHash( *this );
}

/*
==================
operator==
==================
*/
FORCEINLINE bool operator==( const CGuid& x, const CGuid& y )
{
	return ( ( x.a ^ y.a ) | ( x.b ^ y.b ) | ( x.c ^ y.c ) | ( x.d ^ y.d ) ) == 0;
}

/*
==================
operator!=
==================
*/
FORCEINLINE bool operator!=( const CGuid& x, const CGuid& y )
{
	return ( ( x.a ^ y.a ) | ( x.b ^ y.b ) | ( x.c ^ y.c ) | ( x.d ^ y.d ) ) != 0;
}

/*
==================
CGuid::operator<
==================
*/
FORCEINLINE bool CGuid::operator<( const CGuid& other ) const
{
	if ( a < other.a )
	{
		return true;
	}
	if ( b < other.b )
	{
		return true;
	}
	if ( c < other.c )
	{
		return true;
	}
	if ( d < other.d )
	{
		return true;
	}
	return false;
}
