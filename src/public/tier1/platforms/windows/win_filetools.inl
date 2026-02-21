#pragma once

/*
==================
S_IsAbsolutePath
==================
*/
FORCEINLINE bool S_IsAbsolutePath( const char* pPath )
{
	return ( pPath[0] && pPath[1] == ':' ) || pPath[0] == '/' || pPath[0] == '\\';
}
