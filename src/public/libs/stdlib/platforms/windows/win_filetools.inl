#ifndef WIN_FILETOOLS_H
#define WIN_FILETOOLS_H

#include "core/debug.h"

/*
==================
S_SetCurrentDirectory
==================
*/
FORCEINLINE bool S_SetCurrentDirectory( const achar* pDirName )
{ 
	return _chdir( pDirName ) == 0;
}

/*
==================
S_GetCurrentDirectory
==================
*/
FORCEINLINE bool S_GetCurrentDirectory( achar* pDestStr, uint32 maxLen )
{
	Assert( maxLen >= 1 );
	Assert( pDestStr );
	if ( !pDestStr || maxLen < 1 )
	{
		return false;
	}

	return _getcwd( pDestStr, maxLen ) == pDestStr;
}

/*
==================
S_IsAbsolutePath
==================
*/
FORCEINLINE bool S_IsAbsolutePath( const achar* pPath )
{
	return ( pPath[0] && pPath[1] == ':' ) || pPath[0] == '/' || pPath[0] == '\\';
}

#endif // !WIN_FILETOOLS_H