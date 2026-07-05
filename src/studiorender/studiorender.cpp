#include "pch_studiorender.h"
#include "filesystem/ifilesystem.h"
#include "resourcesystem/iresourcesystem.h"
#include "materialsystem/ishadermgr.h"
#include "modelsystem/imodel.h"
#include "modelsystem/imodelsystem.h"
#include "studiorender/studio_renderthread.h"
#include "studiorender/studio_viewport.h"
#include "studiorender/studio_renderpipelineset.h"
#include "studiorender/studio_sceneview.h"
#include "studiorender/studio_scene.h"
#include "studiorender/studiorender.h"

CCVar		  r_vsync( "r_vsync", "0", "Should use vertical synchronization (VSync)", CVAR_FLAG_ARCHIVE );
CStudioRender g_StudioRender;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CStudioRender, IStudioRender, STUDIORENDER_INTERFACE_VERSION, g_StudioRender );

/*
==================
CStudioRender::CStudioRender
==================
*/
CStudioRender::CStudioRender()
	: frameAlloc( "Studio FrameAlloc" )
{
}

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

	// Ge the resource system
	g_pResourceSystem = (IResourceSystem*)pFactory( RESOURCESYSTEM_INTERFACE_VERSION );
	if ( !g_pResourceSystem )
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

	g_pStudioAPI	  = NULL;
	g_pStudioRender	  = NULL;
	g_pShaderMgr	  = NULL;
	g_pModelSystem	  = NULL;
	g_pResourceSystem = NULL;
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
CStudioRender::CreateScene
==================
*/
CRefPtr<IStudioScene> CStudioRender::CreateScene() const
{
	return new CStudioScene();
}

/*
==================
CStudioRender::BeginFrame
==================
*/
void CStudioRender::BeginFrame()
{
	// Swap pools in the frame allocator
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	frameAlloc.SwapPools();

	// Tell StudioAPI about begin of drawing frame
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
	UNIQUE_RENDER_COMMAND_TWOPARAMETER( CStudioRenderCmd_EndFrame,
										studioFrameAlloc_t&, frameAlloc, frameAlloc,
										uint32, framePoolId, frameAlloc.GetCurrentPoolId(),
										{
											g_pStudioAPI->EndDrawingFrame();
											frameAlloc.MarkAsFreePool( framePoolId );
										} );
}

/*
==================
CStudioRender::DrawScene
==================
*/
void CStudioRender::DrawScene( IStudioViewport* pStudioViewport, IStudioScene* pStudioScene, const studioCameraView_t& cameraView )
{
	// Do nothing if the viewport or the scene isn't valid
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	if ( !pStudioViewport || !pStudioScene )
	{
		return;
	}

	// Cast the scene to our type and allocate a scene view
	CStudioScene*	   pStudioSceneLocal = (CStudioScene*)pStudioScene;
	studioSceneView_t* pSceneView		 = (studioSceneView_t*)frameAlloc.AllocZero( sizeof( studioSceneView_t ) );

	// Find all visible entities and lights, after that creates
	// entity views and lights for them
	pStudioSceneLocal->FindEntityViews( pSceneView );

	// Add draw surfaces into the scene view
	AddDrawSurfacesToSceneView( pSceneView );

	// Send a render scene command to the render thread
	UNIQUE_RENDER_COMMAND_TWOPARAMETER( CStudioRenderCmd_DrawScene,
										CRefPtr<CStudioViewport>, pViewport, (CStudioViewport*)pStudioViewport,
										studioSceneView_t*, pSceneView, pSceneView,
										{
											g_StudioRender.R_DrawScene( pViewport, pSceneView );
										} );
}

/*
==================
CStudioRender::AddDrawSurfacesToSceneView
==================
*/
void CStudioRender::AddDrawSurfacesToSceneView( studioSceneView_t* pSceneView )
{
	// Go through each view entity and create surfaces for it
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pSceneView->numDrawSurfaces == 0 );

	IResourceTypeMgr*	 pModelsMgr	   = g_pResourceSystem->GetResourceManagerForType<IModel>();
	CResourcePtr<IModel> pDefaultModel = pModelsMgr->GetDefaultResource();
	for ( studioEntityView_t* pEntityView = pSceneView->pEntityViews; pEntityView; pEntityView = pEntityView->pNext )
	{
		// Get entity's model, if it isn't cached use default one
		CResourcePtr<IModel> pModelRef = pEntityView->pEntity->params.pModel;
		IModelResource*		 pModel	   = pModelRef.IsCached() ? pModelRef->GetStudioResource() : pDefaultModel->GetStudioResource();
		if ( !pModel )
		{
			continue;
		}

		// Get in the model resource surfaces and materials
		uint32							  numSurfaces  = pModel->GetNumSurfaces();
		uint32							  numMaterials = pModel->GetNumMaterials();
		const modelSurface_t*			  pSurfaces	   = pModel->GetSurfaces();
		const CRefPtr<IMaterialResource>* pMaterials   = pModel->GetMaterials();

		// Add surfaces into the scene view
		pSceneView->numDrawSurfaces += numSurfaces;
		for ( uint32 index = 0; index < numSurfaces; index++ )
		{
			const modelSurface_t& surface	   = pSurfaces[index];
			studioDrawSurface_t*  pDrawSurface = frameAlloc.Construct<studioDrawSurface_t>();
			pDrawSurface->pEntityView		   = pEntityView;
			pDrawSurface->pModel			   = pModel;
			pDrawSurface->pMaterial			   = pMaterials[surface.materialId];
			pDrawSurface->baseVertexIndex	   = surface.baseVertexIndex;
			pDrawSurface->baseIndex			   = surface.baseIndex;
			pDrawSurface->numIndices		   = surface.numIndices;
			pDrawSurface->pNext				   = pEntityView->pDrawSurfaces;
			pEntityView->pDrawSurfaces		   = pDrawSurface;

			// Increase number of draw surfaces if the one should draw on a pass
			if ( CStudioRenderPassPresent::ShouldDrawSurfaceInPass( pDrawSurface ) )
			{
				studioRenderPass_t& renderPass										   = pSceneView->renderPasses[STUDIO_RENDERPASS_TYPE_PRESENT];
				pDrawSurface->bShouldDrawSurfaceInPass[STUDIO_RENDERPASS_TYPE_PRESENT] = true;
				++renderPass.numDrawSurfaces;
			}
		}
	}

	// Move the draw surfaces to the scene view
	Assert( !pSceneView->pDrawSurfaces );
	uint32 globalDrawSurfaceId = 0;
	uint32 localDrawSurfaceIds[STUDIO_RENDERPASS_NUM_TYPES];
	pSceneView->pDrawSurfaces = (studioDrawSurface_t**)frameAlloc.AllocZero( pSceneView->numDrawSurfaces * sizeof( studioEntityId_t* ) );
	for ( uint32 index = 0; index < STUDIO_RENDERPASS_NUM_TYPES; ++index )
	{
		studioRenderPass_t& renderPass = pSceneView->renderPasses[index];
		renderPass.pDrawSurfaceIds	   = (uint32*)frameAlloc.AllocZero( renderPass.numDrawSurfaces * sizeof( uint32 ) );
		localDrawSurfaceIds[index]	   = 0;
	}

	for ( studioEntityView_t* pEntityView = pSceneView->pEntityViews; pEntityView; pEntityView = pEntityView->pNext )
	{
		for ( studioDrawSurface_t* pDrawSurface = pEntityView->pDrawSurfaces; pDrawSurface; pDrawSurface = pDrawSurface->pNext, ++globalDrawSurfaceId )
		{
			// Add indices of the draw surfaces for each render pass if it need
			for ( uint32 renderPassId = 0; renderPassId < STUDIO_RENDERPASS_NUM_TYPES; ++renderPassId )
			{
				if ( pDrawSurface->bShouldDrawSurfaceInPass[renderPassId] )
				{
					studioRenderPass_t& renderPass				   = pSceneView->renderPasses[renderPassId];
					uint32&				localDrawSurfaceId		   = localDrawSurfaceIds[renderPassId];
					renderPass.pDrawSurfaceIds[localDrawSurfaceId] = globalDrawSurfaceId;
					++localDrawSurfaceId;
				}
			}

			// Add the draw surfaces to the scene view
			pSceneView->pDrawSurfaces[globalDrawSurfaceId] = pDrawSurface;
		}
	}
}

/*
==================
CStudioRender::R_DrawScene
==================
*/
void CStudioRender::R_DrawScene( CStudioViewport* pViewport, studioSceneView_t* pSceneView )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( Studio_IsInRenderThread() );
	presentRenderPass.R_DrawPass( pViewport, pSceneView );
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
