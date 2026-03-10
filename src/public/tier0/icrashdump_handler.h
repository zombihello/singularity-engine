#pragma once
#include "tier0/tier0.h"

//-----------------------------------------------------------------------------
// Crash dump system
//-----------------------------------------------------------------------------
struct crashDumpAppInfo_t
{
	const char* pAppName;
	const char* pAppVersion;
	const char* pSupportEmail;
	const char* pSupportURL;
};

class ICrashDumpHandler
{
public:
	// Initialize and shutdown the crash dump handler
	virtual void Init()		= 0;
	virtual void Shutdown() = 0;

	// Functions to attach some information to a crash dump
	virtual void SetAppInfo( const crashDumpAppInfo_t& appInfo ) = 0;
	virtual void SetMessage( const char* pMessage )				 = 0;
	virtual void AddLogFile( const char* pPath )				 = 0;

	// NOTE: Those functions must be called from every thread (except IThread, there already they are uses)
	virtual void OnThreadRun()	= 0;
	virtual void OnThreadStop() = 0;
};

TIER0_INTERFACE ICrashDumpHandler* CrashDumpHandler();
