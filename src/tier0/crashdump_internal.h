#pragma once
#include "tier0/crashdump.h"

//-----------------------------------------------------------------------------
// Internal crash dump functions
//-----------------------------------------------------------------------------
// Installs crash dump handler to be called by the runtime in response to unhandled C++ exception
void CrashDump_SetupExceptionHandler();
void CrashDump_SetMessage( const char* pMsg );
