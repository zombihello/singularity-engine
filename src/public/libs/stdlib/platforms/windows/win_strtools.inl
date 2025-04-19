#ifndef WIN_STRTOOLS_H
#define WIN_STRTOOLS_H

/*
==================
S_Stricmp
==================
*/
FORCEINLINE uint32 S_Stricmp( const achar* pString1, const achar* pString2 ) 
{ 
	return stricmp( pString1, pString2 ); 
}

/*
==================
S_Stricmp
==================
*/
FORCEINLINE uint32 S_Stricmp( const wchar* pString1, const wchar* pString2 )
{ 
	return wcsicmp( pString1, pString2 ); 
}

/*
==================
S_Strnicmp
==================
*/
FORCEINLINE uint32 S_Strnicmp( const achar* pString1, const achar* pString2, uint32 count ) 
{ 
	return strnicmp( pString1, pString2, count ); 
}

/*
==================
S_Strnicmp
==================
*/
FORCEINLINE uint32 S_Strnicmp( const wchar* pString1, const wchar* pString2, uint32 count )
{
	return wcsnicmp( pString1, pString2, count );
}

#endif // !WIN_STRTOOLS_H