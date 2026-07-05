#pragma once
#include "tier1/framealloc.h"
#include "studiorender/istudiorender.h"
#include "studiorender/studio_renderpass_present.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioViewport;

//-----------------------------------------------------------------------------
// Studio frame allocator
//-----------------------------------------------------------------------------
typedef CFrameAlloc<STUDIO_FRAMEALLOC_BLOCK_SIZE, STUDIO_FRAMEALLOC_NUM_POOLS> studioFrameAlloc_t;

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
	// Functions to draw scene
	virtual void BeginFrame() override;
	virtual void EndFrame() override;
	virtual void DrawScene( IStudioViewport* pStudioViewport, IStudioScene* pStudioScene, const studioCameraView_t& cameraView ) override;

	virtual CRefPtr<IStudioViewport>		  CreateViewport() const override;
	virtual CRefPtr<IStudioRenderPipelineSet> CreateRenderPipelineSet() const override;
	virtual CRefPtr<IStudioScene>			  CreateScene() const override;

	// Returns a command buffer of the render thread. If return NULL it's mean what StudioRender don't use render thread
	virtual IStudioCmdBuffer* GetCommandBuffer() const override;
	virtual bool			  IsInRenderThread() const override;

	CStudioRender();
	studioFrameAlloc_t& GetFrameAlloc();

private:
	void AddDrawSurfacesToSceneView( studioSceneView_t* pSceneView );
	void R_DrawScene( CStudioViewport* pViewport, studioSceneView_t* pSceneView );

	CStudioRenderPassPresent presentRenderPass;
	studioFrameAlloc_t		 frameAlloc;
};

extern CStudioRender g_StudioRender;
#include "studiorender/studiorender.inl"
