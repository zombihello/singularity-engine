#ifndef STUDIO_VIEWPORT_H
#define STUDIO_VIEWPORT_H

#include "stdlib/delegate.h"
#include "studiorender/istudio_viewport.h"
#include "studiorender/studio_renderthread.h"
#include "studiorender/istudio_renderresource.h"

//-----------------------------------------------------------------------------
// Studio viewport
//-----------------------------------------------------------------------------
class CStudioViewport : public TRefCounted<IStudioViewport>, public TStudioRenderResource<IStudioRenderResource>
{
public:
	// IStudioViewport interface
	virtual void Init( windowHandle_t windowHandle, uint32 width, uint32 height, bool bUseVSync = false ) override;
	virtual void Shutdown() override;

	virtual void Resize( uint32 newWidth, uint32 newHeight ) override;
	virtual void Update( float deltaSeconds ) override;
	virtual void DrawFrame( bool bShouldPresent = true, bool bFlushRenderCmds = true ) override;

	virtual void SetViewportClient( IStudioViewportClient* pStudioViewportClient ) override;

	virtual bool IsInited() const override;
	virtual bool IsUseVSync() const override;
	virtual IStudioViewportClient* GetViewportClient() const override;
	virtual ivec2_t GetSize() const override;
	virtual IStudioAPISwapChain* GetStudioAPISwapChain() const override;

	// The function returns a StudioAPI render pass for draw into the viewport
	virtual IStudioAPIRenderPass* GetStudioAPIRenderPass() const override;

	// The function returns a StudioAPI frame buffers for render into the viewport (valid only per a frame, because the viewport can use double/triple/etc buffering)
	virtual IStudioAPIFrameBuffer* GetStudioAPIFrameBuffer() const override;

	DECLARE_MULTICAST_DELEGATE( COnReleaseViewportIndex, CStudioViewport* /* pViewport */ );
	DECLARE_MULTICAST_DELEGATE( COnRenderPassUpdated, CStudioViewport* /* pViewport */ );

	CStudioViewport();
	~CStudioViewport();

	static CStudioViewport* R_GetActiveViewport();
	uint32 GetIndex() const;

	COnReleaseViewportIndex& OnReleaseViewportIndex() const;
	COnRenderPassUpdated& OnRenderPassUpdated() const;

private:
	// IStudioRenderResource interface
	virtual void InitStudioAPI() override;
	virtual void UpdateStudioAPI() override;
	virtual void ReleaseStudioAPI() override;

	// Part of DrawFrame() function that executes at the render thread
	void R_DrawFrame( bool bShouldPresent );
	static void OnSwapChainReCreated( void* pUserData, IStudioAPISwapChain* pStudioAPISwapChain, bool bChangedImageFormat );

	uint32 AcquireIndex();
	void ReleaseIndex( uint32 viewportIndex );

	bool															bUseVSync;
	uint32															index;
	windowHandle_t													windowHandle;
	IStudioViewportClient*											pStudioViewportClient;
	ivec2_t															size;
	TRefPtr<IStudioAPISwapChain>									pStudioAPISwapChain;
	TRefPtr<IStudioAPIRenderPass>									pStudioAPIRenderPass;
	IStudioAPISwapChain::IOnReCreated::funcDelegate_t*				pSwapChainReCreatedDelegate;
	mutable COnReleaseViewportIndex									onReleaseViewportIndex;
	mutable COnRenderPassUpdated									onRenderPassUpdated;
	std::vector<TRefPtr<IStudioAPIFrameBuffer>>						studioAPIFrameBuffers;
	static TRefPtr<CStudioViewport>									s_pActiveViewport;
	static CThreadMutex												s_ViewportIndexMutex;
	static uint32													s_LastViewportIndex;
	static std::list<uint32>										s_FreeViewportIndices;
};

#include "studiorender/studio_viewport.inl"

#endif // !STUDIO_VIEWPORT_H