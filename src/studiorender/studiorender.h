#ifndef STUDIORENDER_H
#define STUDIORENDER_H

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
// Studio render
//-----------------------------------------------------------------------------
class CStudioRender : public CBaseAppSystem<IStudioRender>
{
public:
	// IAppSystem interface
	// Here's where the app systems get to learn about each other
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void* QueryInterface( const achar* pInterfaceName ) override;

	// Initialize and shutdown
	virtual bool Init() override;
	virtual void Shutdown() override;

	// IStudioRender interface
	// Sets which API we should be using. Has to be done before connect
	virtual void SetStudioAPI( const achar* pStudioAPIDLL ) override;

	// NOTE: FOR TEST ONLY!
	virtual void DrawQuad( IMaterial* pMaterial, IStudioAPIBuffer* pVertexBuffer, IStudioAPIBuffer* pIndexBuffer ) override;

	virtual IStudioViewport* CreateViewport() const override;
	virtual IStudioRenderPipelineSet* CreateRenderPipelineSet() const override;

	// Returns a command buffer of the render thread. If return NULL it's mean what StudioRender don't use render thread
	virtual IStudioCmdBuffer* GetCommandBuffer() const override;
	virtual bool IsInRenderThread() const override;

	CStudioRender();

	void BeginFrame();
	void EndFrame();

	void R_DrawFrame( CStudioViewport* pViewport );

private:
	createInterfaceFn_t CreateStudioAPI( const achar* pStudioAPIDLL );
	void DestroyStudioAPI();

	std::string					studioAPIDLLName;
	dllHandle_t					studioAPIHandle;
	createInterfaceFn_t			pStudioAPIFactory;
	createInterfaceFn_t			pAppSystemFactory;
	CStudioRenderPassPresent	presentRenderPass;
};
extern CStudioRender			g_StudioRender;

#endif // !STUDIORENDER_H