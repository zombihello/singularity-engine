#pragma once
#include "materialsystem/imaterial.h"
#include "studiorender/studio_renderpass_base.h"

//-----------------------------------------------------------------------------
// Studio present render pass
// Copy final frame from `__rt_scenecolor_ldr` into a viewport
//-----------------------------------------------------------------------------
class CStudioRenderPassPresent : public CStudioRenderPassBase
{
public:
	// CStudioRenderPassBase interface
	virtual void Init() override;
	virtual void Shutdown() override;

	virtual void R_DrawPass( CStudioViewport* pViewport, studioSceneView_t* pSceneView ) const override;
	virtual void R_RebuildFrameBuffers( const vector2i_t& bufferSize ) override;

	CStudioRenderPassPresent();

private:
	CResourcePtr<IMaterial>	   pPresentMaterial;
	CRefPtr<IMaterialResource> pPresentMaterialResource;
};
