#pragma once
#include "studiorender/studio_renderpasstypes.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioViewport;
struct studioSceneView_t;
class IStudioAPIBoundShaderState;
class IStudioAPIRenderPipeline;

//-----------------------------------------------------------------------------
// Base class for a studio render pass
//-----------------------------------------------------------------------------
class CStudioRenderPassBase
{
public:
	virtual ~CStudioRenderPassBase() {}

	virtual void Init()		= 0;
	virtual void Shutdown() = 0;

	virtual void							  R_DrawPass( CStudioViewport* pViewport, studioSceneView_t* pSceneView ) const												  = 0;
	virtual void							  R_RebuildFrameBuffers( const vector2i_t& bufferSize )																		  = 0;
	virtual CRefPtr<IStudioAPIRenderPipeline> R_CreateStudioAPIRenderPipeline( CStudioViewport* pViewport, IStudioAPIBoundShaderState* pStudioAPIBoundShaderState ) const = 0;
};
