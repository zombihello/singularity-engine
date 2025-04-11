/**
 * @file
 * @addtogroup engine engine
 */

#ifndef VERSION_H
#define VERSION_H

#include "stdlib/template.h"
#include "stdlib/buildnum.h"

/**
 * @ingroup engine
 * @brief Engine version major
 */
#define ENGINE_VERSION_MAJOR			0

/**
 * @ingroup engine
 * @brief Engine version minor
 */
#define ENGINE_VERSION_MINOR			1

/**
* @ingroup engine
* @brief Engine version patch
*/
#define ENGINE_VERSION_PATCH			0

/**
 * @ingroup engine
 * @brief Engine branch name
 */
#define ENGINE_BRANCH_NAME				"dev"

/**
 * @ingroup engine
 * @brief Engine version
 */
#define ENGINE_VERSION					PACK_VERSION( ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH )

/**
 * @ingroup engine
 * @brief Engine version in string format
 */
#define ENGINE_VERSION_STRING			"0.1.0-dev"

/**
 * @ingroup engine
 * @brief Get engine build number
 * @return Return engine build number
 */
FORCEINLINE uint32 Engine_BuildNumber()
{
	// 45113 - Jul 07 2024 (Singularity Engine development start)
	static TBuildNumber<45113>		s_BuildNumber;
	return s_BuildNumber.GetBuildNumber();
}

#endif // !VERSION_H