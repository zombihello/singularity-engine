#ifndef STUDIO_RENDERTHREAD_H
#define STUDIO_RENDERTHREAD_H

#include "studiorender/studio_cmdbuffer.h"

//-----------------------------------------------------------------------------
// Render thread functions
//-----------------------------------------------------------------------------
extern CStudioCmdBuffer		g_StudioCmdBuffer;

FORCEINLINE bool Studio_IsInRenderThread()
{
	extern threadId_t	g_RenderThreadId;
	return g_RenderThreadId == INVALID_THREAD_ID || g_RenderThreadId == Sys_GetCurrentThreadId();
}

void Studio_StartRenderThread();
void Studio_StopRenderThread();

#endif // !STUDIO_RENDERTHREAD_H