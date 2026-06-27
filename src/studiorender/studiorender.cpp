#include "pch_studiorender.h"
#include "filesystem/ifilesystem.h"
#include "materialsystem/ishadermgr.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "studiorender/studio_renderthread.h"
#include "studiorender/studio_viewport.h"
#include "studiorender/studio_renderpipelineset.h"
#include "studiorender/studio_vertexdeclarations.h"
#include "studiorender/studio_renderobject_quad.h"
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

	// Initialize the present pass
	presentRenderPass.Init();

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

	// Shutdown the present pass
	presentRenderPass.Shutdown();

	// Release all global resources
	CStudioGlobalRenderResources::ReleaseResources();
}

/*
==================
CStudioRender::SetCameraView
==================
*/
void CStudioRender::SetCameraView( const studioCameraView_t& cameraView )
{
	// Calculate a view matrix
	vector3_t targetDirection = cameraView.rotation * g_vectorForward;
	vector3_t axisUp		  = cameraView.rotation * g_vectorUp;
	S_MatrixLookAt( cameraView.location, cameraView.location + targetDirection, axisUp, sceneView.viewMatrix );

	// Calculate a perspective matrix
	S_MatrixPerspective( cameraView.fieldOfView, cameraView.aspectRatio, cameraView.nearClipPlane, cameraView.farClipPlane, sceneView.projectionMatrix );
}

/*
==================
CStudioRender::RegisterObject
==================
*/
void CStudioRender::RegisterObject( IStudioRenderObject* pRenderObject )
{
	renderObjects.emplace_back( pRenderObject );
}

/*
==================
CStudioRender::UnregisterObject
==================
*/
void CStudioRender::UnregisterObject( IStudioRenderObject* pRenderObject )
{
	for ( uint32 renderObjectIdx = 0, numRenderObjects = (uint32)renderObjects.size(); renderObjectIdx < numRenderObjects; ++renderObjectIdx )
	{
		if ( renderObjects[renderObjectIdx] == pRenderObject )
		{
			renderObjects.erase( renderObjects.begin() + renderObjectIdx );
			return;
		}
	}
}

/*
==================
CStudioRender::UnregisterAllObjects
==================
*/
void CStudioRender::UnregisterAllObjects()
{
	renderObjects.clear();
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
CStudioRender::CreateQuadRenderObject
==================
*/
CRefPtr<IStudioRenderObject> CStudioRender::CreateQuadRenderObject( IResource* pMaterial, IStudioAPIBuffer* pVertexBuffer, IStudioAPIBuffer* pIndexBuffer ) const
{
	return new CStudioRenderObjectQuad( pVertexBuffer, pIndexBuffer, pMaterial );
}

/*
==================
CStudioRender::BeginFrame
==================
*/
void CStudioRender::BeginFrame()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( Sys_IsInMainThread() );
}

/*
==================
CStudioRender::EndFrame
==================
*/
void CStudioRender::EndFrame()
{
	// TODO BS yehor.pohuliaka - Implement here synchronization the renderObjects between the main thread and the render thread
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( Sys_IsInMainThread() );
	Assert( !renderObjects.empty() );
	CStudioRenderObjectQuad* pRenderObject = (CStudioRenderObjectQuad*)renderObjects[0].GetRawPtr();
	pRenderObject->RefreshMaterialResource();
}

/*
==================
CStudioRender::R_DrawFrame
==================
*/

void CStudioRender::R_DrawFrame( CStudioViewport* pViewport )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( Studio_IsInRenderThread() );
	Assert( !renderObjects.empty() );
	presentRenderPass.R_DrawPass( pViewport, (CStudioRenderObjectQuad*)renderObjects[0].GetRawPtr() );
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
