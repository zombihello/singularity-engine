#pragma once
#include "materialsystem/imaterial.h"
#include "studiorender/studio_renderobject_quad.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPIBoundShaderState;
class IStudioAPIRenderPipeline;
class IStudioAPISwapChain;
class CStudioViewport;

//-----------------------------------------------------------------------------
// Studio present render pass
//-----------------------------------------------------------------------------
class CStudioRenderPassPresent
{
public:
	void Init();
	void Shutdown();

	void									 R_DrawPass( CStudioViewport* pViewport, CStudioRenderObjectQuad* pQuad );
	static CRefPtr<IStudioAPIRenderPipeline> R_CreateStudioAPIRenderPipeline( CStudioViewport* pViewport, IStudioAPIBoundShaderState* pStudioAPIBoundShaderState );

private:
};