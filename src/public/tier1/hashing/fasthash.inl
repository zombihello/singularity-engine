#pragma once

/*
==================
FastHash
==================
*/
FORCEINLINE hash FastHash( const void* pData, uint64 size, hash hash /*= 0*/ )
{
	byte* pLocalData = (byte*)pData;
	for ( uint64 index = 0; index < size; ++index )
	{
		hash = FastHashItem( pLocalData[index], hash );
	}
	return hash;
}

/*
==================
FastHash
==================
*/
template<typename TType>
FORCEINLINE hash FastHash( const TType& data, hash hash )
{
	return FastHash( &data, sizeof( data ), hash );
}

/*
==================
FastHashString
==================
*/
template<typename TType>
FORCEINLINE hash FastHashString( const TType* pString, uint64 length, hash hash /*= 0*/ )
{
	for ( uint64 index = 0, size = length * sizeof( TType ); index < size; ++index )
	{
		hash = FastHashItem( pString[index], hash );
	}
	return hash;
}

/*
==================
FastHashString
==================
*/
template<typename TType>
FORCEINLINE hash FastHashString( const TType* pString, hash hash /*= 0*/ )
{
	return FastHashString( pString, S_Strlen( pString ), hash );
}

/*
==================
FastHashString
==================
*/
template<typename TType>
FORCEINLINE hash FastHashString( const eastl::basic_string<TType>& string, hash hash /*= 0*/ )
{
	return FastHashString( string.c_str(), string.size(), hash );
}

/*
==================
FastHashString
==================
*/
template<typename TType>
FORCEINLINE hash FastHashString( const eastl::basic_string_view<TType>& string, hash hash /*= 0*/ )
{
	return FastHashString( string.data(), string.size(), hash );
}

/*
==================
FastHashStringInsensitive
==================
*/
template<typename TType>
FORCEINLINE hash FastHashStringInsensitive( const TType* pString, uint64 length, hash hash /*= 0*/ )
{
	for ( uint64 index = 0, size = length * sizeof( TType ); index < size; ++index )
	{
		hash = FastHashItem( S_ToLower( pString[index] ), hash );
	}
	return hash;
}

/*
==================
FastHashStringInsensitive
==================
*/
template<typename TType>
FORCEINLINE hash FastHashStringInsensitive( const TType* pString, hash hash /*= 0*/ )
{
	return FastHashStringInsensitive( pString, S_Strlen( pString ), hash );
}

/*
==================
FastHashStringInsensitive
==================
*/
template<typename TType>
FORCEINLINE hash FastHashStringInsensitive( const eastl::basic_string<TType>& string, hash hash /*= 0*/ )
{
	return FastHashStringInsensitive( string.c_str(), string.size(), hash );
}

/*
==================
FastHashStringInsensitive
==================
*/
template<typename TType>
FORCEINLINE hash FastHashStringInsensitive( const eastl::basic_string_view<TType>& string, hash hash /*= 0*/ )
{
	return FastHashStringInsensitive( string.data(), string.size(), hash );
}

/*
==================
FastHashItem
==================
*/
template<typename TType>
FORCEINLINE hash FastHashItem( const TType& data, hash hash /* = 0 */ )
{
	return data + ( hash << 6 ) + ( hash << 16 ) - hash;
}
