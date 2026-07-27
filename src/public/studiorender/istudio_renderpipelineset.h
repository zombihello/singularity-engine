#pragma once
#include "studiorender/studioapi/istudioapi_shader.h"
#include "studiorender/studioapi/istudioapi_renderpipeline.h"
#include "studiorender/studio_renderpasstypes.h"

//-----------------------------------------------------------------------------
// Studio render state which is fully owned by a shader
//-----------------------------------------------------------------------------
struct studioRenderState_t
{
	studioAPIInputAssemblyStateInfo_t		 inputAssemblyState;
	studioAPIRasterizerStateInfo_t			 rasterizerState;
	studioAPIDepthStateInfo_t				 depthState;
	studioAPIStencilStateInfo_t				 stencilState;
	studioAPIColorBlendAttachmentStateInfo_t colorBlendAttachment;	// Applied to every color render target of the render pass
	vector4_t								 blendConstants;
};

//-----------------------------------------------------------------------------
// Parameters to bake a render pipeline
//-----------------------------------------------------------------------------
struct studioBakeRenderPipelineParams_t
{
	uint64						 shaderComboIdx;
	uint64						 renderStateIdx;
	studioRenderState_t			 renderState;
	studioRenderPassType_t		 renderPassType;
	IStudioAPIVertexDeclaration* pVertexDeclaration;
	IStudioAPIShader*			 pStudioAPIShaders[STUDIOAPI_SHADER_NUM_DRAW_TYPES];
};

//-----------------------------------------------------------------------------
// Studio render pipeline set interface
//-----------------------------------------------------------------------------
class IStudioRenderPipelineSet : public IRefCounted
{
public:
	virtual IStudioAPIRenderPipeline* R_BakeRenderPipeline( const studioBakeRenderPipelineParams_t& bakeParams )												= 0;
	virtual IStudioAPIRenderPipeline* R_GetStudioAPIRenderPipeline( studioRenderPassType_t renderPassType, uint64 renderStateIdx, uint64 shaderComboIdx ) const = 0;
};
