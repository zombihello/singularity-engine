/**
 * @file
 * @addtogroup core core
 */

#ifndef CORE_PRIVATE_H
#define CORE_PRIVATE_H

#include "core/core.h"

/**
 * @ingroup core
 * @brief Indicates that MainLoop should be exited at the end of the current iteration
 */
extern bool							g_bRequestingExit;

/**
 * @ingroup core
 * @brief Seconds per CPU cycle for this PC
 */
extern double						g_SecondsPerCycle;

/**
 * @ingroup core
 * @brief Is console IO was initialized for logs
 */
extern bool							g_bConsoleIOInited;

/**
 * @ingroup core
 * @brief Get seconds per CPU cycle for this PC
 * @note Need implement on each platform
 * 
 * @return Return seconds per CPU cycle for this PC
 */
double Sys_GetSecondsPerCycle();

#endif // !CORE_PRIVATE_H