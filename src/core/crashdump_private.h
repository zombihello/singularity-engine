#pragma once
#include "core/crashdump.h"

//-----------------------------------------------------------------------------
// Internal crash dump functions
//-----------------------------------------------------------------------------
// Installs crash dump handler to be called by the runtime in response to unhandled C++ exception
void CrashDump_SetupExceptionHandler();

// NOTE: Those functions must be called from every thread
void CrashDump_OnThreadRun();
void CrashDump_OnThreadStop();

void CrashDump_SetMessage( const achar* pMsg );