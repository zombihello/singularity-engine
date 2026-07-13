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
CStudioRenderPassPresent::R_DrawPass
==================
*/
void CStudioRenderPassPresent::R_DrawPass( CStudioViewport* pViewport, studioSceneView_t* pSceneView )
{
	// Do nothing if the swap chain hasn't an acquired image
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	IStudioAPISwapChain* pStudioAPISwapChain = pViewport->GetStudioAPISwapChain();
	if ( !pStudioAPISwapChain->IsImageAcquired() )
	{
		return;
	}

	vector2i_t						viewportSize		  = pViewport->GetSize();
	CRefPtr<IStudioAPIBuffer>		pGlobalConstantBuffer = g_StudioRender.GetStudioAPIGlobalConstantBuffer();
	CRefPtr<IStudioAPICmdContext>	pGraphicsCmdContext	  = g_pStudioAPI->GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_GRAPHICS );
	CRefPtr<IStudioAPICmdListBatch> pGraphicsCmdListBatch = g_pStudioAPI->CreateCmdListBatch( pGraphicsCmdContext );
	CRefPtr<IStudioAPICmdList>		pGraphicsCmdList	  = g_pStudioAPI->CreateCmdList( pGraphicsCmdContext );

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

	const studioRenderPass_t& renderPass = pSceneView->renderPasses[STUDIO_RENDERPASS_TYPE_PRESENT];
	for ( auto it = renderPass.resourceIds.begin(), itEnd = renderPass.resourceIds.end(); it != itEnd; ++it )
	{
		studioResource_t* pResource = pSceneView->resources[*it];
		switch ( pResource->type )
		{
		case STUDIO_RESOURCE_TYPE_MODEL:
		{
			IModelResource* pModel		   = pResource->pModel;
			IVertexFactory* pVertexFactory = pModel->GetVertexFactory();
			pVertexFactory->R_Barrier( pGraphicsCmdList );
			break;
		}

		case STUDIO_RESOURCE_TYPE_MATERIAL:
		{
			IMaterialResource*	pMaterial	 = pResource->pMaterial;
			IShader*			pShader		 = pMaterial->GetShader();
			IShaderContextData* pContextData = pMaterial->GetContextData();
			pShader->R_Barrier( pGraphicsCmdList, pContextData );
			break;
		}

		default:
			AssertMsg( false, "Unknown studio resource type 0x%X", pResource->type );
			break;
		}
	}

	// Draw all surfaces for the pass
	pGraphicsCmdList->BeginRenderPass( pViewport->GetStudioAPIRenderPass(), pViewport->GetStudioAPIFrameBuffer() );
	for ( uint32 index = 0, count = (uint32)renderPass.drawSurfaceIds.size(); index < count; ++index )
	{
		studioDrawSurface_t* pDrawSurface	= pSceneView->drawSurfaces[renderPass.drawSurfaceIds[index]];
		IModelResource*		 pModel			= pSceneView->resources[pDrawSurface->modelId]->pModel;
		IMaterialResource*	 pMaterial		= pSceneView->resources[pDrawSurface->materialId]->pMaterial;
		IShader*			 pShader		= pMaterial->GetShader();
		IShaderContextData*	 pContextData	= pMaterial->GetContextData();
		IVertexFactory*		 pVertexFactory = pModel->GetVertexFactory();

		pVertexFactory->R_PrepareForDraw( pGraphicsCmdList, STUDIO_RENDERPASS_TYPE_PRESENT );
		pShader->R_PrepareForDraw( pGraphicsCmdList, pContextData, pVertexFactory, STUDIO_RENDERPASS_TYPE_PRESENT );
		pGraphicsCmdList->SetConstantBuffer( 0, STUDIO_RESOURCE_BINDING_SLOT_GLOBAL_CB, pGlobalConstantBuffer );
		pGraphicsCmdList->DrawIndexed( pDrawSurface->baseVertexIndex, pDrawSurface->baseIndex, pDrawSurface->numIndices );
	}
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
