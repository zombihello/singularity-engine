#pragma once

/*
==================
Parserlib_GetNumUtf8Bytes
==================
*/
FORCEINLINE uint32 Parserlib_GetNumUtf8Bytes( achar value )
{
    unsigned long    index = 0;
    unsigned long    mask = ( ~value );

#if PLATFORM_WINDOWS
    _BitScanReverse( &index, mask );
#else
    // We're counting the bits 
    index = 31 - __lzcnt32( mask );
#endif // PLATFORM_WINDOWS

    // We have the 0-index context of the most significant bit, which we need to turn into a count of the number of bits (or 1-index context)
    // Then reverse the order so that we have a count of the number of bits leading up to the MSB
    return 8 - ( index + 1 );
}

/*
==================
Parserlib_GetNumChars
==================
*/
FORCEINLINE uint32 Parserlib_GetNumChars( const achar* pString, uint32 size )
{
	uint32      length = 0;
	for ( uint32 idx = 0; idx < size; ++idx )
	{
		if ( ( pString[idx] & 0x80 ) )
		{
			idx += Parserlib_GetNumUtf8Bytes( pString[idx] ) - 1;
		}
		++length;
	}

	return length;
}