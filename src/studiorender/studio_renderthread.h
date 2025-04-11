/**
 * @file
 * @addtogroup studiorender studiorender
 */

#ifndef STUDIO_RENDERTHREAD_H
#define STUDIO_RENDERTHREAD_H

#include "studiorender/studio_cmdbuffer.h"

/**
 * @ingroup studiorender
 * @brief The render command buffer
 */
extern CStudioCmdBuffer		g_StudioCmdBuffer;

/**
 * @ingroup studiorender
 * @brief Is current thread is the render thread
 * @return Return TRUE if it called from the render thread or render thread isn't use, otherwise returns FALSE
 */
FORCEINLINE bool Studio_IsInRenderThread()
{
	extern threadId_t	g_RenderThreadId;
	return g_RenderThreadId == INVALID_THREAD_ID || g_RenderThreadId == Sys_GetCurrentThreadId();
}

/**
 * @ingroup studiorender
 * @brief Starts the render thread
 */
void Studio_StartRenderThread();

/**
 * @ingroup studiorender
 * @brief Stops the render thread
 */
void Studio_StopRenderThread();

#endif // !STUDIO_RENDERTHREAD_H