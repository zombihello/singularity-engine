/**
 * @file
 * @addtogroup studioapi studioapi

 */

#ifndef ISTUDIOAPI_SWAPCHAIN_H
#define ISTUDIOAPI_SWAPCHAIN_H

#include "stdlib/math/math.h"
#include "stdlib/delegate.h"
#include "studiorender/studioapi/istudioapi_framebuffer.h"
#include "studiorender/studioapi/istudioapi_renderpass.h"

// Forward declarations
class IStudioAPISwapChain;

/**
 * @ingroup studioapi
 * @brief StudioAPI swap chain image interface
 */
class IStudioAPISwapChainImage
{
public:
	/**
	 * @brief Get StudioAPI swap chain which the image corresponds to
	 * @return Return StudioAPI swap chain which the image corresponds to
	 */
	virtual IStudioAPISwapChain* GetSwapChain() const = 0;

	/**
	 * @brief Get image index in the swap chain
	 * @return Return image index in the swap chain
	 */
	virtual uint32 GetIndex() const = 0;
};

/**
 * @ingroup studioapi
 * @brief StudioAPI swap chain interface
 */
class IStudioAPISwapChain : public IStudioAPIResource
{
public:
	/**
	 * @brief Delegate interface for handle event when the swap chain has been re-created
	 */
	DECLARE_MULTICAST_DELEGATE_INTERFACE( IOnReCreated, IStudioAPISwapChain* /* pStudioAPISwapChain */, bool /* bChangedImageFormat */ );

	/**
	 * @brief Destructor
	 */
	virtual ~IStudioAPISwapChain() {}

	/**
	 * @brief Resize the swap chain
	 * @param width			New width
	 * @param height		New height
	 */
	virtual void Resize( uint32 width, uint32 height ) = 0;

	/**
	 * @brief Acquire the next swap chain image
	 * @return Return TRUE if the next swap chain image was acquired successfully. In case when the image is out of date return FALSE
	 */
	virtual bool AcquireNextImage() = 0;

	/**
	 * @brief Present the current swap chain image
	 * @return Return TRUE if the swap chain image was present successfully. In case when the image is out of date return FALSE
	 */
	virtual bool Present() = 0;

	/**
	 * @brief Recreate the swap chain
	 * @return Return TRUE if the swap chain successfully has been recreated, otherwise FALSE
	 */
	virtual bool ReCreate() = 0;

	/**
	 * @brief Get the swap chain image size
	 * @return Return the swap chain image size
	 */
	virtual ivec2_t GetSize() const = 0;

	/**
	 * @brief Get index of current swap chain image
	 * @return Return index of current swap chain image
	 */
	virtual uint32 GetCurrentImageIndex() const = 0;

	/**
	 * @brief Get the current swap chain image
	 * @return Return the current swap chain image
	 */
	virtual IStudioAPISwapChainImage* GetCurrentImage() const = 0;

	/**
	 * @brief Get swap chain images number
	 * @return Return images number in the swap chain
	 */
	virtual uint32 GetNumImages() const = 0;

	/**
	 * @brief Get swap chain image by index
	 * @param index		Image index
	 * @return Return swap chain image by index
	 */
	virtual IStudioAPISwapChainImage* GetImage( uint32 index ) const = 0;

	/**
	 * @breif Get the window handle
	 * @return Return the window handle
	 */
	virtual windowHandle_t GetWindowHandle() const = 0;

	/**
	 * @brief Get delegate for handle event when the swap chain has been re-created
	 * @return Return delegate for handle event when the swap chain has been re-created
	 */
	virtual IOnReCreated* OnReCreated() const = 0;

	/**
	 * @brief Is use VSync
	 * @return Return TRUE if the swap chain using VSync, otherwise FALSE
	 */
	virtual bool IsUseVSync() const = 0;

	/**
	 * @brief Is the swap chain valid
	 * @return Return TRUE if the swap chain is valid, otherwise FALSE
	 */
	virtual bool IsValid() const = 0;
};

#endif // !ISTUDIOAPI_SWAPCHAIN_H