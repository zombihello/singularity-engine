#pragma once

/*
==================
FastHash
==================
*/
FORCEINLINE hash_t FastHash( const void* pData, uint64 size, hash_t hash /*= 0*/ )
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
FORCEINLINE hash_t FastHash( const TType& data, hash_t hash )
{
	return FastHash( &data, sizeof( data ), hash );
}

/*
==================
FastHashString
==================
*/
template<typename TType>
FORCEINLINE hash_t FastHashString( const TType* pString, uint64 length, hash_t hash /*= 0*/ )
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
FORCEINLINE hash_t FastHashString( const TType* pString, hash_t hash /*= 0*/ )
{
	return FastHashString( pString, S_Strlen( pString ), hash );
}

/*
==================
FastHashString
==================
*/
template<typename TType>
FORCEINLINE hash_t FastHashString( const eastl::basic_string<TType>& string, hash_t hash /*= 0*/ )
{
	return FastHashString( string.c_str(), string.size(), hash );
}

/*
==================
FastHashString
==================
*/
template<typename TType>
FORCEINLINE hash_t FastHashString( const eastl::basic_string_view<TType>& string, hash_t hash /*= 0*/ )
{
	return FastHashString( string.data(), string.size(), hash );
}

/*
==================
FastHashStringInsensitive
==================
*/
template<typename TType>
FORCEINLINE hash_t FastHashStringInsensitive( const TType* pString, uint64 length, hash_t hash /*= 0*/ )
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
FORCEINLINE hash_t FastHashStringInsensitive( const TType* pString, hash_t hash /*= 0*/ )
{
	return FastHashStringInsensitive( pString, S_Strlen( pString ), hash );
}

/*
==================
FastHashStringInsensitive
==================
*/
template<typename TType>
FORCEINLINE hash_t FastHashStringInsensitive( const eastl::basic_string<TType>& string, hash_t hash /*= 0*/ )
{
	return FastHashStringInsensitive( string.c_str(), string.size(), hash );
}

/*
==================
FastHashStringInsensitive
==================
*/
template<typename TType>
FORCEINLINE hash_t FastHashStringInsensitive( const eastl::basic_string_view<TType>& string, hash_t hash /*= 0*/ )
{
	return FastHashStringInsensitive( string.data(), string.size(), hash );
}

/*
==================
FastHashItem
==================
*/
template<typename TType>
FORCEINLINE hash_t FastHashItem( const TType& data, hash_t hash /* = 0 */ )
{
	return data + ( hash << 6 ) + ( hash << 16 ) - hash;
}
