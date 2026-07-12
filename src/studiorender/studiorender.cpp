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
	g_studioFrameAlloc.SwapPools();

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
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioRenderCmd_EndFrame,
										uint32, framePoolId, g_studioFrameAlloc.GetCurrentPoolId(),
										{
											g_pStudioAPI->EndDrawingFrame();
											g_studioFrameAlloc.MarkAsFreePool( framePoolId );
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
	studioSceneView_t* pSceneView		 = g_studioFrameAlloc.Construct<studioSceneView_t>();

	// Find all visible entities, after that creates entity views for them
	pStudioSceneLocal->FindEntityViews( pSceneView );

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
	uint32							  numMaterials		 = pModelResource->GetNumMaterials();
	const modelSurface_t*			  pSurfaces			 = pModelResource->GetSurfaces();
	const CRefPtr<IMaterialResource>* pMaterialResources = pModelResource->GetMaterials();

	// Go through each resource and add it to the scene view
	uint32	modelId		 = AddResourceToSceneView( pSceneView, pModelResource );
	uint32* pMaterialIds = (uint32*)Mem_Alloca( numMaterials * sizeof( uint32 ) );
	for ( uint32 index = 0; index < numMaterials; ++index )
	{
		pMaterialIds[index] = AddResourceToSceneView( pSceneView, pMaterialResources[index].GetRawPtr() );
	}

	// Go through each model surface and allocate draw surfaces
	for ( uint32 index = 0; index < numSurfaces; ++index )
	{
		const modelSurface_t& surface	   = pSurfaces[index];
		studioDrawSurface_t*  pDrawSurface = (studioDrawSurface_t*)g_studioFrameAlloc.Alloc( sizeof( studioDrawSurface_t ) );
		pDrawSurface->pEntityView		   = pEntityView;
		pDrawSurface->modelId			   = modelId;
		pDrawSurface->materialId		   = pMaterialIds[surface.materialId];
		pDrawSurface->baseVertexIndex	   = surface.baseVertexIndex;
		pDrawSurface->baseIndex			   = surface.baseIndex;
		pDrawSurface->numIndices		   = surface.numIndices;

		// Add the draw surface into the scene view and render passes
		studioRenderPass_t& renderPassPresent = pSceneView->renderPasses[STUDIO_RENDERPASS_TYPE_PRESENT];
		uint32				drawSurfaceId	  = (uint32)pSceneView->drawSurfaces.size();
		pSceneView->drawSurfaces.emplace_back( pDrawSurface );
		renderPassPresent.drawSurfaceIds.emplace_back( drawSurfaceId );
		renderPassPresent.resourceIds.insert( modelId );
		renderPassPresent.resourceIds.insert( pMaterialIds[surface.materialId] );
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
CStudioRender::AddResourceToSceneView
==================
*/
uint32 CStudioRender::AddResourceToSceneView( studioSceneView_t* pSceneView, studioResourcePtr_t pPtr, studioResourceType_t type )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	uint32 resourceId	= INVALID_INDEX;
	auto   itResourceId = pSceneView->resourceDict.find( pPtr );
	if ( itResourceId != pSceneView->resourceDict.end() )
	{
		resourceId = itResourceId->second;
	}
	else
	{
		studioResource_t* pResource = (studioResource_t*)g_studioFrameAlloc.Alloc( sizeof( studioResource_t ) );
		pResource->type				= type;
		pResource->pPtr				= pPtr;
		resourceId					= (uint32)pSceneView->resources.size();
		pSceneView->resources.emplace_back( pResource );
		pSceneView->resourceDict.insert( eastl::make_pair( pPtr, resourceId ) );
	}

	return resourceId;
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
