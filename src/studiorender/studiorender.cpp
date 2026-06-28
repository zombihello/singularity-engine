#include "pch_studiorender.h"
#include "filesystem/ifilesystem.h"
#include "materialsystem/ishadermgr.h"
#include "modelsystem/imodelsystem.h"
#include "studiorender/studio_renderthread.h"
#include "studiorender/studio_viewport.h"
#include "studiorender/studio_renderpipelineset.h"
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
	// Connect Tier1
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	if ( !ConnectTier1( pFactory ) )
	{
		return false;
	}
	LinkCmds();
	LinkCVars();

	// Get the studio api
	g_pStudioAPI = (IStudioAPI*)pFactory( STUDIOAPI_INTERFACE_VERSION );
	if ( !g_pStudioAPI )
	{
		return false;
	}

	// Get the shader manager
	g_pShaderMgr = (IShaderMgr*)pFactory( SHADERMGR_INTERFACE_VERSION );
	if ( !g_pShaderMgr )
	{
		return false;
	}

	// Get the model system
	g_pModelSystem = (IModelSystem*)pFactory( MODELSYSTEM_INTERFACE_VERSION );
	if ( !g_pModelSystem )
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
	UnlinkCVars();
	UnlinkCmds();
	DisconnectTier1();

	g_pStudioAPI	= NULL;
	g_pStudioRender = NULL;
	g_pShaderMgr	= NULL;
	g_pModelSystem	= NULL;
}

/*
==================
CStudioRender::Init
==================
*/
bool CStudioRender::Init()
{
	// Initialize all global resources
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	CStudioGlobalRenderResources::InitResources();

	// Start the render thread
	Studio_StartRenderThread();
	return true;
}

/*
==================
CStudioRender::Shutdown
==================
*/
void CStudioRender::Shutdown()
{
	// Stop the render thread
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
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
