#include "pch_studiorender.h"
#include "studiorender/istudio_rendercmd.h"
#include "studiorender/studio_renderthread.h"

/**
 * @ingroup studiorender
 * @brief The size of the render command buffer, in bytes
 */
#define RENDER_COMMAND_BUFFER_SIZE			( 1024 * 1024 )

/**
 * @ingroup studiorender
 * @brief Whether the renderer is currently running in a separate thread
 */
static bool				s_bIsThreadedRendering = false;

// The render command buffer
CStudioCmdBuffer		g_StudioCmdBuffer( RENDER_COMMAND_BUFFER_SIZE, 16 );

/**
 * @ingroup studiorender
 * @brief The render thread id
 */
threadId_t				g_RenderThreadId = INVALID_THREAD_ID;


/**
 * @ingroup studiorender
 * @brief The render thread
 */
class CRenderThread : public CThread
{
protected:
	/**
	 * @brief Initialize a thread
	 * @note Called in the context of the aggregating thread
	 * @note By default is empty and always returns TRUE
	 * 
	 * Allows per thread initialization
	 *
	 * @return Return TRUE if initialization was successful, FALSE otherwise
	 */
	virtual bool ThreadInit() override
	{
		// Acquire rendering context ownership on the current thread
		g_pStudioAPI->AcquireThreadOwnership();

		// Remember render thread id
		g_RenderThreadId = GetThreadId();

		// Set realtime priority and debug name for the thread
		SetPriority( THREAD_PRIOR_REALTIME );
		SetName( "Render Thread" );
		return true;
	}

	/**
	 * @brief Run a thread
	 * @note Called in the context of the aggregating thread
	 * 
	 * This is where all per thread work is done. This is only called
	 * if the initialization was successful
	 *
	 * @return Return the exit code of the thread
	 */
	virtual uint32 ThreadRun() override
	{
		void*	pReadPointer = nullptr;
		uint32	numReadBytes = 0;
		while ( s_bIsThreadedRendering )
		{	
			// Command processing loop
			while ( g_StudioCmdBuffer.BeginRead( pReadPointer, numReadBytes ) )
			{
				// Execute the Render Command
				IStudioRenderCmd*		pCommand = ( IStudioRenderCmd* )pReadPointer;
				uint32					commandSize = pCommand->Execute();
				pCommand->~IStudioRenderCmd();
				g_StudioCmdBuffer.EndRead( commandSize );
			}
		}
		
		return 0;
	}

	/**
	 * @brief Exit a thread
	 * @note Called in the context of the aggregating thread
	 * @note By default is empty
	 * 
	 * Called to perform any cleanup
	 */
	virtual void ThreadExit() override
	{
		// Release rendering context ownership on the current thread
		g_pStudioAPI->ReleaseThreadOwnership();

		// Reset render thread id
		g_RenderThreadId = INVALID_THREAD_ID;
	}
};


/** Thread used for rendering */
static CRenderThread*	s_pRenderThread = NULL;

/*
==================
Studio_StartRenderThread
==================
*/
void Studio_StartRenderThread()
{
	if ( !s_bIsThreadedRendering )
	{
		// Turn on the threaded rendering flag
		s_bIsThreadedRendering = true;

		// Create the render thread
		s_pRenderThread = new CRenderThread();

		// Release rendering context ownership on the current thread
		g_pStudioAPI->ReleaseThreadOwnership();

		// Start thread
		const uint32	stackSize = 0;
		bool			bResult = s_pRenderThread->Start( stackSize );
		if ( !bResult )
		{
			Sys_Error( "Failed to start the render thread" );
		}

		Msg( "StudioRender: Render thread is started" );
	}
}

/*
==================
Studio_StopRenderThread
==================
*/
void Studio_StopRenderThread()
{
	// This function is not thread-safe. Ensure it is only called by the main thread
	Assert( Sys_IsInMainThread() );

	static bool		s_bIsRenderThreadStopping = false;
	if ( s_bIsThreadedRendering && !s_bIsRenderThreadStopping )
	{
		s_bIsRenderThreadStopping = true;

		// The rendering thread may have already been stopped
		if ( s_bIsThreadedRendering )
		{
			// Turn off the threaded rendering flag
			s_bIsThreadedRendering = false;

			// Reset the render thread id
			g_RenderThreadId = INVALID_THREAD_ID;

			// Wait for the rendering thread to return
			s_pRenderThread->WaitForCompletion();

			// We must stop the thread here, so that it correctly frees up the render thread handle
			// without this we get thread leaks when the device is lost
			s_pRenderThread->Stop();

			// Destroy the render thread object
			delete s_pRenderThread;
			s_pRenderThread = NULL;

			// Acquire rendering context ownership on the current thread
			g_pStudioAPI->AcquireThreadOwnership();
		}
	}

	Msg( "StudioRender: Render thread is stopped" );
	s_bIsRenderThreadStopping = false;
}