#pragma once

/*
==================
Sys_IsDebuggerAttached
==================
*/
FORCEINLINE bool Sys_IsDebuggerAttached()
{
#if !RETAIL
	return IsDebuggerPresent();
#else
	return false;
#endif	// !RETAIL
}

/*
==================
Sys_DebugMessage
==================
*/
FORCEINLINE void Sys_DebugMessage( const char* pMsg )
{
#if !RETAIL
	OutputDebugStringW( UTF8_TO_WCHAR( pMsg ) );
#endif	// !RETAIL
}
