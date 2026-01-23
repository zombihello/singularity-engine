#pragma once
#include "tier1/builddefines.h"
#include "tier1/defines.h"

#if !defined( BUILD_STATIC_LIB )
	#ifdef TIER0_DLL_EXPORT
		#define TIER0_INTERFACE extern "C" DLL_EXPORT
		#define TIER0_CLASS		DLL_EXPORT
		#define TIER0_GLOBAL	extern DLL_EXPORT
	#else
		#define TIER0_INTERFACE extern "C" DLL_IMPORT
		#define TIER0_CLASS		DLL_IMPORT
		#define TIER0_GLOBAL	extern DLL_IMPORT
	#endif	// TIER0_DLL_EXPORT
#else
	#define TIER0_INTERFACE extern "C"
	#define TIER0_CLASS
	#define TIER0_GLOBAL extern
#endif	// !defined(BUILD_STATIC_LIB)
