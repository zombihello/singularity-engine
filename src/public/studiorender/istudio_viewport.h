#pragma once
#include "tier1/refcount.h"
#include "tier1/math/math.h"
#include "studiorender/studioapi/istudioapi_swapchain.h"
#include "studiorender/istudio_viewportclient.h"

//-----------------------------------------------------------------------------
// Studio viewport interface
//-----------------------------------------------------------------------------
class IStudioViewport : public IRefCounted
{
public:
	virtual void Init( windowHandle_t windowHandle, uint32 width, uint32 height, bool bUseVSync = false ) = 0;
	virtual void Destroy()																				  = 0;

	virtual void Resize( uint32 newWidth, uint32 newHeight )						   = 0;
	virtual void Update( float deltaSeconds )										   = 0;
	virtual void DrawFrame( bool bShouldPresent = true, bool bFlushRenderCmds = true ) = 0;

	virtual void SetViewportClient( IStudioViewportClient* pStudioViewportClient ) = 0;

	virtual bool				   IsInited() const				 = 0;
	virtual bool				   IsUseVSync() const			 = 0;
	virtual IStudioViewportClient* GetViewportClient() const	 = 0;
	virtual vector2i_t			   GetSize() const				 = 0;
	virtual IStudioAPISwapChain*   GetStudioAPISwapChain() const = 0;

	// The function returns a StudioAPI render pass for draw into the viewport
	virtual IStudioAPIRenderPass* GetStudioAPIRenderPass() const = 0;

	// The function returns a StudioAPI frame buffers for render into the viewport (valid only per a frame, because the viewport can use double/triple/etc buffering)
	virtual IStudioAPIFrameBuffer* GetStudioAPIFrameBuffer() const = 0;
};
