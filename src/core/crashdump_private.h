/**
 * @file
 * @addtogroup core core
 */

#ifndef CRASHDUMP_PRIVATE_H
#define CRASHDUMP_PRIVATE_H

#include "core/crashdump.h"

/**
 * @ingroup core
 * @brief Setup a crash dump handler
 * @note Need implement on each platform
 * Installs crash dump handler to be called by the runtime in response to unhandled C++ exception
 */
void CrashDump_SetupExceptionHandler();

/**
 * @ingroup core
 * @brief Notify the crash dumper about thread startup
 * @note Need implement on each platform.  Must be called from every thread
 */
void CrashDump_OnThreadRun();

/**
 * @ingroup core
 * @brief Notify the crash dumper about thread stop
 * @note Need implement on each platform.  Must be called from every thread
 */
void CrashDump_OnThreadStop();

/**
 * @ingroup core
 * @brief Set a message to a crash dump
 * @note Need implement on each platform
 * @param pMsg		Message
 */
void CrashDump_SetMessage( const achar* pMsg );

#endif // !CRASHDUMP_PRIVATE_H