#pragma once

/*
==================
CApplication::CApplication
==================
*/
FORCEINLINE CApplication::CApplication( CAppSystemGroup* pChildAppSystemGroup, const char* pLogFileName /* = "" */ )
	: pChildAppSystemGroup( pChildAppSystemGroup )
#if ENABLE_LOGGING
	, pLogFileName( pLogFileName )
	, pLogOutputFile( NULL )
#endif	// ENABLE_LOGGING
{
}
