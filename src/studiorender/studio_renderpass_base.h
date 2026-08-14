#pragma once
#include "tier1/debugname.h"
#include "studiorender/studio_renderpasstypes.h"
#include "studiorender/studio_renderpipelineset.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioViewport;
struct studioSceneView_t;
class IStudioAPIRenderPass;
class IStudioAPIBoundShaderState;
class IStudioAPIRenderPipeline;

//-----------------------------------------------------------------------------
// Base class for a studio render pass
//-----------------------------------------------------------------------------
class CStudioRenderPassBase
{
public:
	CStudioRenderPassBase( studioRenderPassType_t type, uint32 numColorRenderTargets );
	virtual ~CStudioRenderPassBase() {}

	virtual void Init()		= 0;
	virtual void Shutdown() = 0;

	virtual void					  R_DrawPass( CStudioViewport* pViewport, studioSceneView_t* pSceneView ) const = 0;
	virtual void					  R_RebuildFrameBuffers( const vector2i_t& bufferSize )							= 0;
	CRefPtr<IStudioAPIRenderPipeline> R_CreateStudioAPIRenderPipeline( CStudioViewport* pViewport, IStudioAPIBoundShaderState* pStudioAPIBoundShaderState, const studioRenderState_t& renderState, const char* pDebugName = "" ) const;

protected:
	const studioRenderPassType_t   type;
	const uint32				   numColorRenderTargets;
	CRefPtr<IStudioAPIRenderPass>  pStudioAPIRenderPass;
	CRefPtr<IStudioAPIFrameBuffer> pStudioAPIFrameBuffer;
};
