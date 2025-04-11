/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_SWAPCHAIN_H
#define VK_STUDIOAPI_SWAPCHAIN_H

#include "stdlib/refcount.h"
#include "studiorender/studioapi/istudioapi_swapchain.h"
#include "studiorender/studioapi/vk/vk_studioapi.h"
#include "studiorender/studioapi/vk/vk_studioapi_framebuffer.h"
#include "studiorender/studioapi/vk/vk_studioapi_renderpass.h"

// Forward declarations
class CStudioAPICmdBufferVk;
class CStudioAPISwapChainVk;
class CStudioAPICmdListVk;

/**
 * @ingroup studioapi_vk
 * @brief Vulkan swap chain image
 */
class CStudioAPISwapChainImageVk : public IStudioAPISwapChainImage
{
public:
	/**
	 * @brief Constructor
	 * @param pSwapChain		Swap chain owner
	 * @param imageIndex		Image index
	 * @param vkImage			Vulkan swap chain image
	 * @param vkSurfaceFormat	Vulkan surface format of the swap chain image
	 */
	CStudioAPISwapChainImageVk( CStudioAPISwapChainVk* pSwapChain, uint32 imageIndex, VkImage vkImage, VkSurfaceFormatKHR vkSurfaceFormat );

	/**
	 * @brief Destructor
	 */
	~CStudioAPISwapChainImageVk();

	/**
	 * @brief Get StudioAPI swap chain which the image corresponds to
	 * @return Return StudioAPI swap chain which the image corresponds to
	 */
	virtual IStudioAPISwapChain* GetSwapChain() const override;

	/**
	 * @brief Get image index in the swap chain
	 * @return Return image index in the swap chain
	 */
	virtual uint32 GetIndex() const override;

	/**
	 * @brief Update a synchronization state of the texture
	 * The function update a synchronization state of the texture. Useful for
	 * for implicit transfer of ownership without barrier, change an image layout
	 * and update access with stage masks
	 *
	 * @param vkDstImageLayout		Destination image layout
	 * @param vkDstAccessMask		Destination Vulkan access mask
	 * @param vkDstStageMask		Destination Vulkan pipeline stage mask
	 * @param dstQueueFamilyIndex	Destination queue family which will be owner of the resource
	 */
	FORCEINLINE void UpdateSyncState( VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex )
	{
		PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
		VK_UpdateSyncStateImage( vkDstImageLayout, vkDstAccessMask, vkDstStageMask, dstQueueFamilyIndex, syncState );
	}

	/**
	 * @brief Update a synchronization state and make an image barrier
	 * The function update a synchronization state of the texture and make
	 * an image barrier if it need. Useful for transfer ownership, change
	 * an image layout and make barriers for the texture
	 *
	 * @param pCmdList				A command list which will be placed a barrier
	 * @param vkDstImageLayout		Destination image layout
	 * @param vkDstAccessMask		Destination Vulkan access mask
	 * @param vkDstStageMask		Destination Vulkan pipeline stage mask
	 * @param dstQueueFamilyIndex	Destination queue family which will be owner of the resource
	 */
	void UpdateSyncStateWithBarrier( CStudioAPICmdListVk* pCmdList, VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex );

	/**
	 * @brief Get Vulkan image
	 * @return Return vulkan image
	 */
	FORCEINLINE VkImage GetVkImage() const
	{
		return vkImage;
	}

	/**
	 * @brief Get Vulkan image view
	 * @return Return vulkan image view
	 */
	FORCEINLINE VkImageView GetVkImageView() const
	{
		return vkImageView;
	}

	/**
	 * @brief Get synchronization state
	 * @return Return synchronization state
	 */
	FORCEINLINE const studioAPISyncStateImageVk_t& GetSyncState() const
	{
		return syncState;
	}

private:
	uint32							imageIndex;		/**< Image index */
	VkImage							vkImage;		/**< Vulkan image */
	VkImageView						vkImageView;	/**< Vulkan image view */
	studioAPISyncStateImageVk_t		syncState;		/**< Synchronization state */
	CStudioAPISwapChainVk*			pSwapChain;		/**< Swap chain */
};


/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI swap chain
 */
class CStudioAPISwapChainVk : public TRefCounted<IStudioAPISwapChain>
{
public:
	/**
	 * @brief Delegate for handle event when the swap chain has been re-created
	 */
	DECLARE_MULTICAST_DELEGATE( COnReCreated, IStudioAPISwapChain* /* pStudioAPISwapChain */, bool /* bChangedImageFormat */ );

	/*
	 * @brief Constructor
	 * @param pDebugName	Debug name
	 */
	CStudioAPISwapChainVk( const achar* pDebugName = "" );

	/**
	 * @brief Destructor
	 */
	~CStudioAPISwapChainVk();

	/**
	 * @brief Create a swap chain
	 * @param windowHandle		OS handle on window
	 * @param width				Width
	 * @param height			Height
	 * @param vkPixelFormat		Image pixel format of swap chain
	 * @param vkColorSpace		Image color space of swap chain
	 * @param bUseVSync			Is it necessary to use VSync
	 * @return Return TRUE if the swap chain successfully created, otherwise FALSE
	 */
	bool Create( windowHandle_t windowHandle, uint32 width, uint32 height, VkFormat vkPixelFormat, VkColorSpaceKHR vkColorSpace, bool bUseVSync );

	/**
	 * @brief Destroy the swap chain
	 */
	void Destroy();

	/**
	 * @brief Resize the swap chain
	 * @param width			New width
	 * @param height		New height
	 */
	virtual void Resize( uint32 width, uint32 height ) override;

	/**
	 * @brief Acquire the next swap chain image
	 * @return Return TRUE if the next swap chain image was acquired successfully. In case when the image is out of date return FALSE
	 */
	virtual bool AcquireNextImage() override;

	/**
	 * @brief Present the current swap chain image
	 * @return Return TRUE if the swap chain image was present successfully. In case when the image is out of date return FALSE
	 */
	virtual bool Present() override;

	/**
	 * @brief Recreate the swap chain
	 * @return Return TRUE if the swap chain successfully has been recreated, otherwise FALSE
	 */
	virtual bool ReCreate() override;

	/**
	 * @brief Get swap chain surface format
	 * @return Return swap chain surface format. If one is not created return empty struct
	 */
	FORCEINLINE VkSurfaceFormatKHR GetVkSurfaceFormat() const
	{
		return vkSurfaceFormat;
	}

	/**
	 * @brief Get the swap chain image size
	 * @return Return the swap chain image size
	 */
	virtual ivec2_t GetSize() const override;

	/**
	 * @brief Get Vulkan swap chain
	 * @return Return Vulkan swap chain
	 */
	FORCEINLINE VkSwapchainKHR GetVkSwapChain() const
	{
		return vkSwapChain;
	}

	/**
	 * @brief Get index of current swap chain image
	 * @return Return index of current swap chain image
	 */
	virtual uint32 GetCurrentImageIndex() const override;

	/**
	 * @brief Get the current swap chain image
	 * @return Return the current swap chain image
	 */
	virtual IStudioAPISwapChainImage* GetCurrentImage() const override;

	/**
	 * @brief Get swap chain images number
	 * @return Return images number in the swap chain
	 */
	virtual uint32 GetNumImages() const override;

	/**
	 * @brief Get swap chain image by index
	 * @param index		Image index
	 * @return Return swap chain image by index
	 */
	virtual IStudioAPISwapChainImage* GetImage( uint32 index ) const override;

	/**
	 * @breif Get the window handle
	 * @return Return the window handle
	 */
	virtual windowHandle_t GetWindowHandle() const override;

	/**
	 * @brief Is created swap chain
	 * @return Return TRUE if swap chain is created, otherwise return FALSE
	 */
	FORCEINLINE bool IsCreated() const
	{
		return vkSwapChain != VK_NULL_HANDLE;
	}

	/**
	 * @brief Get delegate for handle event when the swap chain has been re-created
	 * @return Return delegate for handle event when the swap chain has been re-created
	 */
	virtual IOnReCreated* OnReCreated() const override;

	/**
	 * @brief Is use VSync
	 * @return Return TRUE if the swap chain using VSync, otherwise FALSE
	 */
	virtual bool IsUseVSync() const override;

	/**
	 * @brief Is the swap chain valid
	 * @return Return TRUE if the swap chain is valid, otherwise FALSE
	 */
	virtual bool IsValid() const override;

	/**
	 * @brief Get the current image available semaphore
	 * The semaphore must be set as wait semaphore at begin of a frame
	 * 
	 * @warning The semaphore valid only from AcquireNextImage call and to Present
	 * @return Return the current image available semaphore
	 */
	FORCEINLINE CStudioAPISemaphoreVk* GetImageAvailableSemaphore() const
	{
		return imageAvailableSemaphores[currentSemaphoreIndex];
	}

	/**
	 * @brief Get the current render finished semaphore
	 * The semaphore must be signaled at the end of the frame
	 * 
	 * @warning The semaphore valid only from AcquireNextImage call and to Present
	 * @return Return the current render finished semaphore
	 */
	FORCEINLINE CStudioAPISemaphoreVk* GetRenderFinishedSemaphore() const
	{
		return renderFinishedSemaphores[currentSemaphoreIndex];
	}

private:
	/**
	 * @brief Delegate called when StudioAPI Vulkan is shutdown 
	 * @param pUserData		Pointer to CStudioAPISwapChainVk
	 */
	static void OnStudioAPIVkShutdown( void* pUserData );

	bool											bUseVSync;							/**< Is it necessary to use VSync */
	windowHandle_t									windowHandle;						/**< Window handle */
	VkSurfaceKHR									vkSurface;							/**< Vulkan surface */
	VkSwapchainKHR									vkSwapChain;						/**< Vulkan swap chain */
	VkSurfaceFormatKHR								vkSurfaceFormat;					/**< Vulkan surface format */
	ivec2_t											size;								/**< Image size */
	uint32											currentImageIndex;					/**< Current image index */
	uint32											currentSemaphoreIndex;				/**< Current semaphore index */
	COnStudioAPIVkShutdown::funcDelegate_t*			pStudioAPIVkShutdownDelegate;		/**< Delegate handle 'on StudioAPI Vulkan shutdown' */
	COnReCreated									onReCreated;						/**< Handle event when the swap chain has been re-created */
	std::vector<CStudioAPISemaphoreVk*>				imageAvailableSemaphores;			/**< Semaphores are signal when in swapchain is exist available image */
	std::vector<CStudioAPISemaphoreVk*>				renderFinishedSemaphores;			/**< Semaphores are signal when render is finished */
	std::vector<CStudioAPISwapChainImageVk*>		swapChainImages;					/**< Swap chain images. There will be output frames in a window */
};

#endif // !VK_STUDIOAPI_SWAPCHAIN_H