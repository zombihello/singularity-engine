#pragma once

/*
==================
stlInsensitiveStringHash_t::operator()
==================
*/
template<typename TType>
FORCEINLINE size_t stlInsensitiveStringHash_t::operator()( const TType* pString ) const
{
	return FastHashStringInsensitive( pString );
}

/*
==================
stlInsensitiveStringHash_t::operator()
==================
*/
template<typename TType>
FORCEINLINE size_t stlInsensitiveStringHash_t::operator()( const eastl::basic_string<TType>& string ) const
{
	return FastHashStringInsensitive( string );
}

/*
==================
stlInsensitiveStringHash_t::operator()
==================
*/
template<typename TType>
FORCEINLINE size_t stlInsensitiveStringHash_t::operator()( const eastl::basic_string_view<TType>& string ) const
{
	return FastHashStringInsensitive( string );
}

/*
==================
stlInsensitiveCompareString_t::operator()
==================
*/
template<typename TType>
FORCEINLINE bool stlInsensitiveCompareString_t::operator()( const TType* pLeft, const TType* pRight ) const
{
	return !S_Stricmp( pLeft, pRight );
}

/*
==================
stlInsensitiveCompareString_t::operator()
==================
*/
template<typename TType>
FORCEINLINE bool stlInsensitiveCompareString_t::operator()( const eastl::basic_string<TType>& left, const eastl::basic_string<TType>& right ) const
{
	return !S_Stricmp( left.c_str(), right.c_str() );
}

/*
==================
stlInsensitiveCompareString_t::operator()
==================
*/
template<typename TType>
FORCEINLINE bool stlInsensitiveCompareString_t::operator()( const eastl::basic_string_view<TType>& left, const eastl::basic_string_view<TType>& right ) const
{
	return left.size() == right.size() && !S_Strnicmp( left.data(), right.data(), left.size() );
}
