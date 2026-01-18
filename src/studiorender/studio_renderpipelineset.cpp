#include "pch_studiorender.h"
#include "studiorender/studio_viewport.h"
#include "studiorender/studio_renderthread.h"
#include "studiorender/studio_vertexdeclarations.h"
#include "studiorender/studio_renderpass_present.h"
#include "studiorender/studio_renderpipelineset.h"

/*
==================
CStudioRenderPipelineSet::CRenderPipelineContainer::CRenderPipelineContainer
==================
*/
CStudioRenderPipelineSet::CRenderPipelineContainer::CRenderPipelineContainer()
	: renderPassType( STUDIO_RENDERPASS_NUM_TYPES )
	, pDataStorage( NULL )
{
}

/*
==================
CStudioRenderPipelineSet::CRenderPipelineContainer::~CRenderPipelineContainer
==================
*/
CStudioRenderPipelineSet::CRenderPipelineContainer::~CRenderPipelineContainer()
{
	AssertMsg( !pDataStorage, "Before destroy you must call Destroy" );
}

/*
==================
CStudioRenderPipelineSet::CRenderPipelineContainer::Init
==================
*/
void CStudioRenderPipelineSet::CRenderPipelineContainer::Init( studioRenderPassType_t renderPassType )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Initialize a data storage
	AssertMsg( !pDataStorage, "The render pipeline container already initialized" );
	Assert( renderPassType < STUDIO_RENDERPASS_NUM_TYPES );
	if ( renderPassType != STUDIO_RENDERPASS_TYPE_PRESENT )
	{
		pDataStorage = new dataStorageDrawRenderPasses_t();
	}
	else
	{
		pDataStorage = new dataStoragePresentPass_t();
	}

	// Remember the pass type
	CRenderPipelineContainer::renderPassType = renderPassType;
}

/*
==================
CStudioRenderPipelineSet::CRenderPipelineContainer::Destroy
==================
*/
void CStudioRenderPipelineSet::CRenderPipelineContainer::Destroy()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	if ( pDataStorage )
	{
		if ( renderPassType != STUDIO_RENDERPASS_TYPE_PRESENT )
		{
			// Remove render pipelines, do it on the render thread to make sure that they will be destroyed at the render thread
			dataStorageDrawRenderPasses_t* pDataStorageDraw = (dataStorageDrawRenderPasses_t*)pDataStorage;
			UNIQUE_RENDER_COMMAND_ONEPARAMETER( CRemoveRenderPipelinesCmd, std::vector<TRefPtr<IStudioAPIRenderPipeline>>, studioAPIRenderPipelines, std::move( pDataStorageDraw->studioAPIRenderPipelines ),
												{
													studioAPIRenderPipelines.clear();
												} );
			delete pDataStorageDraw;
		}
		else
		{
			dataStoragePresentPass_t* pDataStoragePresent = (dataStoragePresentPass_t*)pDataStorage;
			for ( uint32 viewportIdx = 0, numViewports = (uint32)pDataStoragePresent->viewports.size(); viewportIdx < numViewports; ++viewportIdx )
			{
				// Remove all delegates
				viewportRenderPipelines_t& viewportRenderPipelines = pDataStoragePresent->viewports[viewportIdx];
				if ( !viewportRenderPipelines.pViewport )
				{
					Assert( !viewportRenderPipelines.pReleaseViewportIndexDelegate && !viewportRenderPipelines.pRenderPassUpdatedDelegate && viewportRenderPipelines.studioAPIRenderPipelines.empty() );
					continue;
				}

				if ( viewportRenderPipelines.pReleaseViewportIndexDelegate )
				{
					viewportRenderPipelines.pViewport->OnReleaseViewportIndex().RemoveFunc( viewportRenderPipelines.pReleaseViewportIndexDelegate );
					viewportRenderPipelines.pReleaseViewportIndexDelegate = NULL;
				}

				if ( viewportRenderPipelines.pRenderPassUpdatedDelegate )
				{
					viewportRenderPipelines.pViewport->OnRenderPassUpdated().RemoveFunc( viewportRenderPipelines.pRenderPassUpdatedDelegate );
					viewportRenderPipelines.pRenderPassUpdatedDelegate = NULL;
				}

				// Remove render pipelines, do it on the render thread to make sure that they will be destroyed at the render thread
				viewportRenderPipelines.pViewport = NULL;
				UNIQUE_RENDER_COMMAND_ONEPARAMETER( CRemoveRenderPipelinesCmd, std::vector<TRefPtr<IStudioAPIRenderPipeline>>, studioAPIRenderPipelines, std::move( viewportRenderPipelines.studioAPIRenderPipelines ),
													{
														studioAPIRenderPipelines.clear();
													} );
			}

			// Free allocated memory for viewports
			delete pDataStoragePresent;
		}
	}

	pDataStorage   = NULL;
	renderPassType = STUDIO_RENDERPASS_NUM_TYPES;
}

/*
==================
CStudioRenderPipelineSet::CRenderPipelineContainer::R_BakeRenderPipeline
==================
*/
IStudioAPIRenderPipeline* CStudioRenderPipelineSet::CRenderPipelineContainer::R_BakeRenderPipeline( const studioBakeRenderPipelineParams_t& bakeParams )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// For non-present passes we get the array as is
	std::vector<TRefPtr<IStudioAPIRenderPipeline>>* pStudioAPIRenderPipelines = NULL;
	CStudioViewport*								pActiveViewport			  = CStudioViewport::R_GetActiveViewport();
	if ( renderPassType != STUDIO_RENDERPASS_TYPE_PRESENT )
	{
		dataStorageDrawRenderPasses_t* pDataStorageDraw = (dataStorageDrawRenderPasses_t*)pDataStorage;
		pStudioAPIRenderPipelines						= &pDataStorageDraw->studioAPIRenderPipelines;
	}
	// For a present pass we get the array for now the active viewport
	else
	{
		dataStoragePresentPass_t* pDataStoragePresent = (dataStoragePresentPass_t*)pDataStorage;
		Assert( pActiveViewport );

		// If we haven't a render pipelines for the viewport then initialize it now
		uint32 viewportIndex = pActiveViewport->GetIndex();
		if ( viewportIndex >= (uint32)pDataStoragePresent->viewports.size() )
		{
			pDataStoragePresent->viewports.resize( viewportIndex + 1 );
			viewportRenderPipelines_t* pViewportRenderPipelines		= &pDataStoragePresent->viewports[viewportIndex];
			pStudioAPIRenderPipelines								= &pViewportRenderPipelines->studioAPIRenderPipelines;
			pViewportRenderPipelines->pViewport						= pActiveViewport;
			pViewportRenderPipelines->pReleaseViewportIndexDelegate = pActiveViewport->OnReleaseViewportIndex().AddFunc( &CRenderPipelineContainer::OnReleaseViewportIndex, this );
			pViewportRenderPipelines->pRenderPassUpdatedDelegate	= pActiveViewport->OnRenderPassUpdated().AddFunc( &CRenderPipelineContainer::OnRenderPassUpdated, this );
		}
		else
		{
			pStudioAPIRenderPipelines = &pDataStoragePresent->viewports[viewportIndex].studioAPIRenderPipelines;
		}
	}

	// Leave place for a new render pipeline
	if ( bakeParams.pipelineIdx >= (uint64)pStudioAPIRenderPipelines->size() )
	{
		pStudioAPIRenderPipelines->resize( bakeParams.pipelineIdx + 1 );
	}

	// Create a new render pipeline
	TRefPtr<IStudioAPIBoundShaderState> pStudioAPIBoundShaderState = g_pStudioAPI->FindOrCreateBoundShaderState( g_StudioVertexDeclarations.GetStudioAPIVertexDeclaration( bakeParams.vertexType ),
																												 bakeParams.pStudioAPIShaders[STUDIOAPI_SHADER_TYPE_VERTEX],
																												 bakeParams.pStudioAPIShaders[STUDIOAPI_SHADER_TYPE_PIXEL],
																												 bakeParams.pStudioAPIShaders[STUDIOAPI_SHADER_TYPE_HULL],
																												 bakeParams.pStudioAPIShaders[STUDIOAPI_SHADER_TYPE_DOMAIN],
																												 bakeParams.pStudioAPIShaders[STUDIOAPI_SHADER_TYPE_GEOMETRY] );
	TRefPtr<IStudioAPIRenderPipeline>	pStudioAPIRenderPipeline;
	switch ( renderPassType )
	{
		// Present
	case STUDIO_RENDERPASS_TYPE_PRESENT:
		pStudioAPIRenderPipeline = CStudioRenderPassPresent::R_CreateStudioAPIRenderPipeline( pActiveViewport, pStudioAPIBoundShaderState );
		break;

		// Unknown render pass type
	default:
		Sys_Error( "Unknown render pass type 0x%X", renderPassType );
		break;
	}

	( *pStudioAPIRenderPipelines )[bakeParams.pipelineIdx] = pStudioAPIRenderPipeline;
	return pStudioAPIRenderPipeline;
}

/*
==================
CStudioRenderPipelineSet::CRenderPipelineContainer::GetStudioAPIRenderPipeline
==================
*/
IStudioAPIRenderPipeline* CStudioRenderPipelineSet::CRenderPipelineContainer::GetStudioAPIRenderPipeline( uint64 pipelineIdx ) const
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Draw render pipelines
	Assert( pDataStorage && renderPassType != STUDIO_RENDERPASS_NUM_TYPES );
	if ( renderPassType != STUDIO_RENDERPASS_TYPE_PRESENT )
	{
		dataStorageDrawRenderPasses_t* pDataStorageDraw = (dataStorageDrawRenderPasses_t*)pDataStorage;
		return pipelineIdx < (uint32)pDataStorageDraw->studioAPIRenderPipelines.size() ? pDataStorageDraw->studioAPIRenderPipelines[pipelineIdx] : NULL;
	}

	// Present render pipelines
	dataStoragePresentPass_t* pDataStoragePresent = (dataStoragePresentPass_t*)pDataStorage;
	CStudioViewport*		  pActiveViewport	  = CStudioViewport::R_GetActiveViewport();
	Assert( pActiveViewport );
	uint32 viewportIndex = pActiveViewport->GetIndex();
	if ( viewportIndex >= (uint32)pDataStoragePresent->viewports.size() || pipelineIdx >= (uint32)pDataStoragePresent->viewports[viewportIndex].studioAPIRenderPipelines.size() )
	{
		return NULL;
	}

	return pDataStoragePresent->viewports[viewportIndex].studioAPIRenderPipelines[pipelineIdx];
}

/*
==================
CStudioRenderPipelineSet::CRenderPipelineContainer::OnReleaseViewportIndex
==================
*/
void CStudioRenderPipelineSet::CRenderPipelineContainer::OnReleaseViewportIndex( void* pUserData, CStudioViewport* pViewport )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	Assert( pUserData );
	CRenderPipelineContainer* pRenderPipelineContainer = (CRenderPipelineContainer*)pUserData;
	dataStoragePresentPass_t* pDataStoragePresent	   = (dataStoragePresentPass_t*)pRenderPipelineContainer->pDataStorage;
	Assert( pRenderPipelineContainer->renderPassType == STUDIO_RENDERPASS_TYPE_PRESENT && pDataStoragePresent );

	uint32 viewportIndex = pViewport->GetIndex();
	if ( viewportIndex < (uint32)pDataStoragePresent->viewports.size() )
	{
		// Remove all delegates
		viewportRenderPipelines_t& viewportRenderPipelines = pDataStoragePresent->viewports[viewportIndex];
		Assert( viewportRenderPipelines.pViewport == pViewport );
		if ( viewportRenderPipelines.pReleaseViewportIndexDelegate )
		{
			// We not remove delegate for event 'OnReleaseViewportIndex' because the event is calls in destructor of a viewport
			viewportRenderPipelines.pReleaseViewportIndexDelegate = NULL;
		}

		if ( viewportRenderPipelines.pRenderPassUpdatedDelegate )
		{
			viewportRenderPipelines.pViewport->OnRenderPassUpdated().RemoveFunc( viewportRenderPipelines.pRenderPassUpdatedDelegate );
			viewportRenderPipelines.pRenderPassUpdatedDelegate = NULL;
		}
		viewportRenderPipelines.pViewport = NULL;

		// Remove render pipelines, do it on the render thread to make sure that they will be destroyed at the render thread
		UNIQUE_RENDER_COMMAND_ONEPARAMETER( CRemoveCachedPipelinesCmd, std::vector<TRefPtr<IStudioAPIRenderPipeline>>, studioAPIRenderPipelines, std::move( viewportRenderPipelines.studioAPIRenderPipelines ),
											{
												studioAPIRenderPipelines.clear();
											} );
	}
}

/*
==================
CStudioRenderPipelineSet::CRenderPipelineContainer::OnRenderPassUpdated
==================
*/
void CStudioRenderPipelineSet::CRenderPipelineContainer::OnRenderPassUpdated( void* pUserData, CStudioViewport* pViewport )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// If a viewport render pass was updated remove all cached pipelines for the viewport
	Assert( pUserData );
	CRenderPipelineContainer* pRenderPipelineContainer = (CRenderPipelineContainer*)pUserData;
	dataStoragePresentPass_t* pDataStoragePresent	   = (dataStoragePresentPass_t*)pRenderPipelineContainer->pDataStorage;
	Assert( pRenderPipelineContainer->renderPassType == STUDIO_RENDERPASS_TYPE_PRESENT && pDataStoragePresent );

	uint32 viewportIndex = pViewport->GetIndex();
	if ( viewportIndex < (uint32)pDataStoragePresent->viewports.size() )
	{
		// Remove render pipelines, do it on the render thread to make sure that they will be destroyed at the render thread
		viewportRenderPipelines_t& viewportRenderPipelines = pDataStoragePresent->viewports[viewportIndex];
		Assert( viewportRenderPipelines.pViewport == pViewport );
		UNIQUE_RENDER_COMMAND_ONEPARAMETER( CRemoveCachedPipelinesCmd, std::vector<TRefPtr<IStudioAPIRenderPipeline>>, studioAPIRenderPipelines, std::move( viewportRenderPipelines.studioAPIRenderPipelines ),
											{
												studioAPIRenderPipelines.clear();
											} );
	}
}

/*
==================
CStudioRenderPipelineSet::Init
==================
*/
void CStudioRenderPipelineSet::Init()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Initialize containers
	for ( uint32 renderPassType = 0; renderPassType < STUDIO_RENDERPASS_NUM_TYPES; ++renderPassType )
	{
		renderPipelineContainers[renderPassType].Init( (studioRenderPassType_t)renderPassType );
	}

	// Initialize StudioAPI resources
	Studio_BeginUpdateResourceSafe<CStudioRenderPipelineSet>( this );
}

/*
==================
CStudioRenderPipelineSet::Shutdown
==================
*/
void CStudioRenderPipelineSet::Shutdown()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Destroy containers
	for ( uint32 renderPassType = 0; renderPassType < STUDIO_RENDERPASS_NUM_TYPES; ++renderPassType )
	{
		renderPipelineContainers[renderPassType].Destroy();
	}

	// Release StudioAPI resources
	Studio_BeginReleaseResourceSafe<CStudioRenderPipelineSet>( this );
}

/*
==================
CStudioVertexDeclarations::R_BakeRenderPipeline
==================
*/
IStudioAPIRenderPipeline* CStudioRenderPipelineSet::R_BakeRenderPipeline( const studioBakeRenderPipelineParams_t& bakeParams )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	Assert( Studio_IsInRenderThread() );
	Assert( bakeParams.renderPassType < STUDIO_RENDERPASS_NUM_TYPES );
	return renderPipelineContainers[bakeParams.renderPassType].R_BakeRenderPipeline( bakeParams );
}

/*
==================
CStudioVertexDeclarations::GetStudioAPIRenderPipeline
==================
*/
IStudioAPIRenderPipeline* CStudioRenderPipelineSet::GetStudioAPIRenderPipeline( studioRenderPassType_t renderPassType, uint64 pipelineIdx ) const
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	Assert( renderPassType < STUDIO_RENDERPASS_NUM_TYPES );
	return renderPipelineContainers[renderPassType].GetStudioAPIRenderPipeline( pipelineIdx );
}