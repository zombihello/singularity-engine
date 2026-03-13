#pragma once
#include <stack>

#include "materialsystem/imaterial.h"
#include "studiorender/istudiorender.h"
#include "studiorender/studioapi/istudioapi_swapchain.h"
#include "studiorender/studio_renderpass_present.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioViewport;

//-----------------------------------------------------------------------------
// Studio scene view
//-----------------------------------------------------------------------------
struct studioSceneView_t
{
	matrix4x4_t viewMatrix;
	matrix4x4_t projectionMatrix;
};

//-----------------------------------------------------------------------------
// Studio render
//-----------------------------------------------------------------------------
class CStudioRender : public CBaseAppSystem<IStudioRender>
{
public:
	// IAppSystem interface
	// Here's where the app systems get to learn about each other
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// Initialize and shutdown
	virtual bool Init() override;
	virtual void Shutdown() override;

	// IStudioRender interface
	virtual void SetCameraView( const studioCameraView_t& cameraView ) override;

	// Register and unregister render objects for rendering
	virtual void RegisterObject( IStudioRenderObject* pRenderObject ) override;
	virtual void UnregisterObject( IStudioRenderObject* pRenderObject ) override;
	virtual void UnregisterAllObjects() override;

	virtual IStudioViewport*		  CreateViewport() const override;
	virtual IStudioRenderPipelineSet* CreateRenderPipelineSet() const override;
	// NOTE: FOR TEST ONLY!
	virtual IStudioRenderObject* CreateQuadRenderObject( IMaterial* pMaterial, IStudioAPIBuffer* pVertexBuffer, IStudioAPIBuffer* pIndexBuffer ) const override;

	// Returns a command buffer of the render thread. If return NULL it's mean what StudioRender don't use render thread
	virtual IStudioCmdBuffer* GetCommandBuffer() const override;
	virtual bool			  IsInRenderThread() const override;

	void BeginFrame();
	void EndFrame();

	void R_DrawFrame( CStudioViewport* pViewport );

private:
	CStudioRenderPassPresent				  presentRenderPass;
	studioSceneView_t						  sceneView;
	eastl::vector<CRefPtr<IStudioRenderObject>> renderObjects;
};
extern CStudioRender g_StudioRender;