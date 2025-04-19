#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "core/types.h"
#include "core/platform.h"

//-----------------------------------------------------------------------------
// Engine launcher interface
//-----------------------------------------------------------------------------
#define LAUNCHER_MAIN_FUNCNAME			"LauncherMain"
typedef uint32 ( *launcherMainFn_t )( appInstanceHandle_t hInstance, const achar* pGameDir, const achar* pCommandLine );

#endif // !LAUNCHER_H