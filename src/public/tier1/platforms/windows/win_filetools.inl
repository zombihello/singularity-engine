#pragma once

/*
==================
S_SetCurrentDirectory
==================
*/
FORCEINLINE bool S_SetCurrentDirectory( const char* pDirName )
{
	return _chdir( pDirName ) == 0;
}

/*
==================
S_GetCurrentDirectory
==================
*/
FORCEINLINE bool S_GetCurrentDirectory( char* pDestStr, uint32 maxLen )
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
FORCEINLINE bool S_IsAbsolutePath( const char* pPath )
{
	return ( pPath[0] && pPath[1] == ':' ) || pPath[0] == '/' || pPath[0] == '\\';
}
