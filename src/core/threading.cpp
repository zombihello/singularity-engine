#include "pch_core.h"
#include "core/crashdump_private.h"
#include "core/threading_private.h"

// Main thread ID
static threadId_t		s_MainThreadId = INVALID_THREAD_ID;

/*
 ==================
 Sys_InitMainThread
 ==================
 */
void Sys_InitMainThread()
{
	CrashDump_OnThreadRun();
	CrashDump_SetupExceptionHandler();
	Sys_SetThreadName( Sys_GetCurrentThreadHandle(), "Main Thread" );
	s_MainThreadId = Sys_GetCurrentThreadId();
}

/*
 ==================
 Sys_IsInMainThread
 ==================
 */
bool Sys_IsInMainThread()
{
	return s_MainThreadId == INVALID_THREAD_ID || s_MainThreadId == Sys_GetCurrentThreadId();
}