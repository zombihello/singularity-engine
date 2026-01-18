#pragma once
#include "core/core.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class ILogger;

//-----------------------------------------------------------------------------
// Some globals
//-----------------------------------------------------------------------------
// Indicates that MainLoop should be exited at the end of the current iteration
extern bool g_bRequestingExit;

// Seconds per CPU cycle for this PC
extern double g_SecondsPerCycle;

//-----------------------------------------------------------------------------
// Internal core functions
//-----------------------------------------------------------------------------
double Sys_GetSecondsPerCycle();
#if ENABLE_LOGGING
void Sys_SetupDefaultLogOutputs( ILogger* pLogger );
#endif	// ENABLE_LOGGING
