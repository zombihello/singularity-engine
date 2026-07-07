#pragma once
#include "materialsystem/imaterial.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPIBoundShaderState;
class IStudioAPIRenderPipeline;
class IStudioAPISwapChain;
class CStudioViewport;
struct studioSceneView_t;
struct studioDrawSurface_t;

//-----------------------------------------------------------------------------
// Studio present render pass
//-----------------------------------------------------------------------------
class CStudioRenderPassPresent
{
public:
	void R_DrawPass( CStudioViewport* pViewport, studioSceneView_t* pSceneView );

	static CRefPtr<IStudioAPIRenderPipeline> R_CreateStudioAPIRenderPipeline( CStudioViewport* pViewport, IStudioAPIBoundShaderState* pStudioAPIBoundShaderState );
	static bool								 ShouldDrawSurfaceInPass( studioDrawSurface_t* pDrawSurface );
};

#include "studiorender/studio_renderpass_present.inl"