#pragma once
#include "stdlib/refcount.h"
#include "appframework/iappsystem.h"
#include "studiorender/istudio_cmdbuffer.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioViewport;
class IStudioRenderPipelineSet;
class IMaterial;
class IStudioAPIBuffer;
class IStudioRenderObject;


//-----------------------------------------------------------------------------
// Studio camera view
//-----------------------------------------------------------------------------
struct studioCameraView_t
{
	vec3_t	location;
	quat_t	rotation;
	float	fieldOfView;
	float	nearClipPlane;
	float	farClipPlane;
	float	aspectRatio;
};


//-----------------------------------------------------------------------------
// Studio render interface
//-----------------------------------------------------------------------------
#define STUDIORENDER_INTERFACE_VERSION		"SStudioRender001"
class IStudioRender : public IAppSystem
{
public:
	virtual void SetCameraView( const studioCameraView_t& cameraView ) = 0;

	// Register and unregister render objects for rendering
	virtual void RegisterObject( IStudioRenderObject* pRenderObject ) = 0;
	virtual void UnregisterObject( IStudioRenderObject* pRenderObject ) = 0;
	virtual void UnregisterAllObjects() = 0;

	virtual IStudioViewport* CreateViewport() const = 0;
	virtual IStudioRenderPipelineSet* CreateRenderPipelineSet() const = 0;
	// NOTE: FOR TEST ONLY!
	virtual IStudioRenderObject* CreateQuadRenderObject( IMaterial* pMaterial, IStudioAPIBuffer* pVertexBuffer, IStudioAPIBuffer* pIndexBuffer ) const = 0;

	// Returns a command buffer of the render thread. If return NULL it's mean what StudioRender don't use render thread
	virtual IStudioCmdBuffer* GetCommandBuffer() const = 0;
	virtual bool IsInRenderThread() const = 0;
};