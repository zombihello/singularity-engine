#include "pch_stdlib.h"
#include "stdlib/strtools.h"

/*
==================
S_Vsprintf
==================
*/
std::string S_Vsprintf( const achar* pFormat, va_list params )
{
	int32	bufferSize = 1024;
	achar*	pBuffer = nullptr;
	int		result = -1;

	while ( result == -1 )
	{
		free( pBuffer );
		pBuffer = ( achar* )malloc( bufferSize * sizeof( achar ) );

		// Get formated string with args
		result = S_Vsnprintf( pBuffer, bufferSize, pFormat, params );
		if ( result >= bufferSize )
		{
			result = -1;
		}

		bufferSize *= 2;
	}
	pBuffer[result] = 0;

	std::string		formatedString = pBuffer;
	free( pBuffer );
	return formatedString;
}

/*
==================
S_Vsprintf
==================
*/
std::wstring S_Vsprintf( const wchar* pFormat, va_list params )
{
	int32	bufferSize = 1024;
	wchar*	pBuffer = nullptr;
	int		result = -1;

	while ( result == -1 )
	{
		free( pBuffer );
		pBuffer = ( wchar* )malloc( bufferSize * sizeof( wchar ) );

		// Get formated string with args
		result = S_Vsnprintf( pBuffer, bufferSize, pFormat, params );
		if ( result >= bufferSize )
		{
			result = -1;
		}

		bufferSize *= 2;
	}
	pBuffer[result] = 0;

	std::wstring		formatedString = pBuffer;
	free( pBuffer );
	return formatedString;
}

/*
==================
S_Strncat
==================
*/
achar* S_Strncat( achar* pDest, const achar* pSrc, uint32 destBufferSize, int32 maxCharsToCopy /*= COPY_ALL_CHARACTERS*/ )
{
	Assert( pDest );
	Assert( pSrc );

	// Get string length of pDest and pSrc
	uint32		charsToCopy = 0;
	uint32		destLen = S_Strlen( pDest );
	uint32		srcLen = S_Strlen( pSrc );

	// Determining whether to copy the entire line or part
	if ( maxCharsToCopy <= COPY_ALL_CHARACTERS )
	{
		charsToCopy = srcLen;
	}
	else
	{
		charsToCopy = Min( ( uint32 )maxCharsToCopy, srcLen );
	}

	// Make sure what we won't out of bounds
	if ( destLen + charsToCopy >= destBufferSize )
	{
		charsToCopy = destBufferSize - destLen - 1;
	}

	// If nothing to copy exit
	if ( ( int32 )charsToCopy <= 0 )
	{
		return pDest;
	}

	// Copy pSrc to pDest
	return strncat( pDest, pSrc, charsToCopy );
}

/*
==================
S_Strncat
==================
*/
wchar* S_Strncat( wchar* pDest, const wchar* pSrc, uint32 destBufferSize, int32 maxCharsToCopy /*= COPY_ALL_CHARACTERS*/ )
{
	Assert( pDest );
	Assert( pSrc );

	// Get string length of pDest and pSrc
	uint32		charsToCopy = 0;
	uint32		destLen = S_Strlen( pDest );
	uint32		srcLen = S_Strlen( pSrc );

	// Determining whether to copy the entire line or part
	if ( maxCharsToCopy <= COPY_ALL_CHARACTERS )
	{
		charsToCopy = srcLen;
	}
	else
	{
		charsToCopy = Min( ( uint32 )maxCharsToCopy, srcLen );
	}

	// Make sure what we won't out of bounds
	if ( destLen + charsToCopy >= destBufferSize )
	{
		charsToCopy = destBufferSize - destLen - 1;
	}

	// If nothing to copy exit
	if ( ( int32 )charsToCopy <= 0 )
	{
		return pDest;
	}

	// Copy pSrc to pDest
	return wcsncat( pDest, pSrc, charsToCopy );
}