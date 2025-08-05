#pragma once
#include "stdlib/defines.h"
#include "stdlib/types.h"

//-----------------------------------------------------------------------------
// Engine launcher interface
//-----------------------------------------------------------------------------
#define LAUNCHER_MAIN_FUNCNAME			"LauncherMain"
typedef uint32 ( *launcherMainFn_t )( appInstanceHandle_t hInstance, const achar* pDefaultGameDir, const achar* pCommandLine );