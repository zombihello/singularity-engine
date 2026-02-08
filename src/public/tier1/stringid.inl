#pragma once

/*
==================
CStringID::keyFunc_t::operator()
==================
*/
FORCEINLINE size_t CStringID::keyFunc_t::operator()( const CStringID& stringID ) const
{
	return stringID.GetHash();
}

/*
==================
CStringID::keyFunc_t::operator()
==================
*/
FORCEINLINE bool CStringID::keyFunc_t::operator()( const CStringID& a, const CStringID& b ) const
{
	return a.id < b.id;
}

/*
==================
CStringID::CStringID
==================
*/
FORCEINLINE CStringID::CStringID()
	: id( INVALID_INDEX )
{
}

/*
==================
CStringID::CStringID
==================
*/
FORCEINLINE CStringID::CStringID( const char* pString )
	: id( GetStringPool().FindOrAdd( pString, S_Strlen( pString ) ) )
{
}

/*
==================
CStringID::CStringID
==================
*/
FORCEINLINE CStringID::CStringID( const char* pString, uint64 length )
	: id( GetStringPool().FindOrAdd( pString, length ) )
{
}

/*
==================
CStringID::CStringID
==================
*/
FORCEINLINE CStringID::CStringID( const eastl::string& string )
	: id( GetStringPool().FindOrAdd( string.c_str(), string.size() ) )
{
}

/*
==================
CStringID::CStringID
==================
*/
FORCEINLINE CStringID::CStringID( const eastl::string_view& string )
	: id( GetStringPool().FindOrAdd( string.data(), string.size() ) )
{
}

/*
==================
CStringID::CStringID
==================
*/
FORCEINLINE CStringID::CStringID( const CStringID& copy )
	: id( copy.id )
{
}

/*
==================
CStringID::Make
==================
*/
FORCEINLINE CStringID CStringID::Make( const char* pString )
{
	return CStringID( pString );
}

/*
==================
CStringID::Make
==================
*/
FORCEINLINE CStringID CStringID::Make( const char* pString, uint64 length )
{
	return CStringID( pString, length );
}

/*
==================
CStringID::Make
==================
*/
FORCEINLINE CStringID CStringID::Make( const eastl::string& string )
{
	return CStringID( string );
}

/*
==================
CStringID::Make
==================
*/
FORCEINLINE CStringID CStringID::Make( const eastl::string_view& string )
{
	return CStringID( string );
}

/*
==================
CStringID::Make
==================
*/
FORCEINLINE CStringID CStringID::Make( const CStringID& stringId )
{
	return CStringID( stringId );
}

/*
==================
CStringID::Clear
==================
*/
FORCEINLINE void CStringID::Clear()
{
	id = INVALID_INDEX;
}

/*
==================
CStringID::ToString
==================
*/
FORCEINLINE const char* CStringID::ToString() const
{
	return !IsEmpty() ? GetStringPool().GetString( id ) : "";
}

/*
==================
CStringID::IsEmpty
==================
*/
FORCEINLINE bool CStringID::IsEmpty() const
{
	return id == INVALID_INDEX;
}

/*
==================
CStringID::GetHash
==================
*/
FORCEINLINE hash_t CStringID::GetHash() const
{
	return FastHash( *this );
}

/*
==================
CStringID::operator==
==================
*/
FORCEINLINE bool CStringID::operator==( const char* pString ) const
{
	return GetStringPool().Find( pString, S_Strlen( pString ) ) == id;
}

/*
==================
CStringID::operator==
==================
*/
FORCEINLINE bool CStringID::operator==( const eastl::string& string ) const
{
	return operator==( string.c_str() );
}

/*
==================
CStringID::operator==
==================
*/
FORCEINLINE bool CStringID::operator==( const eastl::string_view& string ) const
{
	return GetStringPool().Find( string.data(), string.size() ) == id;
}

/*
==================
CStringID::operator==
==================
*/
FORCEINLINE bool CStringID::operator==( const CStringID& stringId ) const
{
	return id == stringId.id;
}

/*
==================
CStringID::operator!=
==================
*/
FORCEINLINE bool CStringID::operator!=( const char* pString ) const
{
	return GetStringPool().Find( pString, S_Strlen( pString ) ) != id;
}

/*
==================
CStringID::operator!=
==================
*/
FORCEINLINE bool CStringID::operator!=( const eastl::string& string ) const
{
	return operator!=( string.c_str() );
}

/*
==================
CStringID::operator!=
==================
*/
FORCEINLINE bool CStringID::operator!=( const eastl::string_view& string ) const
{
	return GetStringPool().Find( string.data(), string.size() ) != id;
}

/*
==================
CStringID::operator!=
==================
*/
FORCEINLINE bool CStringID::operator!=( const CStringID& stringId ) const
{
	return id != stringId.id;
}

/*
==================
CStringID::operator=
==================
*/
FORCEINLINE CStringID& CStringID::operator=( const char* pString )
{
	id = GetStringPool().FindOrAdd( pString, S_Strlen( pString ) );
	return *this;
}

/*
==================
CStringID::operator=
==================
*/
FORCEINLINE CStringID& CStringID::operator=( const eastl::string& string )
{
	id = GetStringPool().FindOrAdd( string.c_str(), string.size() );
	return *this;
}

/*
==================
CStringID::operator=
==================
*/
FORCEINLINE CStringID& CStringID::operator=( const eastl::string_view& string )
{
	id = GetStringPool().FindOrAdd( string.data(), string.size() );
	return *this;
}

/*
==================
CStringID::operator=
==================
*/
FORCEINLINE CStringID& CStringID::operator=( const CStringID& stringId )
{
	id = stringId.id;
	return *this;
}
