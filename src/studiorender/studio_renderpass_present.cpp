#include "pch_studiorender.h"
#include "materialsystem/ishader.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "studiorender/studio_renderpasstypes.h"
#include "studiorender/studio_viewport.h"
#include "studiorender/studio_renderpass_present.h"
#include "studiorender/studio_sceneview.h"

/*
==================
CStudioRenderPassPresent::R_DrawPass
==================
*/
void CStudioRenderPassPresent::R_DrawPass( CStudioViewport* pViewport, studioSceneView_t* pSceneView )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	vector2i_t						viewportSize		  = pViewport->GetSize();
	IStudioAPISwapChain*			pStudioAPISwapChain	  = pViewport->GetStudioAPISwapChain();
	CRefPtr<IStudioAPICmdContext>	pGraphicsCmdContext	  = g_pStudioAPI->GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_GRAPHICS );
	CRefPtr<IStudioAPICmdListBatch> pGraphicsCmdListBatch = g_pStudioAPI->CreateCmdListBatch( pGraphicsCmdContext );
	CRefPtr<IStudioAPICmdList>		pGraphicsCmdList	  = g_pStudioAPI->CreateCmdList( pGraphicsCmdContext );

	pGraphicsCmdList->BeginRecord();
	pGraphicsCmdList->SetViewport( 0.f, 0.f, (float)viewportSize.x, (float)viewportSize.y, 0.f, 1.f );
	pGraphicsCmdList->SetScissor( 0, 0, viewportSize.x, viewportSize.y );
	pGraphicsCmdList->SetScissor( 0, 0, viewportSize.x, viewportSize.y );
	{
		studioAPIBarrier_t barriers[] = {
			StudioAPI_MakeTextureBarrier( pStudioAPISwapChain->GetCurrentImage(), STUDIOAPI_TEXTURE_LAYOUT_COLOR_RENDER_TARGET, STUDIOAPI_QUEUE_TYPE_GRAPHICS )
		};
		pGraphicsCmdList->Barrier( barriers, ARRAYSIZE( barriers ) );
	}

	const studioRenderPass_t& renderPass = pSceneView->renderPasses[STUDIO_RENDERPASS_TYPE_PRESENT];
	for ( uint32 index = 0, count = renderPass.numDrawSurfaces; index < count; ++index )
	{
		studioDrawSurface_t* pDrawSurface			= pSceneView->pDrawSurfaces[renderPass.pDrawSurfaceIds[index]];
		IShader*			 pShader				= pDrawSurface->pMaterial->GetShader();
		IShaderContextData*	 pContextData			= pDrawSurface->pMaterial->GetContextData();
		IStudioAPIBuffer*	 pStudioAPIVertexBuffer = pDrawSurface->pModel->GetStudioAPIVertexBuffer();
		IStudioAPIBuffer*	 pStudioAPIIndexBuffer	= pDrawSurface->pModel->GetStudioAPIIndexBuffer();

		pShader->R_Barrier( pGraphicsCmdList, pContextData );
		{
			studioAPIBarrier_t barriers[] = {
				StudioAPI_MakeBufferBarrier( pStudioAPIVertexBuffer, STUDIOAPI_BUFFER_STATE_VERTEX_BUFFER, STUDIOAPI_QUEUE_TYPE_GRAPHICS ),
				StudioAPI_MakeBufferBarrier( pStudioAPIIndexBuffer, STUDIOAPI_BUFFER_STATE_INDEX_BUFFER, STUDIOAPI_QUEUE_TYPE_GRAPHICS )
			};
			pGraphicsCmdList->Barrier( barriers, ARRAYSIZE( barriers ) );
		}
		pGraphicsCmdList->BeginRenderPass( pViewport->GetStudioAPIRenderPass(), pViewport->GetStudioAPIFrameBuffer() );
		pShader->R_PrepareForDraw( pGraphicsCmdList, pContextData, STUDIO_RENDERPASS_TYPE_PRESENT );
		pGraphicsCmdList->SetVertexBuffer( 0, pStudioAPIVertexBuffer, 0 );
		pGraphicsCmdList->SetIndexBuffer( pStudioAPIIndexBuffer, 0 );
		pGraphicsCmdList->DrawIndexed( pDrawSurface->baseVertexIndex, pDrawSurface->baseIndex, pDrawSurface->numIndices );
		pGraphicsCmdList->EndRenderPass();
	}

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
CRefPtr<IStudioAPIRenderPipeline> CStudioRenderPassPresent::R_CreateStudioAPIRenderPipeline( CStudioViewport* pViewport, IStudioAPIBoundShaderState* pStudioAPIBoundShaderState )
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
	studioAPIRenderPipelineCreateInfo.rasterizerState.cullMode			  = STUDIOAPI_RASTERIZER_CULL_MODE_CW;
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
