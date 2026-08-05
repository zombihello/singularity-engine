#pragma once
#include "studiorender/istudiorender.h"
#include "studiorender/studio_sceneview.h"
#include "studiorender/studio_scenerendertargets.h"
#include "studiorender/studio_batchedsimpleelements.h"
#include "studiorender/studio_renderpass_scene.h"
#include "studiorender/studio_renderpass_present.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioViewport;
class IStudioAPIBuffer;
struct studioEntityView_t;

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
	virtual bool PostInit() override;
	virtual void PreShutdown() override;
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

	IStudioAPIBuffer*				 GetStudioAPIGlobalConstantBuffer() const;
	const CStudioSceneRenderTargets& GetSceneRenderTargets() const;
	CStudioRenderPassBase*			 GetRenderPass( studioRenderPassType_t type ) const;
	CStudioBatchedSimpleElements&	 GetBatchedSimpleElements();

private:
	// Adds model/resource to a scene view for the render thread
	void AddModelToSceneView( studioSceneView_t* pSceneView, studioEntityView_t* pEntityView );
	template<class TResourceClass>
	uint32 AddResourceToSceneView( studioSceneView_t* pSceneView, TResourceClass* pResource );
	uint32 AddResourceToSceneView( studioSceneView_t* pSceneView, studioResourcePtr_t pPtr, studioResourceType_t type );

	// Draw scene and rebuild render pass' frame buffers
	void R_DrawScene( CStudioViewport* pViewport, studioSceneView_t* pSceneView );
	void R_RebuildFrameBuffers( const vector2i_t& bufferSize );

	CStudioRenderPassScene		 renderPassScene;
	CStudioRenderPassPresent	 renderPassPresent;
	CStudioSceneRenderTargets	 sceneRenderTargets;
	CStudioBatchedSimpleElements batchedSimpleElements;
	CRefPtr<IStudioAPIBuffer>	 pStudioAPIGlobalConstantBuffer;
	CStudioRenderPassBase*		 pRenderPasses[STUDIO_RENDERPASS_NUM_TYPES];
};

extern CStudioRender g_StudioRender;
#include "studiorender/studiorender.inl"
