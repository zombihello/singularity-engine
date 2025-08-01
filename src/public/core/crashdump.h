#pragma once
#include "core/core.h"

//-----------------------------------------------------------------------------
// Crash dump system
//-----------------------------------------------------------------------------
struct crashDumpAppInfo_t
{
	const achar*	pAppName;		// If NULL or empty string will be used default application name
	const achar*	pAppVersion;	// If NULL or empty string will be used default application version
	const achar*	pSupportEmail;	// If NULL or empty string will be used default email
	const achar*	pSupportURL;	// If NULL or empty string will be used default URL
};


CORE_INTERFACE void CrashDump_SetAppInfo( const crashDumpAppInfo_t& appInfo );
CORE_INTERFACE void CrashDump_AddLogFile( const achar* pPath );