#include "pch_studiorender.h"
#include "materialsystem/ishader.h"
#include "modelsystem/ivertexfactory.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "studiorender/studio_renderthread.h"
#include "studiorender/studio_resourcebindingslots.h"
#include "studiorender/studioapi/istudioapi_renderpass.h"
#include "studiorender/studioapi/istudioapi_framebuffer.h"
#include "studiorender/studiorender.h"
#include "studiorender/studio_renderpass_scene.h"

/*
==================
CStudioRenderPassScene::Init
==================
*/
void CStudioRenderPassScene::Init()
{
	Studio_BeginUpdateResource( this );
}

/*
==================
CStudioRenderPassScene::Shutdown
==================
*/
void CStudioRenderPassScene::Shutdown()
{
	Studio_BeginReleaseResource( this );
}

/*
==================
CStudioRenderPassScene::InitStudioAPI
==================
*/
void CStudioRenderPassScene::InitStudioAPI()
{
	// Initialize a create info struct to create a StudioAPI render pass
	// NOTE: Initialization happen of frame buffer in `R_RebuildFrameBuffers` when scene render targets are changed
	const CStudioSceneRenderTargets&  sceneRenderTargets			 = g_StudioRender.GetSceneRenderTargets();
	studioAPIRenderPassCreateInfo_t	  studioAPIRenderPassCreateInfo	 = {};
	studioAPIColorRenderTargetInfo_t& studioAPIColorRenderTargetInfo = studioAPIRenderPassCreateInfo.colorRenderTargets[0];
	studioAPIColorRenderTargetInfo.pixelFormat						 = sceneRenderTargets.GetTextureFormat( STUDIO_SCENE_RENDERTARGET_TYPE_SCENECOLOR_LDR );
	studioAPIColorRenderTargetInfo.bSwapChain						 = false;
	studioAPIColorRenderTargetInfo.loadOp							 = STUDIOAPI_RENDER_TARGET_LOAD_OP_CLEAR;
	studioAPIColorRenderTargetInfo.storeOp							 = STUDIOAPI_RENDER_TARGET_STORE_OP_STORE;

	studioAPIDepthStencilRenderTargetInfo_t& studioAPIDepthStencilRenderTargetInfo = studioAPIRenderPassCreateInfo.depthStencilRenderTarget;
	studioAPIDepthStencilRenderTargetInfo.pixelFormat							   = sceneRenderTargets.GetTextureFormat( STUDIO_SCENE_RENDERTARGET_TYPE_SCENEDEPTH );
	studioAPIDepthStencilRenderTargetInfo.accessFlags							   = STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_STENCIL_WRITE;
	studioAPIDepthStencilRenderTargetInfo.depthLoadOp							   = STUDIOAPI_RENDER_TARGET_LOAD_OP_CLEAR;
	studioAPIDepthStencilRenderTargetInfo.depthStoreOp							   = STUDIOAPI_RENDER_TARGET_STORE_OP_STORE;
	studioAPIDepthStencilRenderTargetInfo.stencilLoadOp							   = STUDIOAPI_RENDER_TARGET_LOAD_OP_DONT_CARE;
	studioAPIDepthStencilRenderTargetInfo.stencilStoreOp						   = STUDIOAPI_RENDER_TARGET_STORE_OP_DONT_CARE;

	// Create a StudioAPI render pass
	pStudioAPIRenderPass = g_pStudioAPI->CreateRenderPass( studioAPIRenderPassCreateInfo, "Scene RenderPass" );
}

/*
==================
CStudioRenderPassScene::ReleaseStudioAPI
==================
*/
void CStudioRenderPassScene::ReleaseStudioAPI()
{
	pStudioAPIFrameBuffer = NULL;
	pStudioAPIRenderPass  = NULL;
}

/*
==================
CStudioRenderPassScene::R_DrawPass
==================
*/
void CStudioRenderPassScene::R_DrawPass( CStudioViewport* pViewport, studioSceneView_t* pSceneView ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pStudioAPIRenderPass && pStudioAPIFrameBuffer );
	const CStudioSceneRenderTargets& sceneRenderTargets	   = g_StudioRender.GetSceneRenderTargets();
	CRefPtr<IStudioAPIBuffer>		 pGlobalConstantBuffer = g_StudioRender.GetStudioAPIGlobalConstantBuffer();
	CRefPtr<IStudioAPICmdContext>	 pGraphicsCmdContext   = g_pStudioAPI->GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_GRAPHICS );
	CRefPtr<IStudioAPICmdListBatch>	 pGraphicsCmdListBatch = g_pStudioAPI->CreateCmdListBatch( pGraphicsCmdContext );
	CRefPtr<IStudioAPICmdList>		 pGraphicsCmdList	   = g_pStudioAPI->CreateCmdList( pGraphicsCmdContext );

	// Initialize viewport and scissor
	pGraphicsCmdList->BeginRecord();
	pGraphicsCmdList->SetViewport( 0.f, 0.f, (float)pSceneView->globalShaderParams.screenAndBufferSize.x, (float)pSceneView->globalShaderParams.screenAndBufferSize.y, 0.f, 1.f );
	pGraphicsCmdList->SetScissor( 0, 0, pSceneView->globalShaderParams.screenAndBufferSize.x, pSceneView->globalShaderParams.screenAndBufferSize.y );

	// Place barriers into the command list
	{
		studioAPIBarrier_t barriers[] = {
			StudioAPI_MakeTextureBarrier( sceneRenderTargets.GetTextureResource( STUDIO_SCENE_RENDERTARGET_TYPE_SCENECOLOR_LDR )->GetStudioAPITexture(), STUDIOAPI_TEXTURE_LAYOUT_COLOR_RENDER_TARGET, STUDIOAPI_QUEUE_TYPE_GRAPHICS ),
			StudioAPI_MakeTextureBarrier( sceneRenderTargets.GetTextureResource( STUDIO_SCENE_RENDERTARGET_TYPE_SCENEDEPTH )->GetStudioAPITexture(), STUDIOAPI_TEXTURE_LAYOUT_DEPTH_RENDER_TARGET, STUDIOAPI_QUEUE_TYPE_GRAPHICS ),
			StudioAPI_MakeBufferBarrier( pGlobalConstantBuffer, STUDIOAPI_BUFFER_STATE_CONSTANT_BUFFER, STUDIOAPI_QUEUE_TYPE_GRAPHICS )
		};
		pGraphicsCmdList->Barrier( barriers, ARRAYSIZE( barriers ) );
	}

	const studioRenderPass_t& renderPass = pSceneView->renderPasses[STUDIO_RENDERPASS_TYPE_SCENE];
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
	pGraphicsCmdList->BeginRenderPass( pStudioAPIRenderPass, pStudioAPIFrameBuffer );
	for ( uint32 index = 0, count = (uint32)renderPass.drawSurfaceIds.size(); index < count; ++index )
	{
		studioDrawSurface_t*	  pDrawSurface			   = pSceneView->drawSurfaces[renderPass.drawSurfaceIds[index]];
		IModelResource*			  pModel				   = pSceneView->resources[pDrawSurface->modelId]->pModel;
		IMaterialResource*		  pMaterial				   = pSceneView->resources[pDrawSurface->materialId]->pMaterial;
		IShader*				  pShader				   = pMaterial->GetShader();
		IShaderContextData*		  pContextData			   = pMaterial->GetContextData();
		IVertexFactory*			  pVertexFactory		   = pModel->GetVertexFactory();
		IStudioAPIRenderPipeline* pStudioAPIRenderPipeline = pShader->R_ResolveRenderPipeline( pContextData, pVertexFactory, STUDIO_RENDERPASS_TYPE_SCENE );

		pGraphicsCmdList->SetRenderPipeline( pStudioAPIRenderPipeline );
		pGraphicsCmdList->SetConstantBuffer( 0, STUDIO_RESOURCE_BINDING_SLOT_GLOBAL_CB, pGlobalConstantBuffer );
		pVertexFactory->R_Bind( pGraphicsCmdList );
		pShader->R_Bind( pGraphicsCmdList, pContextData );
		pGraphicsCmdList->DrawIndexed( pDrawSurface->baseVertexIndex, pDrawSurface->baseIndex, pDrawSurface->numIndices );
	}
	pGraphicsCmdList->EndRenderPass();
	pGraphicsCmdList->EndRecord();

	// Submit the command list
	pGraphicsCmdListBatch->AddCmdList( pGraphicsCmdList );
	g_pStudioAPI->SubmitCmdListBatch( pGraphicsCmdListBatch );
}

/*
==================
CStudioRenderPassScene::R_RebuildFrameBuffers
==================
*/
void CStudioRenderPassScene::R_RebuildFrameBuffers( const vector2i_t& bufferSize )
{
	// Rebuild the StudioAPI frame buffers
	const CStudioSceneRenderTargets& sceneRenderTargets				 = g_StudioRender.GetSceneRenderTargets();
	studioAPIFrameBufferCreateInfo_t studioAPIFrameBufferCreateInfo	 = {};
	studioAPIFrameBufferCreateInfo.pRenderPass						 = pStudioAPIRenderPass;
	studioAPIFrameBufferCreateInfo.colorRenderTargets[0].pTexture	 = sceneRenderTargets.GetTextureResource( STUDIO_SCENE_RENDERTARGET_TYPE_SCENECOLOR_LDR )->GetStudioAPITexture();
	studioAPIFrameBufferCreateInfo.depthStencilRenderTarget.pTexture = sceneRenderTargets.GetTextureResource( STUDIO_SCENE_RENDERTARGET_TYPE_SCENEDEPTH )->GetStudioAPITexture();
	studioAPIFrameBufferCreateInfo.size								 = bufferSize;
	studioAPIFrameBufferCreateInfo.bClearColor						 = true;
	studioAPIFrameBufferCreateInfo.clearColor						 = CLinearColor::Make( 0.f, 0.f, 0.f );
	studioAPIFrameBufferCreateInfo.bDepthStencilClearValue			 = true;
	studioAPIFrameBufferCreateInfo.depthClearValue					 = 1.f;
	studioAPIFrameBufferCreateInfo.stencilClearValue				 = 0;
	pStudioAPIFrameBuffer											 = g_pStudioAPI->CreateFrameBuffer( studioAPIFrameBufferCreateInfo, "Scene FrameBuffer" );
}

/*
==================
CStudioRenderPassScene::CreateStudioAPIRenderPipeline
==================
*/
CRefPtr<IStudioAPIRenderPipeline> CStudioRenderPassScene::R_CreateStudioAPIRenderPipeline( CStudioViewport* pViewport, IStudioAPIBoundShaderState* pStudioAPIBoundShaderState ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( Studio_IsInRenderThread() && pStudioAPIRenderPass && pStudioAPIBoundShaderState );

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
	studioAPIRenderPipelineCreateInfo.depthState.bTestEnable			  = true;
	studioAPIRenderPipelineCreateInfo.depthState.bWriteEnable			  = true;
	studioAPIRenderPipelineCreateInfo.depthState.compareOp				  = STUDIOAPI_COMPARE_OP_LESS;
	studioAPIRenderPipelineCreateInfo.stencilState.bTestEnable			  = false;
	studioAPIRenderPipelineCreateInfo.colorBlendState.attachmentCount	  = 1;
	studioAPIRenderPipelineCreateInfo.colorBlendState.pAttachments		  = &studioAPIColorBlendAttachmentState;
	studioAPIRenderPipelineCreateInfo.colorBlendState.blendConstants	  = vector4_t( 0.f, 0.f, 0.f, 0.f );
	studioAPIRenderPipelineCreateInfo.pRenderPass						  = pStudioAPIRenderPass;
	return g_pStudioAPI->CreateRenderPipeline( studioAPIRenderPipelineCreateInfo );
}
