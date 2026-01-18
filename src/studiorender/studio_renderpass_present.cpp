#include "pch_studiorender.h"
#include "materialsystem/ishader.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "studiorender/studio_renderpasstypes.h"
#include "studiorender/studio_viewport.h"
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
void CStudioRenderPassPresent::R_DrawPass( CStudioViewport* pViewport, CStudioRenderObjectQuad* pQuad )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	IStudioAPIBuffer*				pQuadVertexBuffer	  = pQuad->GetStudioAPIVertexBuffer();
	IStudioAPIBuffer*				pQuadIndexBuffer	  = pQuad->GetStudioAPIIndexBuffer();
	IMaterial*						pQuadMaterial		  = pQuad->GetMaterial();
	ivec2_t							viewportSize		  = pViewport->GetSize();
	IStudioAPISwapChain*			pStudioAPISwapChain	  = pViewport->GetStudioAPISwapChain();
	TRefPtr<IStudioAPICmdContext>	pGraphicsCmdContext	  = g_pStudioAPI->GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_GRAPHICS );
	TRefPtr<IStudioAPICmdListBatch> pGraphicsCmdListBatch = g_pStudioAPI->CreateCmdListBatch( pGraphicsCmdContext );
	TRefPtr<IStudioAPICmdList>		pGraphicsCmdList	  = g_pStudioAPI->CreateCmdList( pGraphicsCmdContext );
	pGraphicsCmdList->BeginRecord();
	pGraphicsCmdList->SetViewport( 0.f, 0.f, (float)viewportSize.x, (float)viewportSize.y, 0.f, 1.f );
	pGraphicsCmdList->SetScissor( 0, 0, viewportSize.x, viewportSize.y );
	{
		studioAPIBarrier_t barriers[] = {
			StudioAPI_MakeBufferBarrier( pQuadVertexBuffer, STUDIOAPI_BUFFER_STATE_VERTEX_BUFFER, STUDIOAPI_QUEUE_TYPE_GRAPHICS ),
			StudioAPI_MakeBufferBarrier( pQuadIndexBuffer, STUDIOAPI_BUFFER_STATE_INDEX_BUFFER, STUDIOAPI_QUEUE_TYPE_GRAPHICS ),
			StudioAPI_MakeTextureBarrier( pStudioAPISwapChain->GetCurrentImage(), STUDIOAPI_TEXTURE_LAYOUT_COLOR_RENDER_TARGET, STUDIOAPI_QUEUE_TYPE_GRAPHICS )
		};
		pGraphicsCmdList->Barrier( barriers, ARRAYSIZE( barriers ) );
	}
	pQuadMaterial->R_Barrier( pGraphicsCmdList );
	pGraphicsCmdList->BeginRenderPass( pViewport->GetStudioAPIRenderPass(), pViewport->GetStudioAPIFrameBuffer() );
	pQuadMaterial->R_PrepareForDraw( pGraphicsCmdList, STUDIO_RENDERPASS_TYPE_PRESENT );
	pGraphicsCmdList->SetVertexBuffer( 0, pQuadVertexBuffer, 0 );
	pGraphicsCmdList->DrawIndexed( pQuadIndexBuffer, 0, 0, 6, 10 );
	pGraphicsCmdList->EndRenderPass();

	{
		studioAPIBarrier_t barriers[] = {
			StudioAPI_MakeTextureBarrier( pStudioAPISwapChain->GetCurrentImage(), STUDIOAPI_TEXTURE_LAYOUT_PRESENT, STUDIOAPI_QUEUE_TYPE_GRAPHICS )
		};
		pGraphicsCmdList->Barrier( barriers, ARRAYSIZE( barriers ) );
	}
	pGraphicsCmdList->EndRecord();

	pGraphicsCmdListBatch->AddCmdList( pGraphicsCmdList );
	pGraphicsCmdListBatch->SyncSwapChain( pStudioAPISwapChain, STUDIOAPI_SYNC_SWAPCHAIN_FLAGS_ACQUIRE_NEXT_IMAGE | STUDIOAPI_SYNC_SWAPCHAIN_FLAGS_PRESENT_TO_IMAGE );
	g_pStudioAPI->SubmitCmdListBatch( pGraphicsCmdListBatch );
}

/*
==================
CStudioRenderPassPresent::CreateStudioAPIRenderPipeline
==================
*/
TRefPtr<IStudioAPIRenderPipeline> CStudioRenderPassPresent::R_CreateStudioAPIRenderPipeline( CStudioViewport* pViewport, IStudioAPIBoundShaderState* pStudioAPIBoundShaderState )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	Assert( Studio_IsInRenderThread() && pViewport && pStudioAPIBoundShaderState );
	Assert( pViewport->GetStudioAPIRenderPass() );

	studioAPIColorBlendAttachmentStateInfo_t studioAPIColorBlendAttachmentState = {};
	studioAPIColorBlendAttachmentState.colorWriteMask							= STUDIOAPI_COLOR_COMPONENT_FLAG_R | STUDIOAPI_COLOR_COMPONENT_FLAG_G | STUDIOAPI_COLOR_COMPONENT_FLAG_B | STUDIOAPI_COLOR_COMPONENT_FLAG_A;
	studioAPIColorBlendAttachmentState.bBlendEnable								= false;

	studioAPIRenderPipelineCreateInfo_t studioAPIRenderPipelineCreateInfo = {};
	studioAPIRenderPipelineCreateInfo.pBoundShaderState					  = pStudioAPIBoundShaderState;
	studioAPIRenderPipelineCreateInfo.inputAssemblyState.topology		  = STUDIOAPI_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	studioAPIRenderPipelineCreateInfo.rasterizerState.fillMode			  = STUDIOAPI_RASTERIZER_FILL_MODE_SOLID;
	studioAPIRenderPipelineCreateInfo.rasterizerState.cullMode			  = STUDIOAPI_RASTERIZER_CULL_MODE_CW;
	studioAPIRenderPipelineCreateInfo.rasterizerState.lineWidth			  = 1.f;
	studioAPIRenderPipelineCreateInfo.rasterizerState.bDepthBiasEnable	  = false;
	studioAPIRenderPipelineCreateInfo.depthState.bTestEnable			  = false;
	studioAPIRenderPipelineCreateInfo.stencilState.bTestEnable			  = false;
	studioAPIRenderPipelineCreateInfo.colorBlendState.attachmentCount	  = 1;
	studioAPIRenderPipelineCreateInfo.colorBlendState.pAttachments		  = &studioAPIColorBlendAttachmentState;
	studioAPIRenderPipelineCreateInfo.colorBlendState.blendConstants	  = vec4_t( 0.f, 0.f, 0.f, 0.f );
	studioAPIRenderPipelineCreateInfo.pRenderPass						  = pViewport->GetStudioAPIRenderPass();
	return g_pStudioAPI->CreateRenderPipeline( studioAPIRenderPipelineCreateInfo );
}
