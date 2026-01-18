#pragma once

/*
==================
S_Vsnprintf
==================
*/
FORCEINLINE int32 S_Vsnprintf( wchar_t* pDest, uint32 maxLen, const wchar_t* pFormat, va_list params )
{
	return vswprintf( pDest, maxLen, pFormat, params );
}

/*
==================
S_Vsnprintf
==================
*/
FORCEINLINE int32 S_Vsnprintf( char* pDest, uint32 maxLen, const char* pFormat, va_list params )
{
	return vsnprintf( pDest, maxLen, pFormat, params );
}

/*
==================
S_Vsscanf
==================
*/
FORCEINLINE int32 S_Vsscanf( const wchar_t* pString, const wchar_t* pFormat, va_list params )
{
	return vswscanf( pString, pFormat, params );
}

/*
==================
S_Sscanf
==================
*/
FORCEINLINE int32 S_Sscanf( const wchar_t* pString, const wchar_t* pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
	int32 result = S_Vsscanf( pString, pFormat, params );
	va_end( params );
	return result;
}

/*
==================
S_Vsscanf
==================
*/
FORCEINLINE int32 S_Vsscanf( const char* pString, const char* pFormat, va_list params )
{
	return vsscanf( pString, pFormat, params );
}

/*
==================
S_Sscanf
==================
*/
FORCEINLINE int32 S_Sscanf( const char* pString, const char* pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
	int32 result = S_Vsscanf( pString, pFormat, params );
	va_end( params );
	return result;
}

/*
==================
S_Strlen
==================
*/
FORCEINLINE uint32 S_Strlen( const char* pString )
{
	return (uint32)strlen( pString );
}

/*
==================
S_Strlen
==================
*/
FORCEINLINE uint32 S_Strlen( const wchar_t* pString )
{
	return (uint32)wcslen( pString );
}

/*
==================
S_Strcpy
==================
*/
FORCEINLINE void S_Strcpy( char* pDest, const char* pSrc )
{
	strcpy( pDest, pSrc );
}

/*
==================
S_Strcpy
==================
*/
FORCEINLINE void S_Strcpy( wchar_t* pDest, const wchar_t* pSrc )
{
	wcscpy( pDest, pSrc );
}

/*
==================
S_Strncpy
==================
*/
FORCEINLINE void S_Strncpy( char* pDest, const char* pSrc, uint32 maxLen )
{
	strncpy( pDest, pSrc, maxLen );
}

/*
==================
S_Strncpy
==================
*/
FORCEINLINE void S_Strncpy( wchar_t* pDest, const wchar_t* pSrc, uint32 maxLen )
{
	wcsncpy( pDest, pSrc, maxLen );
}

/*
==================
S_Strstr
==================
*/
FORCEINLINE char* S_Strstr( const char* pString, const char* pFind )
{
	return (char*)strstr( pString, pFind );
}

/*
==================
S_Strstr
==================
*/
FORCEINLINE wchar_t* S_Strstr( const wchar_t* pString, const wchar_t* pFind )
{
	return (wchar_t*)wcsstr( pString, pFind );
}

/*
==================
S_Strcmp
==================
*/
FORCEINLINE uint32 S_Strcmp( const char* pString1, const char* pString2 )
{
	return strcmp( pString1, pString2 );
}

/*
==================
S_Strcmp
==================
*/
FORCEINLINE uint32 S_Strcmp( const wchar_t* pString1, const wchar_t* pString2 )
{
	return wcscmp( pString1, pString2 );
}

/*
==================
S_Strncmp
==================
*/
FORCEINLINE uint32 S_Strncmp( const char* pString1, const char* pString2, uint32 count )
{
	return strncmp( pString1, pString2, count );
}

/*
==================
S_Strncmp
==================
*/
FORCEINLINE uint32 S_Strncmp( const wchar_t* pString1, const wchar_t* pString2, uint32 count )
{
	return wcsncmp( pString1, pString2, count );
}

/*
==================
S_Atoi
==================
*/
FORCEINLINE int32 S_Atoi( const char* pString )
{
	return atoi( pString );
}

/*
==================
S_Atoi
==================
*/
FORCEINLINE int32 S_Atoi( const wchar_t* pString )
{
	return (int32)wcstoul( pString, 0, 10 );
}

/*
==================
S_Atof
==================
*/
FORCEINLINE float S_Atof( const char* pString )
{
	return (float)atof( pString );
}

/*
==================
S_Atof
==================
*/
FORCEINLINE float S_Atof( const wchar_t* pString )
{
	return wcstof( pString, 0 );
}

/*
==================
S_Snprintf
==================
*/
FORCEINLINE int32 S_Snprintf( wchar_t* pDest, uint32 maxLen, const wchar_t* pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
	int32 result = S_Vsnprintf( pDest, maxLen, pFormat, params );
	va_end( params );
	return result;
}

/*
==================
S_Snprintf
==================
*/
FORCEINLINE int32 S_Snprintf( char* pDest, uint32 maxLen, const char* pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
	int32 result = S_Vsnprintf( pDest, maxLen, pFormat, params );
	va_end( params );
	return result;
}

/*
==================
S_Sprintf
==================
*/
FORCEINLINE std::string S_Sprintf( const char* pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
	std::string result = S_Vsprintf( pFormat, params );
	va_end( params );
	return result;
}

/*
==================
S_Sprintf
==================
*/
FORCEINLINE std::wstring S_Sprintf( const wchar_t* pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
	std::wstring result = S_Vsprintf( pFormat, params );
	va_end( params );
	return result;
}

/*
==================
S_ToUpper
==================
*/
FORCEINLINE int32 S_ToUpper( int32 ch )
{
	return toupper( ch );
}

/*
==================
S_ToLower
==================
*/
FORCEINLINE int32 S_ToLower( int32 ch )
{
	return tolower( ch );
}

/*
==================
S_Strupr
==================
*/
FORCEINLINE char* S_Strupr( char* pString )
{
	char* pStr = pString;
	while ( *pStr )
	{
		*pStr = S_ToUpper( *pStr );
		++pStr;
	}

	return pString;
}

/*
==================
S_Strlwr
==================
*/
FORCEINLINE char* S_Strlwr( char* pString )
{
	char* pStr = pString;
	while ( *pStr )
	{
		*pStr = S_ToLower( *pStr );
		++pStr;
	}

	return pString;
}

/*
==================
S_Strupr
==================
*/
FORCEINLINE wchar_t* S_Strupr( wchar_t* pString )
{
	wchar_t* pStr = pString;
	while ( *pStr )
	{
		*pStr = S_ToUpper( *pStr );
		++pStr;
	}

	return pString;
}

/*
==================
S_Strlwr
==================
*/
FORCEINLINE wchar_t* S_Strlwr( wchar_t* pString )
{
	wchar_t* pStr = pString;
	while ( *pStr )
	{
		*pStr = S_ToLower( *pStr );
		++pStr;
	}

	return pString;
}

/*
==================
S_IsSpace
==================
*/
FORCEINLINE bool S_IsSpace( char c )
{
	return isspace( c );
}

/*
==================
S_IsSpace
==================
*/
FORCEINLINE bool S_IsSpace( wchar_t c )
{
	return iswspace( c );
}

/*
==================
CANSIToWCHAR_Convert::Convert
==================
*/
FORCEINLINE wchar_t* CANSIToWCHAR_Convert::Convert( const char* pSrcData, wchar_t* pDstData, uint32 size ) const
{
	// Determine whether we need to allocate memory or not
	uint32 length = (uint32)S_Strlen( pSrcData ) + 1;
	if ( length > size )
	{
		// Need to allocate memory because the string is too big
		pDstData = new wchar_t[length * sizeof( wchar_t )];
	}

	// Now do the conversion
	for ( uint32 index = 0; index < length; ++index )
	{
		pDstData[index] = (byte)pSrcData[index];
	}

	pDstData[length] = '\0';
	return pDstData;
}

/*
==================
CANSIToWCHAR_Convert::GetLength
==================
*/
FORCEINLINE uint32 CANSIToWCHAR_Convert::GetLength( wchar_t* pData ) const
{
	return (uint32)S_Strlen( pData );
}

/*
==================
CWCHARToANSI_Convert::Convert
==================
*/
FORCEINLINE char* CWCHARToANSI_Convert::Convert( const wchar_t* pSrcData, char* pDstData, uint32 size ) const
{
	// Determine whether we need to allocate memory or not
	uint32 lengthW = (uint32)S_Strlen( pSrcData );

	// Needs to be multiply by sizeof( wchar_t ) the wide in case each converted char is multibyte
	uint32 lengthA = lengthW * sizeof( wchar_t );
	if ( lengthA > size )
	{
		// Need to allocate memory because the string is too big
		pDstData = new char[lengthA * sizeof( char )];
	}

	// Now do the conversion
	for ( uint32 index = 0; index < lengthW; ++index )
	{
		pDstData[index] = pSrcData[index] & 0xFF;
	}

	pDstData[lengthW] = '\0';
	return pDstData;
}

/*
==================
CWCHARToANSI_Convert::GetLength
==================
*/
FORCEINLINE uint32 CWCHARToANSI_Convert::GetLength( char* pData )
{
	return (uint32)S_Strlen( pData );
}

/*
==================
TStringConversion::operator TConverTo*
==================
*/
template<typename TConverTo, typename TConvertFrom, typename TBaseConverter, uint32 defaultConversionSize /*= 128*/>
FORCEINLINE TStringConversion<TConverTo, TConvertFrom, TBaseConverter, defaultConversionSize>::operator TConverTo*() const
{
	return pConvertedString;
}

/*
==================
TStringConversion::GetLength
==================
*/
template<typename TConverTo, typename TConvertFrom, typename TBaseConverter, uint32 defaultConversionSize /*= 128*/>
FORCEINLINE uint32 TStringConversion<TConverTo, TConvertFrom, TBaseConverter, defaultConversionSize>::GetLength() const
{
	return pConvertedString ? TBaseConverter::GetLength( pConvertedString ) : 0;
}
