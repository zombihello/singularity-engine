/**
 * @file
 * @addtogroup studioapi studioapi
 */

#ifndef ISTUDIOAPI_RENDERPASS_H
#define ISTUDIOAPI_RENDERPASS_H

#include "studiorender/studioapi/istudioapi_texture.h"

// Forward delcarations
class IStudioAPISwapChain;

/**
 * @ingroup studioapi
 * @brief StudioAPI the number of render targets that may be simultaneously written to
 */
#define STUDIOAPI_MAX_SIMULTANEOUS_RENDER_TARGETS		8

/**
 * @ingroup studioapi
 * @brief Specify how contents of an render target are treated at the beginning of the StudioAPI render pass where it is first used
 */
enum studioAPIRenderTargetLoadOp_t
{
	STUDIOAPI_RENDER_TARGET_LOAD_OP_LOAD,			/**< Specifies that the previous contents of the image within the render area will be preserved as the initial values */
	STUDIOAPI_RENDER_TARGET_LOAD_OP_CLEAR,			/**< Specifies that the contents within the render area will be cleared to a uniform value, which is specified when a render pass instance is begun */
	STUDIOAPI_RENDER_TARGET_LOAD_OP_DONT_CARE		/**< Specifies that the previous contents within the area need not be preserved */
};

/**
 * @ingroup studioapi
 * @brief Specify how contents of an render target are treated at the end of the StudioAPI render pass where it is last used
 */
enum studioAPIRenderTargetStoreOp_t
{
	STUDIOAPI_RENDER_TARGET_STORE_OP_STORE,			/**< Specifies the contents generated during the render pass and within the render area are written to memory */
	STUDIOAPI_RENDER_TARGET_STORE_OP_DONT_CARE		/**< Specifies the contents within the render area are not needed after rendering, and may be discarded */
};

/**
 * @ingroup studioapi
 * @brief Hints for some StudioAPIs that support subpasses
 */
enum studioAPISubpassHint_t
{
	STUDIOAPI_SUBPASS_HINT_NONE		/**< Regular rendering */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI depth/stencil access flag
 * 4 bits are used for depth and 4 for stencil to make the hex value readable and non overlapping
 */
enum studioAPIDepthStencilAccessFlag_t
{
	// Don't use those directly, use the combined versions below
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_NOP		= 0x00,		/**< Depth isn't used */
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_NOP		= 0x00,		/**< Stencil isn't used */
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_READ		= 0x01,		/**< Depth read only */
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_READ		= 0x10,		/**< Stencil read only */
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_WRITE		= 0x02,		/**< Depth read/write */
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_WRITE	= 0x20,		/**< Stencil read/write */
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_MASK		= 0x0F,		/**< Depth mask */
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_MASK		= 0xF0,		/**< Stencil mask */

	// Use those:
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_STENCIL_NOP			= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_NOP + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_NOP,		/**< Depth and stencil aren't used */
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_READ_STENCIL_NOP		= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_READ + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_NOP,		/**< Depth read only, stencil isn't used */
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_WRITE_STENCIL_NOP		= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_WRITE + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_NOP,		/**< Depth read/write, stencil isn't used */
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_NOP_STENCIL_READ		= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_NOP + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_READ,		/**< Depth isn't used, stencil read only */
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_STENCIL_READ			= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_READ + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_READ,		/**< Depth and stencil read only */
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_WRITE_STENCIL_READ		= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_WRITE + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_READ,		/**< Depth read/write, stencil read only */
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_NOP_STENCIL_WRITE		= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_NOP + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_WRITE,		/**< Depth isn't used, stencil read/write */
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_READ_STENCIL_WRITE		= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_READ + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_WRITE,		/**< Depth read only, stencil read/write */
	STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_STENCIL_WRITE			= STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_WRITE + STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_STENCIL_WRITE		/**< Depth and stencil read/write */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI color render target information for a render pass
 */
struct studioAPIColorRenderTargetInfo_t
{
	union
	{
		studioAPIPixelFormat_t			pixelFormat;		/**< Specifying the format of a texture that will be used for the render target, used only when bSwapChain is FALSE. If it is STUDIOAPI_PIXEL_FORMAT_UNKNOWN the render target will be ignored */
		IStudioAPISwapChain*			pSwapChain;			/**< Specifying a swap chain that will be used for the render target, used only when bSwapChain is TRUE. If it is NULL the render target will be ignored */
	};
	bool								bSwapChain;			/**< Is the render target a swap chain image */
	studioAPIRenderTargetLoadOp_t		loadOp;				/**< Specifying how the contents of the render target are treated at the beginning of a render pass where it is first used */
	studioAPIRenderTargetStoreOp_t		storeOp;			/**< Specifying how the contents of the render target are treated at the end of a render pass where it is last used */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI depth/stencil render target information for a render pass
 */
struct studioAPIDepthStencilRenderTargetInfo_t
{
	studioAPIPixelFormat_t				pixelFormat;		/**< Specifying the format of a texture that will be used for the render target. If it is STUDIOAPI_PIXEL_FORMAT_UNKNOWN the render target will be ignored */
	uint8								accessFlags;		/**< Depth/stencil access flags (see studioAPIDepthStencilAccessFlag_t) */
	studioAPIRenderTargetLoadOp_t		depthLoadOp;		/**< Specifying how the contents of depth components of the render target are treated at the beginning of a render pass where it is first used */
	studioAPIRenderTargetStoreOp_t		depthStoreOp;		/**< Specifying how the contents of depth components of the render target are treated at the end of a render pass where it is last used */
	studioAPIRenderTargetLoadOp_t		stencilLoadOp;		/**< Specifying how the contents of stencil components of the render target are treated at the beginning of a render pass where it is first used */
	studioAPIRenderTargetStoreOp_t		stencilStoreOp;		/**< Specifying how the contents of stencil components of the render target are treated at the end of a render pass where it is last used */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI render pass create information
 */
struct studioAPIRenderPassCreateInfo_t
{
	studioAPIColorRenderTargetInfo_t			colorRenderTargets[STUDIOAPI_MAX_SIMULTANEOUS_RENDER_TARGETS];		/**< Color render targets */
	studioAPIDepthStencilRenderTargetInfo_t		depthStencilRenderTarget;											/**< Depth/stencil render target */
	studioAPISubpassHint_t						subpassHint;														/**< Hint for some StudioAPI's that render pass will have specific sub-passes */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI render pass interface
 */
class IStudioAPIRenderPass : public IStudioAPIResource
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IStudioAPIRenderPass() {}
};

#endif // !ISTUDIOAPI_RENDERPASS_H