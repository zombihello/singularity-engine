#include "pch_tier1.h"
#include "tier1/strtools.h"

/*
==================
S_Vsprintf
==================
*/
eastl::string S_Vsprintf( const char* pFormat, va_list params )
{
	int32 bufferSize = 1024;
	char* pBuffer	 = nullptr;
	int	  result	 = -1;

	while ( result == -1 )
	{
		free( pBuffer );
		pBuffer = (char*)malloc( bufferSize * sizeof( char ) );

		// Get formated string with args
		result = S_Vsnprintf( pBuffer, bufferSize, pFormat, params );
		if ( result >= bufferSize )
		{
			result = -1;
		}

		bufferSize *= 2;
	}
	pBuffer[result] = 0;

	eastl::string formatedString = pBuffer;
	free( pBuffer );
	return formatedString;
}

/*
==================
S_Vsprintf
==================
*/
eastl::wstring S_Vsprintf( const wchar_t* pFormat, va_list params )
{
	int32	 bufferSize = 1024;
	wchar_t* pBuffer	= nullptr;
	int		 result		= -1;

	while ( result == -1 )
	{
		free( pBuffer );
		pBuffer = (wchar_t*)malloc( bufferSize * sizeof( wchar_t ) );

		// Get formated string with args
		result = S_Vsnprintf( pBuffer, bufferSize, pFormat, params );
		if ( result >= bufferSize )
		{
			result = -1;
		}

		bufferSize *= 2;
	}
	pBuffer[result] = 0;

	eastl::wstring formatedString = pBuffer;
	free( pBuffer );
	return formatedString;
}

/*
==================
S_Strncat
==================
*/
char* S_Strncat( char* pDest, const char* pSrc, uint32 destBufferSize, int32 maxCharsToCopy /*= COPY_ALL_CHARACTERS*/ )
{
	Assert( pDest );
	Assert( pSrc );

	// Get string length of pDest and pSrc
	uint32 charsToCopy = 0;
	uint32 destLen	   = S_Strlen( pDest );
	uint32 srcLen	   = S_Strlen( pSrc );

	// Determining whether to copy the entire line or part
	if ( maxCharsToCopy <= COPY_ALL_CHARACTERS )
	{
		charsToCopy = srcLen;
	}
	else
	{
		charsToCopy = S_Min( (uint32)maxCharsToCopy, srcLen );
	}

	// Make sure what we won't out of bounds
	if ( destLen + charsToCopy >= destBufferSize )
	{
		charsToCopy = destBufferSize - destLen - 1;
	}

	// If nothing to copy exit
	if ( (int32)charsToCopy <= 0 )
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
wchar_t* S_Strncat( wchar_t* pDest, const wchar_t* pSrc, uint32 destBufferSize, int32 maxCharsToCopy /*= COPY_ALL_CHARACTERS*/ )
{
	Assert( pDest );
	Assert( pSrc );

	// Get string length of pDest and pSrc
	uint32 charsToCopy = 0;
	uint32 destLen	   = S_Strlen( pDest );
	uint32 srcLen	   = S_Strlen( pSrc );

	// Determining whether to copy the entire line or part
	if ( maxCharsToCopy <= COPY_ALL_CHARACTERS )
	{
		charsToCopy = srcLen;
	}
	else
	{
		charsToCopy = S_Min( (uint32)maxCharsToCopy, srcLen );
	}

	// Make sure what we won't out of bounds
	if ( destLen + charsToCopy >= destBufferSize )
	{
		charsToCopy = destBufferSize - destLen - 1;
	}

	// If nothing to copy exit
	if ( (int32)charsToCopy <= 0 )
	{
		return pDest;
	}

	// Copy pSrc to pDest
	return wcsncat( pDest, pSrc, charsToCopy );
}

/*
==================
S_UnescapeString
==================
*/
void S_UnescapeString( eastl::string& dest, const char* pSrc, uint32 srcLen )
{
	dest.reserve( srcLen );
	const char* pSrcData = pSrc;
	const char* pEndData = pSrcData + srcLen;
	while ( pSrcData != pEndData )
	{
		// Convert unescaped symbols to escaped symbols (i.g: "\n" -> '\n')
		if ( *pSrcData == '\\' )
		{
			++pSrcData;
			if ( pSrcData == pEndData )
			{
				dest += '\\';
				break;
			}

			switch ( *pSrcData )
			{
			case 'n': dest += '\n'; break;
			case 'r': dest += '\r'; break;
			case 't': dest += '\t'; break;
			default: dest += *pSrcData; break;
			}

			++pSrcData;
			continue;
		}

		// Regular symbol
		dest += *pSrcData++;
	}
}

/*
==================
S_UnescapeString
==================
*/
void S_UnescapeString( eastl::wstring& dest, const wchar_t* pSrc, uint32 srcLen )
{
	dest.reserve( srcLen );
	const wchar_t* pSrcData = pSrc;
	const wchar_t* pEndData = pSrcData + srcLen;
	while ( pSrcData != pEndData )
	{
		// Convert unescaped symbols to escaped symbols (i.g: "\n" -> '\n')
		if ( *pSrcData == L'\\' )
		{
			++pSrcData;
			if ( pSrcData == pEndData )
			{
				dest += L'\\';
				break;
			}

			switch ( *pSrcData )
			{
			case L'n': dest += L'\n'; break;
			case L'r': dest += L'\r'; break;
			case L't': dest += L'\t'; break;
			default: dest += *pSrcData; break;
			}

			++pSrcData;
			continue;
		}

		// Regular symbol
		dest += *pSrcData++;
	}
}

/*
==================
S_EscapeString
==================
*/
void S_EscapeString( eastl::string& dest, const char* pSrc, uint32 srcLen )
{
	dest.reserve( srcLen );
	for ( uint32 index = 0; index < srcLen; ++index )
	{
		const char& c = pSrc[index];
		switch ( c )
		{
		case '\n': dest += "\\n"; break;
		case '\r': dest += "\\r"; break;
		case '\t': dest += "\\t"; break;
		default: dest += c; break;
		}
	}
}

/*
==================
S_EscapeString
==================
*/
void S_EscapeString( eastl::wstring& dest, const wchar_t* pSrc, uint32 srcLen )
{
	dest.reserve( srcLen );
	for ( uint32 index = 0; index < srcLen; ++index )
	{
		const wchar_t& c = pSrc[index];
		switch ( c )
		{
		case L'\n': dest += L"\\n"; break;
		case L'\r': dest += L"\\r"; break;
		case L'\t': dest += L"\\t"; break;
		default: dest += c; break;
		}
	}
}
