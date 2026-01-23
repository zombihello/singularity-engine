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