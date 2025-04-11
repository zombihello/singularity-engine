/**
 * @file
 * @addtogroup core core
 */

#ifndef COREAPI_H
#define COREAPI_H

#include "core/build.h"
#include "core/platform.h"

#if !defined( BUILD_STATIC_LIB ) && !defined( BUILD_MONOLITHIC_ENGINE )
	#ifdef CORE_DLL_EXPORT
		#define CORE_INTERFACE		extern "C" DLL_EXPORT
		#define CORE_CLASS			DLL_EXPORT
		#define CORE_GLOBAL			extern DLL_EXPORT
	#else
		#define CORE_INTERFACE		extern "C" DLL_IMPORT
		#define CORE_CLASS			DLL_IMPORT
		#define CORE_GLOBAL			extern DLL_IMPORT
	#endif // CORE_DLL_EXPORT
#else
	#define CORE_INTERFACE			extern "C"
	#define CORE_CLASS
	#define CORE_GLOBAL				extern
#endif // !BUILD_STATIC_LIB || BUILD_MONOLITHIC_ENGINE

#endif // !COREAPI_H