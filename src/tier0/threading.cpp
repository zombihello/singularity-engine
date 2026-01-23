#include "pch_tier0.h"
#include "tier0/crashdump_private.h"
#include "tier0/threading_private.h"

// Main thread ID
static threadId_t s_MainThreadId = INVALID_THREAD_ID;

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
