#pragma once

/*
==================
stlInsensitiveStringHash_t::operator()
==================
*/
FORCEINLINE std::size_t stlInsensitiveStringHash_t::operator()( const achar* pString ) const
{
	return FastHashFromString( pString );
}


/*
==================
stlInsensitiveCompareString_t::operator()
==================
*/
FORCEINLINE bool stlInsensitiveCompareString_t::operator()( const achar* pLeft, const achar* pRight ) const
{
	return !S_Stricmp( pLeft, pRight );
}