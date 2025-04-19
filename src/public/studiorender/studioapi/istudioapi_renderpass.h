#ifndef ISTUDIOAPI_RENDERPASS_H
#define ISTUDIOAPI_RENDERPASS_H

#include "studiorender/studioapi/istudioapi_texture.h"

//-----------------------------------------------------------------------------
// Forward delcarations
//-----------------------------------------------------------------------------
class IStudioAPISwapChain;


//-----------------------------------------------------------------------------
// Render pass constants
//-----------------------------------------------------------------------------
#define STUDIOAPI_MAX_SIMULTANEOUS_RENDER_TARGETS		8


//-----------------------------------------------------------------------------
// Render pass structs
//-----------------------------------------------------------------------------
enum studioAPIRenderTargetLoadOp_t
{
	STUDIOAPI_RENDER_TARGET_LOAD_OP_LOAD,
	STUDIOAPI_RENDER_TARGET_LOAD_OP_CLEAR,
	STUDIOAPI_RENDER_TARGET_LOAD_OP_DONT_CARE
};


enum studioAPIRenderTargetStoreOp_t
{
	STUDIOAPI_RENDER_TARGET_STORE_OP_STORE,
	STUDIOAPI_RENDER_TARGET_STORE_OP_DONT_CARE
};


enum studioAPISubpassHint_t
{
	STUDIOAPI_SUBPASS_HINT_NONE
};


// 4 bits are used for depth and 4 for stencil to make the hex value readable and non overlapping
enum studioAPIDepthStencilAccessFlag_t
{
	// Don't use those directly, use the combined versions below
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_NOP		= 0x00,
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_NOP		= 0x00,
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_READ		= 0x01,
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_READ		= 0x10,
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_WRITE		= 0x02,
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_WRITE	= 0x20,
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_MASK		= 0x0F,
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_MASK		= 0xF0,

	// Use those:
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_STENCIL_NOP			= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_NOP + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_NOP,
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_READ_STENCIL_NOP		= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_READ + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_NOP,
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_WRITE_STENCIL_NOP		= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_WRITE + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_NOP,
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_NOP_STENCIL_READ		= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_NOP + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_READ,
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_STENCIL_READ			= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_READ + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_READ,
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_WRITE_STENCIL_READ		= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_WRITE + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_READ,
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_NOP_STENCIL_WRITE		= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_NOP + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_WRITE,
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_READ_STENCIL_WRITE		= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_READ + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_WRITE,
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_STENCIL_WRITE			= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_WRITE + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_WRITE
};


struct studioAPIColorRenderTargetInfo_t
{
	union
	{
		studioAPIPixelFormat_t			pixelFormat;		// Specifying the format of a texture that will be used for the render target, used only when bSwapChain is FALSE. If it is STUDIOAPI_PIXEL_FORMAT_UNKNOWN the render target will be ignored
		IStudioAPISwapChain*			pSwapChain;			// Specifying a swap chain that will be used for the render target, used only when bSwapChain is TRUE. If it is NULL the render target will be ignored
	};
	bool								bSwapChain;			// Is the render target a swap chain image
	studioAPIRenderTargetLoadOp_t		loadOp;
	studioAPIRenderTargetStoreOp_t		storeOp;
};


struct studioAPIDepthStencilRenderTargetInfo_t
{
	studioAPIPixelFormat_t				pixelFormat;
	uint8								accessFlags;
	studioAPIRenderTargetLoadOp_t		depthLoadOp;
	studioAPIRenderTargetStoreOp_t		depthStoreOp;
	studioAPIRenderTargetLoadOp_t		stencilLoadOp;
	studioAPIRenderTargetStoreOp_t		stencilStoreOp;
};


struct studioAPIRenderPassCreateInfo_t
{
	studioAPIColorRenderTargetInfo_t			colorRenderTargets[STUDIOAPI_MAX_SIMULTANEOUS_RENDER_TARGETS];
	studioAPIDepthStencilRenderTargetInfo_t		depthStencilRenderTarget;
	studioAPISubpassHint_t						subpassHint;
};


//-----------------------------------------------------------------------------
// StudioAPI render pass interface
//-----------------------------------------------------------------------------
class IStudioAPIRenderPass : public IStudioAPIResource
{
public:
	virtual ~IStudioAPIRenderPass() {}
};

#endif // !ISTUDIOAPI_RENDERPASS_H