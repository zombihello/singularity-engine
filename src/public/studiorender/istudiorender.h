#pragma once
#include "tier1/refcount.h"
#include "appframework/iappsystem.h"
#include "studiorender/istudio_cmdbuffer.h"
#include "studiorender/istudio_viewport.h"
#include "studiorender/istudio_renderpipelineset.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IResource;
class IStudioAPIBuffer;

//-----------------------------------------------------------------------------
// Studio camera view
//-----------------------------------------------------------------------------
struct studioCameraView_t
{
};

//-----------------------------------------------------------------------------
// Studio render interface
//-----------------------------------------------------------------------------
#define STUDIORENDER_INTERFACE_VERSION "SStudioRender001"
class IStudioRender : public IAppSystem
{
public:
	// Functions to draw scene
	virtual void BeginFrame() = 0;
	virtual void EndFrame()	  = 0;

	// Functions to create a Studio object
	virtual CRefPtr<IStudioViewport>		  CreateViewport() const		  = 0;
	virtual CRefPtr<IStudioRenderPipelineSet> CreateRenderPipelineSet() const = 0;

	// Returns a command buffer of the render thread. If return NULL it's mean what the studio render not use the render thread
	virtual IStudioCmdBuffer* GetCommandBuffer() const = 0;
	virtual bool			  IsInRenderThread() const = 0;
};
