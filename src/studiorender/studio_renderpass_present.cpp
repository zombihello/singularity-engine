#include "pch_studiorender.h"
#include "materialsystem/ishader.h"
#include "modelsystem/ivertexfactory.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "studiorender/studio_resourcebindingslots.h"
#include "studiorender/studio_renderpasstypes.h"
#include "studiorender/studio_viewport.h"
#include "studiorender/studiorender.h"
#include "studiorender/studio_renderpass_present.h"

/*
==================
CStudioRenderPassPresent::Init
==================
*/
void CStudioRenderPassPresent::Init()
{
}

/*
==================
CStudioRenderPassPresent::Shutdown
==================
*/
void CStudioRenderPassPresent::Shutdown()
{
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
	IStudioAPISwapChain*			pStudioAPISwapChain	  = pViewport->GetStudioAPISwapChain();
	CRefPtr<IStudioAPICmdContext>	pGraphicsCmdContext	  = g_pStudioAPI->GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_GRAPHICS );
	CRefPtr<IStudioAPICmdListBatch> pGraphicsCmdListBatch = g_pStudioAPI->CreateCmdListBatch( pGraphicsCmdContext );
	CRefPtr<IStudioAPICmdList>		pGraphicsCmdList	  = g_pStudioAPI->CreateCmdList( pGraphicsCmdContext );

	// Initialize viewport and scissor
	pGraphicsCmdList->BeginRecord();
	pGraphicsCmdList->SetViewport( 0.f, 0.f, (float)viewportSize.x, (float)viewportSize.y, 0.f, 1.f );
	pGraphicsCmdList->SetScissor( 0, 0, viewportSize.x, viewportSize.y );

	// Place barriers into the command list
	{
		studioAPIBarrier_t barrier = StudioAPI_MakeTextureBarrier( pStudioAPISwapChain->GetCurrentImage(), STUDIOAPI_TEXTURE_LAYOUT_COLOR_RENDER_TARGET, STUDIOAPI_QUEUE_TYPE_GRAPHICS );
		pGraphicsCmdList->Barrier( &barrier, 1 );
	}

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

/*
==================
CStudioRenderPassPresent::CreateStudioAPIRenderPipeline
==================
*/
CRefPtr<IStudioAPIRenderPipeline> CStudioRenderPassPresent::R_CreateStudioAPIRenderPipeline( CStudioViewport* pViewport, IStudioAPIBoundShaderState* pStudioAPIBoundShaderState ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( Studio_IsInRenderThread() && pViewport && pStudioAPIBoundShaderState );
	Assert( pViewport->GetStudioAPIRenderPass() );

	studioAPIColorBlendAttachmentStateInfo_t studioAPIColorBlendAttachmentState = {};
	studioAPIColorBlendAttachmentState.colorWriteMask							= STUDIOAPI_COLOR_COMPONENT_FLAG_R | STUDIOAPI_COLOR_COMPONENT_FLAG_G | STUDIOAPI_COLOR_COMPONENT_FLAG_B | STUDIOAPI_COLOR_COMPONENT_FLAG_A;
	studioAPIColorBlendAttachmentState.bBlendEnable								= false;

	studioAPIRenderPipelineCreateInfo_t studioAPIRenderPipelineCreateInfo = {};
	studioAPIRenderPipelineCreateInfo.pBoundShaderState					  = pStudioAPIBoundShaderState;
	studioAPIRenderPipelineCreateInfo.inputAssemblyState.topology		  = STUDIOAPI_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	studioAPIRenderPipelineCreateInfo.rasterizerState.fillMode			  = STUDIOAPI_RASTERIZER_FILL_MODE_SOLID;
	studioAPIRenderPipelineCreateInfo.rasterizerState.cullMode			  = STUDIOAPI_RASTERIZER_CULL_MODE_NONE;
	studioAPIRenderPipelineCreateInfo.rasterizerState.lineWidth			  = 1.f;
	studioAPIRenderPipelineCreateInfo.rasterizerState.bDepthBiasEnable	  = false;
	studioAPIRenderPipelineCreateInfo.depthState.bTestEnable			  = false;
	studioAPIRenderPipelineCreateInfo.stencilState.bTestEnable			  = false;
	studioAPIRenderPipelineCreateInfo.colorBlendState.attachmentCount	  = 1;
	studioAPIRenderPipelineCreateInfo.colorBlendState.pAttachments		  = &studioAPIColorBlendAttachmentState;
	studioAPIRenderPipelineCreateInfo.colorBlendState.blendConstants	  = vector4_t( 0.f, 0.f, 0.f, 0.f );
	studioAPIRenderPipelineCreateInfo.pRenderPass						  = pViewport->GetStudioAPIRenderPass();
	return g_pStudioAPI->CreateRenderPipeline( studioAPIRenderPipelineCreateInfo );
}
