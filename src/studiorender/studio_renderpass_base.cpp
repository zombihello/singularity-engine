#include "pch_studiorender.h"
#include "studiorender/studioapi/istudioapi_renderpass.h"
#include "studiorender/studio_renderthread.h"
#include "studiorender/studio_renderpass_base.h"

/*
==================
CStudioRenderPassBase::CStudioRenderPassBase
==================
*/
CStudioRenderPassBase::CStudioRenderPassBase( studioRenderPassType_t type, uint32 numColorRenderTargets )
	: type( type )
	, numColorRenderTargets( numColorRenderTargets )
{
	Assert( type < STUDIO_RENDERPASS_NUM_TYPES );
	Assert( numColorRenderTargets <= STUDIOAPI_MAX_SIMULTANEOUS_RENDER_TARGETS );
}

/*
==================
CStudioRenderPassBase::R_CreateStudioAPIRenderPipeline
==================
*/
CRefPtr<IStudioAPIRenderPipeline> CStudioRenderPassBase::R_CreateStudioAPIRenderPipeline( CStudioViewport* pViewport, IStudioAPIBoundShaderState* pStudioAPIBoundShaderState, const studioRenderState_t& renderState ) const
{
	// Get the viewport's render pass if the type is `STUDIO_RENDERPASS_TYPE_PRESENT`, otherwise use `pStudioAPIRenderPass`
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( Studio_IsInRenderThread() && pStudioAPIBoundShaderState );
	IStudioAPIRenderPass* pRenderPass = NULL;
	switch ( type )
	{
	case STUDIO_RENDERPASS_TYPE_SCENE: pRenderPass = pStudioAPIRenderPass; break;
	case STUDIO_RENDERPASS_TYPE_PRESENT:
		Assert( pViewport );
		pRenderPass = pViewport->GetStudioAPIRenderPass();
		break;

	default:
		AssertMsg( false, "Unknown render pass type 0x%X", type );
		break;
	}
	Assert( pRenderPass );

	// The shader's render state describes a single color blend attachment - replicate it across
	// every color render target the render pass actually has
	studioAPIColorBlendAttachmentStateInfo_t studioAPIColorBlendAttachments[STUDIOAPI_MAX_SIMULTANEOUS_RENDER_TARGETS];
	for ( uint32 index = 0; index < numColorRenderTargets; ++index )
	{
		studioAPIColorBlendAttachments[index] = renderState.colorBlendAttachment;
	}

	// Create a render pipeline for the render pass
	studioAPIRenderPipelineCreateInfo_t studioAPIRenderPipelineCreateInfo = {};
	studioAPIRenderPipelineCreateInfo.pBoundShaderState					  = pStudioAPIBoundShaderState;
	studioAPIRenderPipelineCreateInfo.inputAssemblyState				  = renderState.inputAssemblyState;
	studioAPIRenderPipelineCreateInfo.rasterizerState					  = renderState.rasterizerState;
	studioAPIRenderPipelineCreateInfo.depthState						  = renderState.depthState;
	studioAPIRenderPipelineCreateInfo.stencilState						  = renderState.stencilState;
	studioAPIRenderPipelineCreateInfo.colorBlendState.attachmentCount	  = numColorRenderTargets;
	studioAPIRenderPipelineCreateInfo.colorBlendState.pAttachments		  = studioAPIColorBlendAttachments;
	studioAPIRenderPipelineCreateInfo.colorBlendState.blendConstants	  = renderState.blendConstants;
	studioAPIRenderPipelineCreateInfo.pRenderPass						  = pRenderPass;
	return g_pStudioAPI->CreateRenderPipeline( studioAPIRenderPipelineCreateInfo );
}
