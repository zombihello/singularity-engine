#pragma once
#include "studiorender/istudio_renderresource.h"
#include "studiorender/studio_renderpass_base.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPIRenderPass;
class IStudioAPIFrameBuffer;

//-----------------------------------------------------------------------------
// Studio scene render pass
// The render pass renders geometry into `__rt_scenecolor_ldr` and `__rt_scenedepth`
//-----------------------------------------------------------------------------
class CStudioRenderPassScene : public CStudioRenderPassBase, public CStudioRenderResource<IStudioRenderResource>
{
public:
	// CStudioRenderPassBase interface
	virtual void Init() override;
	virtual void Shutdown() override;

	virtual void							  R_DrawPass( CStudioViewport* pViewport, studioSceneView_t* pSceneView ) const override;
	virtual void							  R_RebuildFrameBuffers( const vector2i_t& bufferSize ) override;
	virtual CRefPtr<IStudioAPIRenderPipeline> R_CreateStudioAPIRenderPipeline( CStudioViewport* pViewport, IStudioAPIBoundShaderState* pStudioAPIBoundShaderState ) const override;

private:
	// IStudioRenderResource interface
	virtual void InitStudioAPI() override;
	virtual void ReleaseStudioAPI() override;

	CRefPtr<IStudioAPIRenderPass>  pStudioAPIRenderPass;
	CRefPtr<IStudioAPIFrameBuffer> pStudioAPIFrameBuffer;
};
