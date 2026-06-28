#pragma once
#include "materialsystem/imaterial.h"

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
	void									 R_DrawPass( CStudioViewport* pViewport );
	static CRefPtr<IStudioAPIRenderPipeline> R_CreateStudioAPIRenderPipeline( CStudioViewport* pViewport, IStudioAPIBoundShaderState* pStudioAPIBoundShaderState );

private:
};