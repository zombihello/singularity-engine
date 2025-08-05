#pragma once

/*
==================
FastHash
==================
*/
FORCEINLINE hash_t FastHash( const void* pData, uint64 size, hash_t hash /*= 0*/ )
{
	byte*	pLocalData = ( byte* )pData;
	for ( uint64 index = 0; index < size; ++index )
	{
		hash = pLocalData[index] + ( hash << 6 ) + ( hash << 16 ) - hash;
	}

	return hash;
}

/*
==================
FastHashFromString
==================
*/
FORCEINLINE hash_t FastHashFromString( const void* pData, uint64 size, hash_t hash /*= 0*/ )
{
	byte*		pLocalData = ( byte* )pData;
	for ( uint64 index = 0; index < size; ++index )
	{
		hash = S_ToLower( pLocalData[index] ) + ( hash << 6 ) + ( hash << 16 ) - hash;
	}

	return hash;
}

/*
==================
FastHash
==================
*/
template<typename TType>
FORCEINLINE hash_t FastHash( const TType& value, hash_t hash /*= 0*/ )
{
	return FastHash( &value, sizeof( value ), hash );
}

/*
==================
FastHashFromString
==================
*/
FORCEINLINE hash_t FastHashFromString( const wchar* pString, hash_t hash /*= 0*/ )
{
	return FastHashFromString( pString, S_Strlen( pString ) * sizeof( wchar ), hash );
}

/*
==================
FastHashFromString
==================
*/
FORCEINLINE hash_t FastHashFromString( const std::wstring& string, hash_t hash /*= 0*/ )
{
	return FastHashFromString( string.data(), ( uint64 )string.size() * sizeof( wchar ), hash );
}

/*
==================
FastHashFromString
==================
*/
FORCEINLINE hash_t FastHashFromString( const achar* pString, hash_t hash /*= 0*/ )
{
	return FastHashFromString( pString, S_Strlen( pString ) * sizeof( achar ), hash );
}

/*
==================
FastHashFromString
==================
*/
FORCEINLINE hash_t FastHashFromString( const std::string& string, hash_t hash /*= 0*/ )
{
	return FastHashFromString( string.data(), ( uint64 )string.size() * sizeof( achar ), hash );
}