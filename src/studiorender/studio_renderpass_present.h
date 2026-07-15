#pragma once
#include "studiorender/studio_renderpass_base.h"

//-----------------------------------------------------------------------------
// Studio present render pass
//-----------------------------------------------------------------------------
class CStudioRenderPassPresent : public CStudioRenderPassBase
{
public:
	// CStudioRenderPassBase interface
	virtual void Init() override;
	virtual void Shutdown() override;

	virtual void							  R_DrawPass( CStudioViewport* pViewport, studioSceneView_t* pSceneView ) const override;
	virtual void							  R_RebuildFrameBuffers( const vector2i_t& bufferSize ) override;
	virtual CRefPtr<IStudioAPIRenderPipeline> R_CreateStudioAPIRenderPipeline( CStudioViewport* pViewport, IStudioAPIBoundShaderState* pStudioAPIBoundShaderState ) const override;
};
