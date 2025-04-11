/**
 * @file
 * @addtogroup stdlib stdlib
 */

#ifndef FASTHASH_H
#define FASTHASH_H

#include "core/types.h"
#include "core/platform.h"
#include "stdlib/strtools.h"

/**
 * @ingroup stdlib
 * @brief Fast hashing function that doesn't require a table lookup for each element
 *
 * @param pData		Pointer to data for which is considered hash
 * @param size		Data size
 * @param hash		Start hash
 * @return Return calculated hash
 */
FORCEINLINE hash_t FastHash( const void* pData, uint64 size, hash_t hash = 0 )
{
	byte*	pLocalData = ( byte* )pData;
	for ( uint64 index = 0; index < size; ++index )
	{
		hash = pLocalData[index] + ( hash << 6 ) + ( hash << 16 ) - hash;
	}

	return hash;
}

/**
 * @ingroup stdlib
 * @brief Fast hashing function from a string that doesn't require a table lookup for each element
 * @note The function calculate a hash with insensitive to case
 * 
 * @param pString	The string for which is considered hash
 * @param size		Data size
 * @param hash		Start hash
 * @return Return calculated hash
 */
FORCEINLINE hash_t FastHashFromString( const void* pData, uint64 size, hash_t hash = 0 )
{
	byte*		pLocalData = ( byte* )pData;
	for ( uint64 index = 0; index < size; ++index )
	{
		hash = S_ToLower( pLocalData[index] ) + ( hash << 6 ) + ( hash << 16 ) - hash;
	}

	return hash;
}

/**
 * @ingroup stdlib
 * @brief Fast hashing function that doesn't require a table lookup for each element
 *
 * @param value		The value for which is considered hash
 * @param hash		Start hash
 * @return Return calculated hash
 */
template<typename TType>
FORCEINLINE hash_t FastHash( const TType& value, hash_t hash = 0 )
{
	return FastHash( &value, sizeof( value ), hash );
}

/**
 * @ingroup stdlib
 * @brief Fast hashing function from string that doesn't require a table lookup for each element
 *
 * @param pString	The string for which is considered hash
 * @param hash		Start hash
 * @return Return calculated hash
 */
FORCEINLINE hash_t FastHashFromString( const wchar* pString, hash_t hash = 0 )
{
	return FastHashFromString( pString, S_Strlen( pString ) * sizeof( wchar ), hash );
}

/**
 * @ingroup stdlib
 * @brief Fast hashing function from string that doesn't require a table lookup for each element
 *
 * @param string	The string for which is considered hash
 * @param hash		Start hash
 * @return Return calculated hash
 */
FORCEINLINE hash_t FastHashFromString( const std::wstring& string, hash_t hash = 0 )
{
	return FastHashFromString( string.data(), ( uint64 )string.size() * sizeof( wchar ), hash );
}

/**
 * @ingroup stdlib
 * @brief Fast hashing function from string that doesn't require a table lookup for each element
 *
 * @param pString	The string for which is considered hash
 * @param hash		Start hash
 * @return Return calculated hash
 */
FORCEINLINE hash_t FastHashFromString( const achar* pString, hash_t hash = 0 )
{
	return FastHashFromString( pString, S_Strlen( pString ) * sizeof( achar ), hash );
}

/**
 * @ingroup stdlib
 * @brief Fast hashing function from string that doesn't require a table lookup for each element
 *
 * @param string	The string for which is considered hash
 * @param hash		Start hash
 * @return Return calculated hash
 */
FORCEINLINE hash_t FastHashFromString( const std::string& string, hash_t hash = 0 )
{
	return FastHashFromString( string.data(), ( uint64 )string.size() * sizeof( achar ), hash );
}

#endif // !FASTHASH_H