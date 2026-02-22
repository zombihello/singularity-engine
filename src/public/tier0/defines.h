#pragma once

//-----------------------------------------------------------------------------
// Build specific macros
//-----------------------------------------------------------------------------
#ifndef PLATFORM_32BIT
	#define PLATFORM_32BIT 0
#endif	// !PLATFORM_32BIT

#ifndef PLATFORM_64BIT
	#define PLATFORM_64BIT 0
#endif	// !PLATFORM_64BIT

#ifndef ENABLE_ASSERT
	#define ENABLE_ASSERT !RETAIL
#endif	// !ENABLE_ASSERT

#ifndef ENABLE_ENSURE
	#define ENABLE_ENSURE !RETAIL
#endif	// !ENABLE_ENSURE

#ifndef ENABLE_LOGGING
	#define ENABLE_LOGGING 1
#endif	// !ENABLE_LOGGING

#ifndef ENABLE_PROFILING
	#define ENABLE_PROFILING !RETAIL
#endif	// !ENABLE_PROFILING

//-----------------------------------------------------------------------------
// Platform specific macros
//-----------------------------------------------------------------------------
// By default all defines PLATFORM_XXX are 0
#define PLATFORM_WINDOWS		  0
#define PLATFORM_USE_WCHAR_STDOUT 0

#if _WIN32 || _WIN64  // Windows platform
	#include "tier0/platforms/windows/win_defines.h"
#else  // Unknown platform
	#error Unknown platform
#endif	// _WIN32 || _WIN64

//-----------------------------------------------------------------------------
// Generic macros
//-----------------------------------------------------------------------------
#undef ARRAYSIZE
#undef OFFSET_OF

#define DEPRECATED( Version, Message )		[[deprecated( Message " Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile." )]]
#define NOOP								(void)0
#define UNUSED_VAR( Var )					(void)( Var )
#define CONCAT_IMPL( X, Y )					X##Y
#define CONCAT( X, Y )						CONCAT_IMPL( X, Y )
#define ARRAYSIZE( Array )					( sizeof( Array ) / sizeof( ( Array )[0] ) )
#define OFFSET_OF( Struct, Member )			( ( ptrint ) & ( (Struct*)0 )->Member )
#define PACK_VERSION( Major, Minor, Patch ) ( (uint32)Major << 24 | (uint32)Minor << 16 | Patch )
#define UNPACK_VERSION( Version, Major, Minor, Patch ) \
	{                                                  \
		Major = ( (uint32)Version >> 24 ) & 0xFF;      \
		Minor = ( (uint32)Version >> 16 ) & 0xFF;      \
		Patch = (uint32)Version & 0xFFFF;              \
	}
#define BIT( Bit )			( 1 << Bit )
#define STRINGIFY_IMPL( X ) #X
#define STRINGIFY( X )		STRINGIFY_IMPL( X )
