/**
 * @file
 * @addtogroup launcher launcher
 */

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "core/types.h"
#include "core/platform.h"

/**
 * @ingroup launcher
 * @brief Launcher function name in a DLL
 */
#define LAUNCHER_MAIN_FUNCNAME			"LauncherMain"

/**
 * @ingroup launcher
 * @brief Launcher main function
 */
typedef uint32 ( *launcherMainFn_t )( appInstanceHandle_t hInstance, const achar* pGameDir, const achar* pCommandLine );

#endif // !LAUNCHER_H