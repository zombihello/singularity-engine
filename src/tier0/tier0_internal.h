#pragma once
#include "tier0/tier0.h"

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
// Internal tier0 functions
//-----------------------------------------------------------------------------
double Sys_GetSecondsPerCycle();
