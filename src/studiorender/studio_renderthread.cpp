#include "pch_studiorender.h"
#include "studiorender/istudio_rendercmd.h"
#include "studiorender/studio_renderthread.h"

//-----------------------------------------------------------------------------
// Constants and types
//-----------------------------------------------------------------------------
#define RENDER_COMMAND_BUFFER_SIZE ( 1024 * 1024 )

CStudioCmdBuffer		g_studioCmdBuffer( RENDER_COMMAND_BUFFER_SIZE, 16 );
threadId_t				g_renderThreadId		= INVALID_THREAD_ID;
static CThread*			s_pRenderThread			= NULL;
static CRunnableObject* s_pRenderRunnableObject = NULL;
static bool				s_bIsThreadedRendering	= false;

//-----------------------------------------------------------------------------
// Render runnable object
//-----------------------------------------------------------------------------
class CRenderRunnableObject : public CRunnableObject
{
public:
	virtual bool   Init() override;
	virtual uint32 Run() override;
	virtual void   Exit() override;
};

/*
==================
CRenderRunnableObject::Init
==================
*/
bool CRenderRunnableObject::Init()
{
	g_pStudioAPI->AcquireThreadOwnership();
	return true;
}

/*
==================
CRenderRunnableObject::Run
==================
*/
uint32 CRenderRunnableObject::Run()
{
	void*  pReadPointer = NULL;
	uint32 numReadBytes = 0;
	while ( s_bIsThreadedRendering )
	{
		// Command processing loop
		PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
		while ( g_studioCmdBuffer.BeginRead( pReadPointer, numReadBytes ) )
		{
			// Execute the Render Command
			IStudioRenderCmd* pCommand	  = (IStudioRenderCmd*)pReadPointer;
			uint32			  commandSize = pCommand->Execute();
			pCommand->~IStudioRenderCmd();
			g_studioCmdBuffer.EndRead( commandSize );
		}
	}

	return 0;
}

/*
==================
CRenderRunnableObject::Exit
==================
*/
void CRenderRunnableObject::Exit()
{
	g_pStudioAPI->ReleaseThreadOwnership();
}

/*
==================
Studio_StartRenderThread
==================
*/
void Studio_StartRenderThread()
{
	Assert( Sys_IsInMainThread() );
	if ( !s_bIsThreadedRendering )
	{
		// Turn on the threaded rendering flag
		s_bIsThreadedRendering = true;
		g_pStudioAPI->ReleaseThreadOwnership();

		// Start the render thread
		s_pRenderRunnableObject = new CRenderRunnableObject();
		s_pRenderThread			= new CThread();
		s_pRenderThread->SetName( "Render Thread" );
		s_pRenderThread->SetPriority( THREAD_PRIOR_REALTIME );
		if ( !s_pRenderThread->Start( s_pRenderRunnableObject ) )
		{
			Sys_Error( "Failed to start the render thread" );
		}

		// We are done
		g_renderThreadId = s_pRenderThread->GetId();
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
	Assert( Sys_IsInMainThread() );
	if ( s_bIsThreadedRendering )
	{
		// Wait for the rendering thread to return
		s_bIsThreadedRendering = false;
		s_pRenderThread->WaitForCompletion();

		// Destroy render thread objects
		delete s_pRenderRunnableObject;
		delete s_pRenderThread;
		s_pRenderThread			= NULL;
		s_pRenderRunnableObject = NULL;
		g_renderThreadId		= INVALID_THREAD_ID;
		g_pStudioAPI->AcquireThreadOwnership();
		Msg( "StudioRender: Render thread is stopped" );
	}
}
