#include "pch_studiorender.h"
#include "materialsystem/ishader.h"
#include "materialsystem/imaterialvar.h"
#include "materialsystem/itexture.h"
#include "resourcesystem/iresourcesystem.h"
#include "modelsystem/imodelsystem.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "studiorender/studio_resourcebindingslots.h"
#include "studiorender/studio_renderpasstypes.h"
#include "studiorender/studio_viewport.h"
#include "studiorender/studio_renderutils.h"
#include "studiorender/studiorender.h"
#include "studiorender/studio_renderpass_present.h"

/*
==================
CStudioRenderPassPresent::CStudioRenderPassPresent
==================
*/
CStudioRenderPassPresent::CStudioRenderPassPresent()
	: CStudioRenderPassBase( STUDIO_RENDERPASS_TYPE_PRESENT, 1 )
{
}

/*
==================
CStudioRenderPassPresent::Init
==================
*/
void CStudioRenderPassPresent::Init()
{
	// Create a material for present the final render target
	PROFILER_SCOPE_FUNC();
	IResourceTypeMgr*	  pMaterialsMgr			= g_pResourceSystem->GetResourceManagerForType<IMaterial>();
	IResourceTypeMgr*	  pTexturesMgr			= g_pResourceSystem->GetResourceManagerForType<ITexture>();
	materialVarInfo_t	  presentBaseTextureVar = MaterialVar_MakeTexture( "basetexture", pTexturesMgr->FindResource( "__rt_scenecolor_ldr" ) );
	materialInitialData_t presentMatInitialData = {};
	presentMatInitialData.pShaderName			= "screenspace";
	presentMatInitialData.numVars				= 1;
	presentMatInitialData.pVars					= &presentBaseTextureVar;

	pMaterial = pMaterialsMgr->CreateResource( "__scenecolor_present" );
	pMaterial->Init( presentMatInitialData );
	pMaterialResource = pMaterial->GetStudioResource();

	// Create a vertex factory for draw a quad
	pVertexFactory = g_pModelSystem->CreateVertexFactory<IVertexFactorySimple>();
	pVertexFactory->Init();
}

/*
==================
CStudioRenderPassPresent::Shutdown
==================
*/
void CStudioRenderPassPresent::Shutdown()
{
	PROFILER_SCOPE_FUNC();
	pVertexFactory->Shutdown();
	pVertexFactory	  = NULL;
	pMaterialResource = NULL;
	pMaterial		  = NULL;
}

/*
==================
CStudioRenderPassPresent::R_DrawPass
==================
*/
void CStudioRenderPassPresent::R_DrawPass( CStudioViewport* pViewport, studioSceneView_t* pSceneView ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	vector2i_t						viewportSize		  = pViewport->GetSize();
	CRefPtr<IStudioAPIBuffer>		pGlobalConstantBuffer = g_StudioRender.GetStudioAPIGlobalConstantBuffer();
	IStudioAPISwapChain*			pStudioAPISwapChain	  = pViewport->GetStudioAPISwapChain();
	CRefPtr<IStudioAPICmdContext>	pGraphicsCmdContext	  = g_pStudioAPI->GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_GRAPHICS );
	CRefPtr<IStudioAPICmdListBatch> pGraphicsCmdListBatch = g_pStudioAPI->CreateCmdListBatch( pGraphicsCmdContext );
	CRefPtr<IStudioAPICmdList>		pGraphicsCmdList	  = g_pStudioAPI->CreateCmdList( pGraphicsCmdContext );

	IShader*		   pShader	  = pMaterialResource->GetShader();
	shaderDrawParams_t drawParams = { pMaterialResource->GetPerMaterialContextData(), pShader->GetDefaultPerDrawVars(), pVertexFactory };

	// Initialize viewport and scissor
	pGraphicsCmdList->BeginRecord();
	pGraphicsCmdList->SetViewport( 0.f, 0.f, (float)viewportSize.x, (float)viewportSize.y, 0.f, 1.f );
	pGraphicsCmdList->SetScissor( 0, 0, viewportSize.x, viewportSize.y );

	// Place barriers into the command list
	{
		studioAPIBarrier_t barriers[] = {
			StudioAPI_MakeTextureBarrier( pStudioAPISwapChain->GetCurrentImage(), STUDIOAPI_TEXTURE_LAYOUT_COLOR_RENDER_TARGET, STUDIOAPI_QUEUE_TYPE_GRAPHICS ),
			StudioAPI_MakeBufferBarrier( pGlobalConstantBuffer, STUDIOAPI_BUFFER_STATE_CONSTANT_BUFFER, STUDIOAPI_QUEUE_TYPE_GRAPHICS )
		};
		pGraphicsCmdList->Barrier( barriers, ARRAYSIZE( barriers ) );
	}
	drawParams.pPerMaterialContextData->R_Barrier( pGraphicsCmdList );

	// Copy `__rt_scenecolor_ldr` into a swapchain image
	studioDenormalizedQuadParams_t denormalizedQuadParams = { pVertexFactory, vector2_t( 0.f, 0.f ), viewportSize, vector2_t( 0.f, 0.f ), viewportSize, viewportSize, g_StudioRender.GetSceneRenderTargets().GetBufferSize(), 1.f, CColor::Make( 255, 255, 255 ) };
	pGraphicsCmdList->BeginRenderPass( pViewport->GetStudioAPIRenderPass(), pViewport->GetStudioAPIFrameBuffer() );
	pGraphicsCmdList->SetRenderPipeline( pShader->R_ResolveRenderPipeline( drawParams, STUDIO_RENDERPASS_TYPE_PRESENT ) );
	pGraphicsCmdList->SetConstantBuffer( 0, STUDIO_RESOURCE_BINDING_SLOT_GLOBAL_CB, pGlobalConstantBuffer );
	pShader->R_Bind( pGraphicsCmdList, drawParams );
	R_DrawDenormalizedQuad( pGraphicsCmdList, denormalizedQuadParams );
	pGraphicsCmdList->EndRenderPass();

	// Place barriers for the swapchain image
	{
		studioAPIBarrier_t barrier = StudioAPI_MakeTextureBarrier( pStudioAPISwapChain->GetCurrentImage(), STUDIOAPI_TEXTURE_LAYOUT_PRESENT, STUDIOAPI_QUEUE_TYPE_GRAPHICS );
		pGraphicsCmdList->Barrier( &barrier, 1 );
	}
	pGraphicsCmdList->EndRecord();

	// Submit the command list
	pGraphicsCmdListBatch->AddCmdList( pGraphicsCmdList );
	pGraphicsCmdListBatch->SyncSwapChain( pStudioAPISwapChain, STUDIOAPI_SYNC_SWAPCHAIN_FLAGS_ACQUIRE_NEXT_IMAGE | STUDIOAPI_SYNC_SWAPCHAIN_FLAGS_PRESENT_TO_IMAGE );
	g_pStudioAPI->SubmitCmdListBatch( pGraphicsCmdListBatch );
}

/*
==================
CStudioRenderPassPresent::R_RebuildFrameBuffers
==================
*/
void CStudioRenderPassPresent::R_RebuildFrameBuffers( const vector2i_t& bufferSize )
{
	// The present pass draws into the viewport's own swap chain frame buffer, which the viewport
	// already manages/resizes itself - nothing to do here
}
