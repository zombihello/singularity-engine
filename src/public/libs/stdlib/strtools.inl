#pragma once

/*
==================
S_Vsnprintf
==================
*/
FORCEINLINE int32 S_Vsnprintf( wchar* pDest, uint32 maxLen, const wchar* pFormat, va_list params )
{
	return vswprintf( pDest, maxLen, pFormat, params );
}

/*
==================
S_Vsnprintf
==================
*/
FORCEINLINE int32 S_Vsnprintf( achar* pDest, uint32 maxLen, const achar* pFormat, va_list params )
{
	return vsnprintf( pDest, maxLen, pFormat, params );
}

/*
==================
S_Vsscanf
==================
*/
FORCEINLINE int32 S_Vsscanf( const wchar* pString, const wchar* pFormat, va_list params )
{
	return vswscanf( pString, pFormat, params );
}

/*
==================
S_Sscanf
==================
*/
FORCEINLINE int32 S_Sscanf( const wchar* pString, const wchar* pFormat, ... )
{
	va_list		params;
	va_start( params, pFormat );
	int32		result = S_Vsscanf( pString, pFormat, params );
	va_end( params );
	return result;
}

/*
==================
S_Vsscanf
==================
*/
FORCEINLINE int32 S_Vsscanf( const achar* pString, const achar* pFormat, va_list params )
{
	return vsscanf( pString, pFormat, params );
}

/*
==================
S_Sscanf
==================
*/
FORCEINLINE int32 S_Sscanf( const achar* pString, const achar* pFormat, ... )
{
	va_list		params;
	va_start( params, pFormat );
	int32		result = S_Vsscanf( pString, pFormat, params );
	va_end( params );
	return result;
}

/*
==================
S_Strlen
==================
*/
FORCEINLINE uint32 S_Strlen( const achar* pString )
{
	return ( uint32 )strlen( pString );
}

/*
==================
S_Strlen
==================
*/
FORCEINLINE uint32 S_Strlen( const wchar* pString )
{
	return ( uint32 )wcslen( pString );
}

/*
==================
S_Strcpy
==================
*/
FORCEINLINE void S_Strcpy( achar* pDest, const achar* pSrc )
{
	strcpy( pDest, pSrc );
}

/*
==================
S_Strcpy
==================
*/
FORCEINLINE void S_Strcpy( wchar* pDest, const wchar* pSrc )
{
	wcscpy( pDest, pSrc );
}

/*
==================
S_Strncpy
==================
*/
FORCEINLINE void S_Strncpy( achar* pDest, const achar* pSrc, uint32 maxLen )
{
	strncpy( pDest, pSrc, maxLen );
}

/*
==================
S_Strncpy
==================
*/
FORCEINLINE void S_Strncpy( wchar* pDest, const wchar* pSrc, uint32 maxLen )
{
	wcsncpy( pDest, pSrc, maxLen );
}

/*
==================
S_Strstr
==================
*/
FORCEINLINE achar* S_Strstr( const achar* pString, const achar* pFind )
{
	return ( achar* )strstr( pString, pFind );
}

/*
==================
S_Strstr
==================
*/
FORCEINLINE wchar* S_Strstr( const wchar* pString, const wchar* pFind )
{
	return ( wchar* )wcsstr( pString, pFind );
}

/*
==================
S_Strcmp
==================
*/
FORCEINLINE uint32 S_Strcmp( const achar* pString1, const achar* pString2 )
{
	return strcmp( pString1, pString2 );
}

/*
==================
S_Strcmp
==================
*/
FORCEINLINE uint32 S_Strcmp( const wchar* pString1, const wchar* pString2 )
{
	return wcscmp( pString1, pString2 );
}

/*
==================
S_Strncmp
==================
*/
FORCEINLINE uint32 S_Strncmp( const achar* pString1, const achar* pString2, uint32 count )
{
	return strncmp( pString1, pString2, count );
}

/*
==================
S_Strncmp
==================
*/
FORCEINLINE uint32 S_Strncmp( const wchar* pString1, const wchar* pString2, uint32 count )
{
	return wcsncmp( pString1, pString2, count );
}

/*
==================
S_Atoi
==================
*/
FORCEINLINE int32 S_Atoi( const achar* pString )
{
	return atoi( pString );
}

/*
==================
S_Atoi
==================
*/
FORCEINLINE int32 S_Atoi( const wchar* pString )
{
	return ( int32 )wcstoul( pString, 0, 10 );
}

/*
==================
S_Atof
==================
*/
FORCEINLINE float S_Atof( const achar* pString )
{
	return ( float )atof( pString );
}

/*
==================
S_Atof
==================
*/
FORCEINLINE float S_Atof( const wchar* pString )
{
	return wcstof( pString, 0 );
}

/*
==================
S_Snprintf
==================
*/
FORCEINLINE int32 S_Snprintf( wchar* pDest, uint32 maxLen, const wchar* pFormat, ... )
{
	va_list		params;
	va_start( params, pFormat );
	int32		result = S_Vsnprintf( pDest, maxLen, pFormat, params );
	va_end( params );
	return result;
}

/*
==================
S_Snprintf
==================
*/
FORCEINLINE int32 S_Snprintf( achar* pDest, uint32 maxLen, const achar* pFormat, ... )
{
	va_list		params;
	va_start( params, pFormat );
	int32		result = S_Vsnprintf( pDest, maxLen, pFormat, params );
	va_end( params );
	return result;
}

/*
==================
S_Sprintf
==================
*/
FORCEINLINE std::string S_Sprintf( const achar* pFormat, ... )
{
	va_list			params;
	va_start( params, pFormat );
	std::string		result = S_Vsprintf( pFormat, params );
	va_end( params );
	return result;
}

/*
==================
S_Sprintf
==================
*/
FORCEINLINE std::wstring S_Sprintf( const wchar* pFormat, ... )
{
	va_list			params;
	va_start( params, pFormat );
	std::wstring	result = S_Vsprintf( pFormat, params );
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
FORCEINLINE achar* S_Strupr( achar* pString )
{
	achar* pStr = pString;
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
FORCEINLINE achar* S_Strlwr( achar* pString )
{
	achar* pStr = pString;
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
FORCEINLINE wchar* S_Strupr( wchar* pString )
{
	wchar* pStr = pString;
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
FORCEINLINE wchar* S_Strlwr( wchar* pString )
{
	wchar* pStr = pString;
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
FORCEINLINE bool S_IsSpace( achar c )
{
	return isspace( c );
}

/*
==================
S_IsSpace
==================
*/
FORCEINLINE bool S_IsSpace( wchar c )
{
	return iswspace( c );
}


/*
==================
CANSIToWCHAR_Convert::Convert
==================
*/
FORCEINLINE wchar* CANSIToWCHAR_Convert::Convert( const achar* pSrcData, wchar* pDstData, uint32 size ) const
{
	// Determine whether we need to allocate memory or not
	uint32		length = ( uint32 )S_Strlen( pSrcData ) + 1;
	if ( length > size )
	{
		// Need to allocate memory because the string is too big
		pDstData = new wchar[length * sizeof( wchar )];
	}

	// Now do the conversion
	for ( uint32 index = 0; index < length; ++index )
	{
		pDstData[index] = ( byte )pSrcData[index];
	}

	pDstData[length] = '\0';
	return pDstData;
}

/*
==================
CANSIToWCHAR_Convert::GetLength
==================
*/
FORCEINLINE uint32 CANSIToWCHAR_Convert::GetLength( wchar* pData ) const
{
	return ( uint32 )S_Strlen( pData );
}


/*
==================
CWCHARToANSI_Convert::Convert
==================
*/
FORCEINLINE achar* CWCHARToANSI_Convert::Convert( const wchar* pSrcData, achar* pDstData, uint32 size ) const
{
	// Determine whether we need to allocate memory or not
	uint32	lengthW = ( uint32 )S_Strlen( pSrcData );

	// Needs to be multiply by sizeof( wchar ) the wide in case each converted char is multibyte
	uint32	lengthA = lengthW * sizeof( wchar );
	if ( lengthA > size )
	{
		// Need to allocate memory because the string is too big
		pDstData = new achar[lengthA * sizeof( achar )];
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
FORCEINLINE uint32 CWCHARToANSI_Convert::GetLength( achar* pData )
{
	return ( uint32 )S_Strlen( pData );
}


/*
==================
TStringConversion::operator TConverTo*
==================
*/
template<typename TConverTo, typename TConvertFrom, typename TBaseConverter, uint32 defaultConversionSize /*= 128*/>
FORCEINLINE TStringConversion<TConverTo, TConvertFrom, TBaseConverter, defaultConversionSize>::operator TConverTo* () const
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
