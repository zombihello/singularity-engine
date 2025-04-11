/**
 * @file
 * @addtogroup studioapi studioapi
 */

#ifndef ISTUDIOAPI_FRAMEBUFFER_H
#define ISTUDIOAPI_FRAMEBUFFER_H

#include "stdlib/math/math.h"
#include "stdlib/math/color.h"
#include "studiorender/studioapi/istudioapi_renderpass.h"

// Forward declarations
class IStudioAPITexture;
class IStudioAPISwapChainImage;

/**
 * @ingroup studioapi
 * @brief StudioAPI render target flag
 */
enum studioAPIRenderTargetFlag_t
{
	STUDIOAPI_RENDER_TARGET_FLAG_SWAPCHAIN_IMAGE	= 1 << 0,	/**< The render target it is an image of a swap chain */
	STUDIOAPI_RENDER_TARGET_FLAG_DEPTH_ONLY			= 1 << 1,	/**< Use depth only. If set STUDIOAPI_RENDER_TARGET_FLAG_SWAPCHAIN_IMAGE you can't use the flag */
	STUDIOAPI_RENDER_TARGET_FLAG_STENCIL_ONLY		= 1 << 2	/**< Use stencil only. If set STUDIOAPI_RENDER_TARGET_FLAG_SWAPCHAIN_IMAGE you can't use the flag */
};

/**
 * @brief StudioAPI render target
 */
struct studioAPIRenderTarget_t
{
	union
	{
		IStudioAPITexture*			pTexture;			/**< Texture, used when the render target hasn't STUDIOAPI_RENDER_TARGET_FLAG_SWAPCHAIN_IMAGE */
		IStudioAPISwapChainImage*	pSwapChainImage;	/**< Swap chain image, used when the render target has STUDIOAPI_RENDER_TARGET_FLAG_SWAPCHAIN_IMAGE */
	};
	uint8							flags;				/**< Render target flags */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI frame buffer create information
 */
struct studioAPIFrameBufferCreateInfo_t
{
	IStudioAPIRenderPass*		pRenderPass;													/**< Render pass which a frame buffer will be created for */
	studioAPIRenderTarget_t		colorRenderTargets[STUDIOAPI_MAX_SIMULTANEOUS_RENDER_TARGETS];	/**< Color render targets */
	studioAPIRenderTarget_t		depthStencilRenderTarget;										/**< Depth stencil render target  */
	ivec2_t						size;															/**< Frame buffer size */
	uint32						renderTargetMip;												/**< Render target mip level */
	uint32						renderTargetLayer;												/**< Render target layer */
	bool						bClearColor;													/**< Is need clear color at begin render to the frame buffer */
	CColor						clearColor;														/**< Clear color */
	bool						bDepthStencilClearValue;										/**< Is need clear depth stencil at begin render to the frame buffer */
	float						depthClearValue;												/**< Depth clear value */
	uint32						stencilClearValue;												/**< Stencil clear value */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI frame buffer interface
 */
class IStudioAPIFrameBuffer : public IStudioAPIResource
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IStudioAPIFrameBuffer() {}

	/**
	 * @brief Get the frame buffer image size
	 * @return Return the frame buffer size
	 */
	virtual ivec2_t GetSize() const = 0;
};

#endif // !ISTUDIOAPI_FRAMEBUFFER_H