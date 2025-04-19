#ifndef VERSION_H
#define VERSION_H

#include "stdlib/template.h"
#include "stdlib/buildnum.h"

#define ENGINE_VERSION_MAJOR			0
#define ENGINE_VERSION_MINOR			1
#define ENGINE_VERSION_PATCH			0
#define ENGINE_BRANCH_NAME				"dev"
#define ENGINE_VERSION					PACK_VERSION( ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH )
#define ENGINE_VERSION_STRING			"0.1.0-dev"

FORCEINLINE uint32 Engine_BuildNumber()
{
	// 45113 - Jul 07 2024 (Singularity Engine development start)
	static TBuildNumber<45113>		s_BuildNumber;
	return s_BuildNumber.GetBuildNumber();
}

#endif // !VERSION_H