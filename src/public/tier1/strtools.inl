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
S_Strchr
==================
*/
FORCEINLINE const char* S_Strchr( const char* pString, char c )
{
	return strchr( pString, c );
}

/*
==================
S_Strchr
==================
*/
FORCEINLINE const wchar_t* S_Strchr( const wchar_t* pString, wchar_t c )
{
	return wcschr( pString, c );
}

/*
==================
S_Strstr
==================
*/
FORCEINLINE const char* S_Strstr( const char* pString, const char* pFind )
{
	return strstr( pString, pFind );
}

/*
==================
S_Strstr
==================
*/
FORCEINLINE const wchar_t* S_Strstr( const wchar_t* pString, const wchar_t* pFind )
{
	return wcsstr( pString, pFind );
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
FORCEINLINE int32 S_Atoi( const char* pString, char** pEndPtr /*= NULL*/ )
{
	return (int32)strtol( pString, pEndPtr, 10 );
}

/*
==================
S_Atoi
==================
*/
FORCEINLINE int32 S_Atoi( const wchar_t* pString, wchar_t** pEndPtr /*= NULL */ )
{
	return (int32)wcstol( pString, pEndPtr, 10 );
}

/*
==================
S_Atoi64
==================
*/
FORCEINLINE int64 S_Atoi64( const char* pString, char** pEndPtr /*= NULL*/ )
{
	return (int64)strtoll( pString, pEndPtr, 10 );
}

/*
==================
S_Atoi64
==================
*/
FORCEINLINE int64 S_Atoi64( const wchar_t* pString, wchar_t** pEndPtr /*= NULL */ )
{
	return (int64)wcstoll( pString, pEndPtr, 10 );
}

/*
==================
S_Atof
==================
*/
FORCEINLINE float S_Atof( const char* pString, char** pEndPtr /*= NULL*/ )
{
	return strtof( pString, pEndPtr );
}

/*
==================
S_Atof
==================
*/
FORCEINLINE float S_Atof( const wchar_t* pString, wchar_t** pEndPtr /*= NULL */ )
{
	return wcstof( pString, pEndPtr );
}

/*
==================
S_Atod
==================
*/
FORCEINLINE double S_Atod( const char* pString, char** pEndPtr /*= NULL*/ )
{
	return strtod( pString, pEndPtr );
}

/*
==================
S_Atod
==================
*/
FORCEINLINE double S_Atod( const wchar_t* pString, wchar_t** pEndPtr /*= NULL */ )
{
	return wcstod( pString, pEndPtr );
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
FORCEINLINE eastl::string S_Sprintf( const char* pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
	eastl::string result = S_Vsprintf( pFormat, params );
	va_end( params );
	return result;
}

/*
==================
S_Sprintf
==================
*/
FORCEINLINE eastl::wstring S_Sprintf( const wchar_t* pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
	eastl::wstring result = S_Vsprintf( pFormat, params );
	va_end( params );
	return result;
}

/*
==================
S_ToUpper
==================
*/
FORCEINLINE char S_ToUpper( char c )
{
	return (char)toupper( c );
}

/*
==================
S_ToLower
==================
*/
FORCEINLINE char S_ToLower( char c )
{
	return (char)tolower( c );
}

/*
==================
S_ToUpper
==================
*/
FORCEINLINE wchar_t S_ToUpper( wchar_t c )
{
	return (wchar_t)towupper( c );
}

/*
==================
S_ToLower
==================
*/
FORCEINLINE wchar_t S_ToLower( wchar_t c )
{
	return (wchar_t)towlower( c );
}

/*
==================
S_Strupr
==================
*/
FORCEINLINE void S_Strupr( char* pString )
{
	char* pStr = pString;
	while ( *pStr )
	{
		*pStr = S_ToUpper( *pStr );
		++pStr;
	}
}

/*
==================
S_Strupr
==================
*/
FORCEINLINE void S_Strupr( eastl::string& string )
{
	S_Strupr( string.data() );
}

/*
==================
S_Strlwr
==================
*/
FORCEINLINE void S_Strlwr( char* pString )
{
	char* pStr = pString;
	while ( *pStr )
	{
		*pStr = S_ToLower( *pStr );
		++pStr;
	}
}

/*
==================
S_Strlwr
==================
*/
FORCEINLINE void S_Strlwr( eastl::string& string )
{
	S_Strlwr( string.data() );
}

/*
==================
S_Strupr
==================
*/
FORCEINLINE void S_Strupr( wchar_t* pString )
{
	wchar_t* pStr = pString;
	while ( *pStr )
	{
		*pStr = S_ToUpper( *pStr );
		++pStr;
	}
}

/*
==================
S_Strupr
==================
*/
FORCEINLINE void S_Strupr( eastl::wstring& string )
{
	S_Strupr( string.data() );
}

/*
==================
S_Strlwr
==================
*/
FORCEINLINE void S_Strlwr( wchar_t* pString )
{
	wchar_t* pStr = pString;
	while ( *pStr )
	{
		*pStr = S_ToLower( *pStr );
		++pStr;
	}
}

/*
==================
S_Strlwr
==================
*/
FORCEINLINE void S_Strlwr( eastl::wstring& string )
{
	S_Strlwr( string.data() );
}

/*
==================
S_IsSpace
==================
*/
FORCEINLINE bool S_IsSpace( char c )
{
	return isspace( (uint8)c );
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
S_IsDigit
==================
*/
FORCEINLINE bool S_IsDigit( char c )
{
	return isdigit( (uint8)c );
}

/*
==================
S_IsDigit
==================
*/
FORCEINLINE bool S_IsDigit( wchar_t c )
{
	return iswdigit( c );
}

/*
==================
S_Utf8_CharSize
==================
*/
FORCEINLINE uint32 S_Utf8_CharSize( byte b )
{
	// 0xxxxxxx
	if ( ( b & 0x80 ) == 0x00 )
	{
		return 1;
	}

	// 110xxxxx
	if ( ( b & 0xE0 ) == 0xC0 )
	{
		return 2;
	}

	// 1110xxxx
	if ( ( b & 0xF0 ) == 0xE0 )
	{
		return 3;
	}

	// 11110xxx
	if ( ( b & 0xF8 ) == 0xF0 )
	{
		return 4;
	}

	// Continuation (10xxxxxx) or prohibited 11111xxx
	return 0;
}

/*
==================
S_Utf8_Strlen
==================
*/
FORCEINLINE uint32 S_Utf8_Strlen( const char* pString, uint32 size )
{
	uint32 length = 0;
	for ( uint32 index = 0; index < size; ++length )
	{
		uint32 charSize = S_Utf8_CharSize( pString[index] );
		Assert( charSize > 0 );
		index += charSize;
	}

	return length;
}

/*
==================
S_ConvertEscapeToUnescapeSymbols
==================
*/
FORCEINLINE void S_ConvertEscapeToUnescapeSymbols( eastl::string& dest, const char* pSrc )
{
	S_ConvertEscapeToUnescapeSymbols( dest, pSrc, S_Strlen( pSrc ) );
}

/*
==================
S_ConvertEscapeToUnescapeSymbols
==================
*/
FORCEINLINE void S_ConvertEscapeToUnescapeSymbols( eastl::wstring& dest, const wchar_t* pSrc )
{
	S_ConvertEscapeToUnescapeSymbols( dest, pSrc, S_Strlen( pSrc ) );
}

/*
==================
S_ConvertUnescapeToEscapeSymbols
==================
*/
FORCEINLINE void S_ConvertUnescapeToEscapeSymbols( eastl::string& dest, const char* pSrc )
{
	S_ConvertUnescapeToEscapeSymbols( dest, pSrc, S_Strlen( pSrc ) );
}

/*
==================
S_ConvertUnescapeToEscapeSymbols
==================
*/
FORCEINLINE void S_ConvertUnescapeToEscapeSymbols( eastl::wstring& dest, const wchar_t* pSrc )
{
	S_ConvertUnescapeToEscapeSymbols( dest, pSrc, S_Strlen( pSrc ) );
}

/*
==================
CANSIToWCHAR_Convert::Convert
==================
*/
FORCEINLINE wchar_t* CANSIToWCHAR_Convert::Convert( const char* pSrcData, wchar_t* pDstData, uint32 size )
{
	// Determine whether we need to allocate memory or not
	uint32 length = S_Strlen( pSrcData ) + 1;
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
CWCHARToANSI_Convert::Convert
==================
*/
FORCEINLINE char* CWCHARToANSI_Convert::Convert( const wchar_t* pSrcData, char* pDstData, uint32 size )
{
	// Determine whether we need to allocate memory or not
	uint32 lengthW = S_Strlen( pSrcData );

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
CBoolToString::Convert
==================
*/
template<typename TCharType>
FORCEINLINE const TCharType* CBoolToString::Convert( bool bValue )
{
	static_assert( false, "Unknown TCharType" );
	return nullptr;
}

/*
==================
CBoolToString::Convert
==================
*/
template<>
FORCEINLINE const char* CBoolToString::Convert<char>( bool bValue )
{
	return bValue ? "true" : "false";
}

/*
==================
CBoolToString::Convert
==================
*/
template<>
FORCEINLINE const wchar_t* CBoolToString::Convert<wchar_t>( bool bValue )
{
	return bValue ? L"true" : L"false";
}

/*
==================
CStringToBool::Convert
==================
*/
template<typename TCharType>
FORCEINLINE bool CStringToBool::Convert( const TCharType* pValue )
{
	static_assert( false, "Unknown TCharType" );
	return false;
}

/*
==================
CStringToBool::Convert
==================
*/
template<>
FORCEINLINE bool CStringToBool::Convert<char>( const char* pValue )
{
	if ( !S_Stricmp( pValue, "true" ) || !S_Stricmp( pValue, "yes" ) || !S_Stricmp( pValue, "on" ) )
	{
		return true;
	}
	else if ( !S_Stricmp( pValue, "false" ) || !S_Stricmp( pValue, "no" ) || !S_Stricmp( pValue, "off" ) )
	{
		return false;
	}
	else
	{
		return S_Atoi( pValue ) ? true : false;
	}
}

/*
==================
CStringToBool::Convert
==================
*/
template<>
FORCEINLINE bool CStringToBool::Convert<wchar_t>( const wchar_t* pValue )
{
	if ( !S_Stricmp( pValue, L"true" ) || !S_Stricmp( pValue, L"yes" ) || !S_Stricmp( pValue, L"on" ) )
	{
		return true;
	}
	else if ( !S_Stricmp( pValue, L"false" ) || !S_Stricmp( pValue, L"no" ) || !S_Stricmp( pValue, L"off" ) )
	{
		return false;
	}
	else
	{
		return S_Atoi( pValue ) ? true : false;
	}
}
