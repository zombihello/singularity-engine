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

	// Find all visible entities, after that creates entity views for them
	pStudioSceneLocal->FindEntityViews( pSceneView );

	// Preallocate memory for draw surfaces
	pSceneView->numDrawSurfaces = 0;
	pSceneView->maxDrawSurfaces = STUDIO_SCENEVIEW_INITIAL_NUM_DRAWSURFACES;
	pSceneView->pDrawSurfaces	= (studioDrawSurface_t**)frameAlloc.Alloc( pSceneView->maxDrawSurfaces * sizeof( studioDrawSurface_t* ) );

	// Go through each entity view and add draw surfaces to the scene view
	for ( studioEntityView_t* pEntityView = pSceneView->pEntityViews; pEntityView; pEntityView = pEntityView->pNext )
	{
		AddModelToSceneView( pSceneView, pEntityView );
	}

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
CStudioRender::AddModelToSceneView
==================
*/
void CStudioRender::AddModelToSceneView( studioSceneView_t* pSceneView, studioEntityView_t* pEntityView )
{
	// Get the default model for case when an entity's model has been uncached
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	IResourceTypeMgr*	 pModelsMgr	   = g_pResourceSystem->GetResourceManagerForType<IModel>();
	CResourcePtr<IModel> pDefaultModel = pModelsMgr->GetDefaultResource();

	// Get entity's model, if it isn't cached use default one
	CResourcePtr<IModel> pModel			= pEntityView->pEntity->params.pModel;
	IModelResource*		 pModelResource = pModel.IsCached() ? pModel->GetStudioResource() : pDefaultModel->GetStudioResource();
	if ( !pModelResource )
	{
		return;
	}

	// Get all surfaces and materials
	uint32							  numSurfaces		 = pModelResource->GetNumSurfaces();
	const modelSurface_t*			  pSurfaces			 = pModelResource->GetSurfaces();
	const CRefPtr<IMaterialResource>* pMaterialResources = pModelResource->GetMaterials();

	// Resize the draw surfaces list if it doesn't fit
	uint32 requiredCapacity = pSceneView->numDrawSurfaces + numSurfaces;
	if ( requiredCapacity > pSceneView->maxDrawSurfaces )
	{
		studioDrawSurface_t** pOldList	  = pSceneView->pDrawSurfaces;
		uint32				  oldCapacity = pSceneView->maxDrawSurfaces;
		while ( pSceneView->maxDrawSurfaces < requiredCapacity )
		{
			pSceneView->maxDrawSurfaces *= 2;
		}
		pSceneView->pDrawSurfaces = (studioDrawSurface_t**)frameAlloc.Alloc( pSceneView->maxDrawSurfaces * sizeof( studioDrawSurface_t* ) );
		Mem_Memcpy( pSceneView->pDrawSurfaces, pOldList, oldCapacity * sizeof( studioDrawSurface_t* ) );
	}

	// Go through each model surface and add draw surfaces to the scene view
	for ( uint32 index = 0; index < numSurfaces; index++ )
	{
		const modelSurface_t& surface						   = pSurfaces[index];
		studioDrawSurface_t*  pDrawSurface					   = frameAlloc.Construct<studioDrawSurface_t>();
		pDrawSurface->pEntityView							   = pEntityView;
		pDrawSurface->pModel								   = pModelResource;
		pDrawSurface->pMaterial								   = pMaterialResources[surface.materialId];
		pDrawSurface->baseVertexIndex						   = surface.baseVertexIndex;
		pDrawSurface->baseIndex								   = surface.baseIndex;
		pDrawSurface->numIndices							   = surface.numIndices;
		pSceneView->pDrawSurfaces[pSceneView->numDrawSurfaces] = pDrawSurface;
		++pSceneView->numDrawSurfaces;
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
