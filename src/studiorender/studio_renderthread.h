#pragma once
#include "studiorender/studio_cmdbuffer.h"

//-----------------------------------------------------------------------------
// Render thread functions
//-----------------------------------------------------------------------------
extern CStudioCmdBuffer g_studioCmdBuffer;

FORCEINLINE bool Studio_IsInRenderThread()
{
	extern threadId_t g_renderThreadId;
	return g_renderThreadId == INVALID_THREAD_ID || g_renderThreadId == Sys_GetCurrentThreadId();
}

void Studio_StartRenderThread();
void Studio_StopRenderThread();