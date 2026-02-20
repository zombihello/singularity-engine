#pragma once
#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier1/strtools.h"

//-----------------------------------------------------------------------------
// Debugger functions
//-----------------------------------------------------------------------------
#if !RETAIL
	#define Sys_DebugBreak() ( Sys_IsDebuggerAttached() ? ( DEBUG_BREAK(), true ) : true )
#else
	#define Sys_DebugBreak()
#endif	// !RETAIL

bool Sys_IsDebuggerAttached();
void Sys_DebugMessage( const char* pMsg );

#if PLATFORM_WINDOWS
	#include "tier0/platforms/windows/win_debug.inl"
#else
	#error Unknown platform
#endif	// PLATFORM_WINDOWS
