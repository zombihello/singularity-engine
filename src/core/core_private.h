#ifndef CORE_PRIVATE_H
#define CORE_PRIVATE_H

#include "core/core.h"

//-----------------------------------------------------------------------------
// Some globals
//-----------------------------------------------------------------------------
// Indicates that MainLoop should be exited at the end of the current iteration
extern bool							g_bRequestingExit;

// Seconds per CPU cycle for this PC
extern double						g_SecondsPerCycle;
extern bool							g_bConsoleIOInited;


//-----------------------------------------------------------------------------
// Internal core functions
//-----------------------------------------------------------------------------
double Sys_GetSecondsPerCycle();

#endif // !CORE_PRIVATE_H