#ifndef ISTUDIOAPI_FRAMEBUFFER_H
#define ISTUDIOAPI_FRAMEBUFFER_H

#include "stdlib/math/math.h"
#include "stdlib/math/color.h"
#include "studiorender/studioapi/istudioapi_renderpass.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPITexture;
class IStudioAPISwapChainImage;


//-----------------------------------------------------------------------------
// Render target flags and some structs
//-----------------------------------------------------------------------------
enum studioAPIRenderTargetFlag_t
{
	STUDIOAPI_RENDER_TARGET_FLAG_SWAPCHAIN_IMAGE	= 1 << 0,	// The render target it is an image of a swap chain
	STUDIOAPI_RENDER_TARGET_FLAG_DEPTH_ONLY			= 1 << 1,	// Use depth only. If set STUDIOAPI_RENDER_TARGET_FLAG_SWAPCHAIN_IMAGE you can't use the flag
	STUDIOAPI_RENDER_TARGET_FLAG_STENCIL_ONLY		= 1 << 2	// Use stencil only. If set STUDIOAPI_RENDER_TARGET_FLAG_SWAPCHAIN_IMAGE you can't use the flag
};


struct studioAPIRenderTarget_t
{
	union
	{
		IStudioAPITexture*			pTexture;			// Texture, used when the render target hasn't STUDIOAPI_RENDER_TARGET_FLAG_SWAPCHAIN_IMAGE
		IStudioAPISwapChainImage*	pSwapChainImage;	// Swap chain image, used when the render target has STUDIOAPI_RENDER_TARGET_FLAG_SWAPCHAIN_IMAGE
	};
	uint8							flags;
};


struct studioAPIFrameBufferCreateInfo_t
{
	IStudioAPIRenderPass*		pRenderPass;
	studioAPIRenderTarget_t		colorRenderTargets[STUDIOAPI_MAX_SIMULTANEOUS_RENDER_TARGETS];
	studioAPIRenderTarget_t		depthStencilRenderTarget;
	ivec2_t						size;
	uint32						renderTargetMip;
	uint32						renderTargetLayer;
	bool						bClearColor;
	CColor						clearColor;
	bool						bDepthStencilClearValue;
	float						depthClearValue;
	uint32						stencilClearValue;
};


//-----------------------------------------------------------------------------
// StudioAPI frame buffer interface
//-----------------------------------------------------------------------------
class IStudioAPIFrameBuffer : public IStudioAPIResource
{
public:
	virtual ~IStudioAPIFrameBuffer() {}
	virtual ivec2_t GetSize() const = 0;
};

#endif // !ISTUDIOAPI_FRAMEBUFFER_H