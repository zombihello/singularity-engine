/**
 * @file
 * @addtogroup studiorender studiorender
 */

#ifndef ISTUDIO_VIEWPORT_H
#define ISTUDIO_VIEWPORT_H

#include "stdlib/refcount.h"
#include "stdlib/math/math.h"
#include "studiorender/studioapi/istudioapi_swapchain.h"
#include "studiorender/istudio_renderresource.h"
#include "studiorender/istudio_viewportclient.h"

/**
 * @ingroup studiorender
 * @brief Studio viewport interface
 */
class IStudioViewport : public IRefCounted
{
public:
	/**
	 * @brief Initialize viewport
	 * @param windowHandle		Window handle where will be created the viewport
	 * @param width				Viewport width
	 * @param height			Viewport height
	 * @param bUseVSync			Is it necessary to use VSync
	 */
	virtual void Init( windowHandle_t windowHandle, uint32 width, uint32 height, bool bUseVSync = false ) = 0;

	/**
	 * @brief Shutdown viewport
	 */
	virtual void Shutdown() = 0;

	/**
	 * @brief Resize viewport
	 * @param newWidth		New viewport width
	 * @param newHeight		New viewport height
	 */
	virtual void Resize( uint32 newWidth, uint32 newHeight ) = 0;
	
	/**
	 * @brief Update viewport logic
	 * @param deltaSeconds		Delta time since the last frame (in seconds)
	 */
	virtual void Update( float deltaSeconds ) = 0;

	/**
	 * @brief Draw frame
	 * @param bShouldPresent	Is need present of the frame
	 * @param bFlushRenderCmds	Is need flush render commands
	 */
	virtual void DrawFrame( bool bShouldPresent = true, bool bFlushRenderCmds = true ) = 0;

	/**
	 * @brief Is initialized viewport
	 * @return Return TRUE if the viewport is initialized, otherwise FALSE
	 */
	virtual bool IsInited() const = 0;

	/**
	 * @brief Set viewport client
	 * @param pStudioViewportClient		A new viewport client
	 */
	virtual void SetViewportClient( IStudioViewportClient* pStudioViewportClient ) = 0;

	/**
	 * @brief Get viewport client
	 * @return Return viewport client. If the viewport have not return NULL 
	 */
	virtual IStudioViewportClient* GetViewportClient() const = 0;

	/**
	 * @brief Get viewport size
	 * @return Return viewport size
	 */
	virtual ivec2_t GetSize() const = 0;

	/**
	 * @brief Get StudioAPI swap chain
	 * @return Return StudioAPI swap chain. If the viewport isn't initialized return NULL
	 */
	virtual IStudioAPISwapChain* GetStudioAPISwapChain() const = 0;

	/**
	 * @brief Get StudioAPI render pass
	 * The function returns a StudioAPI render pass for into the viewport
	 * @return Return StudioAPI render pass. If the viewport isn't initialized return NULL
	 */
	virtual IStudioAPIRenderPass* GetStudioAPIRenderPass() const = 0;

	/**
	 * @brief Get StudioAPI frame buffer
	 * The function returns a StudioAPI frame buffers for render into the viewport (valid only per a frame, because the viewport can use double/triple/etc buffering)
	 * @return Return StudioAPI frame buffer. If the viewport isn't initialized return NULL
	 */
	virtual IStudioAPIFrameBuffer* GetStudioAPIFrameBuffer() const = 0;

	/**
	 * @brief Is use VSync
	 * @return Return TRUE if the viewport using VSync, otherwise FALSE
	 */
	virtual bool IsUseVSync() const = 0;
};

#endif // !ISTUDIO_VIEWPORT_H