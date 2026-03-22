#pragma once
#include "tier1/event.h"
#include "studiorender/istudio_viewport.h"
#include "studiorender/studio_renderthread.h"
#include "studiorender/istudio_renderresource.h"

//-----------------------------------------------------------------------------
// Studio viewport
//-----------------------------------------------------------------------------
class CStudioViewport : public CRefCounted<IStudioViewport>, private CStudioRenderResource<IStudioRenderResource>
{
public:
	// IStudioViewport interface
	virtual void Init( windowHandle_t windowHandle, uint32 width, uint32 height, bool bUseVSync = false ) override;
	virtual void Destroy() override;

	virtual void Resize( uint32 newWidth, uint32 newHeight ) override;
	virtual void Update( float deltaSeconds ) override;
	virtual void DrawFrame( bool bShouldPresent = true, bool bFlushRenderCmds = true ) override;

	virtual void SetViewportClient( IStudioViewportClient* pStudioViewportClient ) override;

	virtual bool				   IsInited() const override;
	virtual bool				   IsUseVSync() const override;
	virtual IStudioViewportClient* GetViewportClient() const override;
	virtual vector2i_t			   GetSize() const override;
	virtual IStudioAPISwapChain*   GetStudioAPISwapChain() const override;

	// The function returns a StudioAPI render pass for draw into the viewport
	virtual IStudioAPIRenderPass* GetStudioAPIRenderPass() const override;

	// The function returns a StudioAPI frame buffers for render into the viewport (valid only per a frame, because the viewport can use double/triple/etc buffering)
	virtual IStudioAPIFrameBuffer* GetStudioAPIFrameBuffer() const override;

	DECLARE_EVENT( COnReleaseViewportIndex, CStudioViewport* /* pViewport */ );
	DECLARE_EVENT( COnRenderPassUpdated, CStudioViewport* /* pViewport */ );

	CStudioViewport();
	~CStudioViewport();

	static CStudioViewport* R_GetActiveViewport();
	uint32					GetIndex() const;

	COnReleaseViewportIndex& OnReleaseViewportIndex() const;
	COnRenderPassUpdated&	 OnRenderPassUpdated() const;

	// IRefCounted interface
protected:
	virtual void FinalRelease() override;

private:
	// IStudioRenderResource interface
	virtual void InitStudioAPI() override;
	virtual void UpdateStudioAPI() override;
	virtual void ReleaseStudioAPI() override;

	// Part of DrawFrame() function that executes at the render thread
	void		R_DrawFrame( bool bShouldPresent );
	static void OnSwapChainReCreated( void* pUserData, IStudioAPISwapChain* pStudioAPISwapChain, bool bChangedImageFormat );

	uint32 AcquireIndex();
	void   ReleaseIndex( uint32 viewportIndex );

	bool										  bUseVSync;
	uint32										  index;
	windowHandle_t								  windowHandle;
	IStudioViewportClient*						  pStudioViewportClient;
	vector2i_t									  size;
	CRefPtr<IStudioAPISwapChain>				  pStudioAPISwapChain;
	CRefPtr<IStudioAPIRenderPass>				  pStudioAPIRenderPass;
	IStudioAPISwapChain::IOnReCreated::handle_t	  onSwapChainReCreatedHandle;
	mutable COnReleaseViewportIndex				  onReleaseViewportIndex;
	mutable COnRenderPassUpdated				  onRenderPassUpdated;
	eastl::vector<CRefPtr<IStudioAPIFrameBuffer>> studioAPIFrameBuffers;
	static CRefPtr<CStudioViewport>				  s_pActiveViewport;
	static CThreadMutex							  s_ViewportIndexMutex;
	static uint32								  s_LastViewportIndex;
	static eastl::list<uint32>					  s_FreeViewportIndices;
};

#include "studiorender/studio_viewport.inl"
