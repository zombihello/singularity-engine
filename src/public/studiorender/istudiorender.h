#pragma once
#include "tier1/refcount.h"
#include "tier1/math/math_types.h"
#include "appframework/iappsystem.h"
#include "studiorender/istudio_cmdbuffer.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioViewport;
class IStudioRenderPipelineSet;
class IStudioScene;

//-----------------------------------------------------------------------------
// Studio camera view
//-----------------------------------------------------------------------------
struct studioCameraView_t
{
	vector3_t	 location;
	quaternion_t rotation;
	float		 fieldOfView;
	float		 nearClipPlane;
	float		 farClipPlane;
	float		 aspectRatio;
};

//-----------------------------------------------------------------------------
// Studio render interface
//-----------------------------------------------------------------------------
#define STUDIORENDER_INTERFACE_VERSION "SStudioRender001"
class IStudioRender : public IAppSystem
{
public:
	// Functions to draw scene
	virtual void BeginFrame()																									 = 0;
	virtual void EndFrame()																										 = 0;
	virtual void DrawScene( IStudioViewport* pStudioViewport, IStudioScene* pStudioScene, const studioCameraView_t& cameraView ) = 0;

	// Functions to create a Studio object
	virtual CRefPtr<IStudioViewport>		  CreateViewport() const		  = 0;
	virtual CRefPtr<IStudioRenderPipelineSet> CreateRenderPipelineSet() const = 0;
	virtual CRefPtr<IStudioScene>			  CreateScene() const			  = 0;

	// Returns a command buffer of the render thread. If return NULL it's mean what the studio render not use the render thread
	virtual IStudioCmdBuffer* GetCommandBuffer() const = 0;
	virtual bool			  IsInRenderThread() const = 0;
};
