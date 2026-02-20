#pragma once

/*
==================
S_Stricmp
==================
*/
FORCEINLINE uint32 S_Stricmp( const char* pString1, const char* pString2 )
{
	return stricmp( pString1, pString2 );
}

/*
==================
S_Stricmp
==================
*/
FORCEINLINE uint32 S_Stricmp( const wchar_t* pString1, const wchar_t* pString2 )
{
	return wcsicmp( pString1, pString2 );
}

/*
==================
S_Strnicmp
==================
*/
FORCEINLINE uint32 S_Strnicmp( const char* pString1, const char* pString2, uint32 count )
{
	return strnicmp( pString1, pString2, count );
}

/*
==================
S_Strnicmp
==================
*/
FORCEINLINE uint32 S_Strnicmp( const wchar_t* pString1, const wchar_t* pString2, uint32 count )
{
	return wcsnicmp( pString1, pString2, count );
}

/*
==================
CUTF8ToWCHAR_Convert::Convert
==================
*/
FORCEINLINE wchar_t* CUTF8ToWCHAR_Convert::Convert( const char* pSrcData, wchar_t* pDstData, uint32 size )
{
	// Determine whether we need to allocate memory or not
	uint32 length = MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, pSrcData, -1, NULL, 0 );
	if ( length > size )
	{
		// Need to allocate memory because the string is too big
		pDstData = new wchar_t[length];
	}

	AssertMsg( length != 0, "Unable to convert string from Utf-8 to Utf-16 (GetLastError 0x%X)", GetLastError() );
	length = MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, pSrcData, -1, pDstData, length );
	Assert( length > 0 );

	pDstData[length - 1] = '\0';
	return pDstData;
}

/*
==================
CWCHARToUTF8_Convert::Convert
==================
*/
FORCEINLINE char* CWCHARToUTF8_Convert::Convert( const wchar_t* pSrcData, char* pDstData, uint32 size )
{
	// Determine whether we need to allocate memory or not
	// NOTE: MB_ERR_INVALID_CHARS is invalid flag for such kind of call
	uint32 length = WideCharToMultiByte( CP_UTF8, 0, pSrcData, -1, NULL, 0, NULL, NULL );
	if ( length > size )
	{
		// Need to allocate memory because the string is too big
		pDstData = new char[length];
	}

	AssertMsg( length != 0, "Unable to convert string from Utf-16 to Utf-8 (GetLastError 0x%X)", GetLastError() );
	length = WideCharToMultiByte( CP_UTF8, 0, pSrcData, -1, pDstData, length, NULL, NULL );
	Assert( length > 0 );

	pDstData[length - 1] = '\0';
	return pDstData;
}
