/**
 * @file
 * @addtogroup core core
 */

#ifndef CRASHDUMP_H
#define CRASHDUMP_H

#include "core/core.h"

/**
 * @ingroup core
 * @brief Crash dump application information
 */
struct crashDumpAppInfo_t
{
	const achar*	pAppName;		/**< Application name. If NULL or empty string will be used default application name */
	const achar*	pAppVersion;	/**< Application version. If NULL or empty string will be used default application version */
	const achar*	pSupportEmail;	/**< Support email. If NULL or empty string will be used default email */
	const achar*	pSupportURL;	/**< Support URL. If NULL or empty string will be used default URL */
};

/**
 * @ingroup core
 * @brief Set information about the application
 * @note Need implement on each platform
 * @param appInfo		Application information
 */
CORE_INTERFACE void CrashDump_SetAppInfo( const crashDumpAppInfo_t& appInfo );

/**
 * @ingroup core
 * @brief Add a log file to the crash dump system
 * @note Need implement on each platform
 * @param pPath		Path to a log file
 */
CORE_INTERFACE void CrashDump_AddLogFile( const achar* pPath );

#endif // !CRASHDUMP_H