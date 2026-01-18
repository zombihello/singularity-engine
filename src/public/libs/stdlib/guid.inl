#pragma once

/*
==================
CGuid::keyFunc_t::operator()
==================
*/
FORCEINLINE std::size_t CGuid::keyFunc_t::operator()( const CGuid& guid ) const
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
CGuid::IsValid
==================
*/
FORCEINLINE bool CGuid::IsValid() const
{
	return ( a | b | c | d ) != 0;
}

/*
==================
CGuid::Invalidate
==================
*/
FORCEINLINE void CGuid::Invalidate()
{
	a = b = c = d = 0;
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
CGuid::AsString
==================
*/
FORCEINLINE std::string CGuid::AsString() const
{
	return S_Sprintf( "%08X%08X%08X%08X", a, b, c, d );
}

/*
==================
CGuid::InitFromString
==================
*/
FORCEINLINE bool CGuid::InitFromString( const std::string& string )
{
	// Size matches, try to parse it
	bool bSuccessful = false;
	if ( string.size() == 32 )
	{
		S_Sscanf( string.c_str(), "%08X%08X%08X%08X", &a, &b, &c, &d );
		bSuccessful = true;
	}
	// Size mis-match, invalidate the Guid
	else
	{
		Invalidate();
	}
	return bSuccessful;
}

/*
==================
CGuid::GetHash
==================
*/
FORCEINLINE hash_t CGuid::GetHash() const
{
	return FastHash( *this );
}
