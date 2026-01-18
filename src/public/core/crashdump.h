#pragma once
#include "core/core.h"

//-----------------------------------------------------------------------------
// Crash dump system
//-----------------------------------------------------------------------------
struct crashDumpAppInfo_t
{
	const char* pAppName;		// If NULL or empty string will be used default application name
	const char* pAppVersion;	// If NULL or empty string will be used default application version
	const char* pSupportEmail;	// If NULL or empty string will be used default email
	const char* pSupportURL;	// If NULL or empty string will be used default URL
};

CORE_INTERFACE void CrashDump_SetAppInfo( const crashDumpAppInfo_t& appInfo );
CORE_INTERFACE void CrashDump_AddLogFile( const char* pPath );
