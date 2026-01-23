#pragma once
#include "tier0/defines.h"
#include "tier0/types.h"

//-----------------------------------------------------------------------------
// Engine launcher interface
//-----------------------------------------------------------------------------
#define LAUNCHER_MAIN_FUNCNAME "LauncherMain"
typedef uint32 ( *launcherMainFn_t )( appInstanceHandle_t hInstance, const char* pDefaultGameDir, const char* pCommandLine );