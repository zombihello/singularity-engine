#include "pch_studiorender.h"
#include "filesystem/ifilesystem.h"
#include "materialsystem/ishadermgr.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "studiorender/studio_renderthread.h"
#include "studiorender/studio_viewport.h"
#include "studiorender/studio_renderpipelineset.h"
#include "studiorender/studio_vertexdeclarations.h"
#include "studiorender/studiorender.h"

CCVar		  r_vsync( "r_vsync", "0", "Should use vertical synchronization (VSync)", CVAR_FLAG_ARCHIVE );
CStudioRender g_StudioRender;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CStudioRender, IStudioRender, STUDIORENDER_INTERFACE_VERSION, g_StudioRender );

/*
==================
CStudioRender::Connect
==================
*/
bool CStudioRender::Connect( createInterfaceFn_t pFactory )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Connect Tier1
	if ( !ConnectTier1( pFactory ) )
	{
		return false;
	}
	LinkCmds();
	LinkCVars();

	// Get Studio API
	g_pStudioAPI = (IStudioAPI*)pFactory( STUDIOAPI_INTERFACE_VERSION );
	if ( !g_pStudioAPI )
	{
		return false;
	}

	// Get shader manager
	g_pShaderMgr = (IShaderMgr*)pFactory( SHADERMGR_INTERFACE_VERSION );
	if ( !g_pShaderMgr )
	{
		return false;
	}

	g_pStudioRender = this;
	return true;
}

/*
==================
CStudioRender::Disconnect
==================
*/
void CStudioRender::Disconnect()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Disconnect Tier1
	UnlinkCVars();
	UnlinkCmds();
	DisconnectTier1();

	g_pStudioAPI	= NULL;
	g_pStudioRender = NULL;
	g_pShaderMgr	= NULL;
}

/*
==================
CStudioRender::Init
==================
*/
bool CStudioRender::Init()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Initialize all global resources
	CStudioGlobalRenderResources::InitResources();

	// Start the render thread
	Studio_StartRenderThread();

	// We are done!
	return true;
}

/*
==================
CStudioRender::Shutdown
==================
*/
void CStudioRender::Shutdown()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Stop the render thread
	Studio_StopRenderThread();

	// Release all global resources
	CStudioGlobalRenderResources::ReleaseResources();
}

/*
==================
CStudioRender::CreateViewport
==================
*/
CRefPtr<IStudioViewport> CStudioRender::CreateViewport() const
{
	return new CStudioViewport();
}

/*
==================
CStudioRender::CreateRenderPipelineSet
==================
*/
CRefPtr<IStudioRenderPipelineSet> CStudioRender::CreateRenderPipelineSet() const
{
	return new CStudioRenderPipelineSet();
}

/*
==================
CStudioRender::BeginFrame
==================
*/
void CStudioRender::BeginFrame()
{
	// Tell StudioAPI about begin of drawing frame
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	UNIQUE_RENDER_COMMAND( CStudioRenderCmd_BeginFrame,
						   {
							   g_pStudioAPI->BeginDrawingFrame();
						   } );
}

/*
==================
CStudioRender::EndFrame
==================
*/
void CStudioRender::EndFrame()
{
	// Tell StudioAPI about end of drawing frame and free the pool in the frame allocator
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	UNIQUE_RENDER_COMMAND( CStudioRenderCmd_EndFrame,
						   {
							   g_pStudioAPI->EndDrawingFrame();
						   } );
}

/*
==================
CStudioRender::GetCommandBuffer
==================
*/
IStudioCmdBuffer* CStudioRender::GetCommandBuffer() const
{
	return &g_studioCmdBuffer;
}

/*
==================
CStudioRender::IsInRenderThreads
==================
*/
bool CStudioRender::IsInRenderThread() const
{
	return Studio_IsInRenderThread();
}
