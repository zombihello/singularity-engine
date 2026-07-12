#pragma once
#include "studiorender/studioapi/istudioapi_shader.h"
#include "studiorender/studio_renderpasstypes.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPIRenderPipeline;
class IStudioAPIVertexDeclaration;

//-----------------------------------------------------------------------------
// A studio render pipeline set interface
//-----------------------------------------------------------------------------
struct studioBakeRenderPipelineParams_t
{
	uint64						  pipelineIdx;
	studioRenderPassType_t		  renderPassType;
	IStudioAPIVertexDeclaration* pVertexDeclaration;
	IStudioAPIShader*			  pStudioAPIShaders[STUDIOAPI_SHADER_NUM_DRAW_TYPES];
};

class IStudioRenderPipelineSet : public IRefCounted
{
public:
	virtual IStudioAPIRenderPipeline* R_BakeRenderPipeline( const studioBakeRenderPipelineParams_t& bakeParams )					  = 0;
	virtual IStudioAPIRenderPipeline* R_GetStudioAPIRenderPipeline( studioRenderPassType_t renderPassType, uint64 pipelineIdx ) const = 0;
};
