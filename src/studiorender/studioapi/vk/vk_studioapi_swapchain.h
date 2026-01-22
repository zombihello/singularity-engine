#pragma once
#include "stdlib/refcount.h"
#include "studiorender/studioapi/istudioapi_swapchain.h"
#include "studiorender/studioapi/vk/vk_studioapi.h"
#include "studiorender/studioapi/vk/vk_studioapi_framebuffer.h"
#include "studiorender/studioapi/vk/vk_studioapi_renderpass.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPICmdBufferVk;
class CStudioAPISwapChainVk;
class CStudioAPICmdListVk;

//-----------------------------------------------------------------------------
// Vulkan swap chain image
//-----------------------------------------------------------------------------
class CStudioAPISwapChainImageVk : public IStudioAPISwapChainImage
{
public:
	// IStudioAPISwapChainImage interface
	virtual IStudioAPISwapChain* GetSwapChain() const override;
	virtual uint32				 GetIndex() const override;

	CStudioAPISwapChainImageVk( CStudioAPISwapChainVk* pSwapChain, uint32 imageIndex, VkImage vkImage, VkSurfaceFormatKHR vkSurfaceFormat );
	~CStudioAPISwapChainImageVk();

	// The function update a synchronization state of the texture. Useful for implicit transfer of ownership without barrier, change an image layout
	// and update access with stage masks
	void UpdateSyncState( VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex );

	// The function update a synchronization state of the texture and make an image barrier if it need. Useful for transfer ownership, change
	// an image layout and make barriers for the texture
	void UpdateSyncStateWithBarrier( CStudioAPICmdListVk* pCmdList, VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex );

	VkImage							   GetVkImage() const;
	VkImageView						   GetVkImageView() const;
	const studioAPISyncStateImageVk_t& GetSyncState() const;

private:
	uint32						imageIndex;
	VkImage						vkImage;
	VkImageView					vkImageView;
	studioAPISyncStateImageVk_t syncState;
	CStudioAPISwapChainVk*		pSwapChain;
};

//-----------------------------------------------------------------------------
// Vulkan StudioAPI swap chain
//-----------------------------------------------------------------------------
class CStudioAPISwapChainVk : public TRefCounted<IStudioAPISwapChain>
{
public:
	// IStudioAPISwapChain interface
	virtual void Resize( uint32 width, uint32 height ) override;
	virtual bool AcquireNextImage() override;
	virtual bool Present() override;
	virtual bool ReCreate() override;

	virtual bool					  IsUseVSync() const override;
	virtual bool					  IsValid() const override;
	virtual ivec2_t					  GetSize() const override;
	virtual uint32					  GetCurrentImageIndex() const override;
	virtual IStudioAPISwapChainImage* GetCurrentImage() const override;
	virtual uint32					  GetNumImages() const override;
	virtual IStudioAPISwapChainImage* GetImage( uint32 index ) const override;
	virtual windowHandle_t			  GetWindowHandle() const override;

	virtual IOnReCreated* OnReCreated() const override;

	DECLARE_MULTICAST_DELEGATE( COnReCreated, IStudioAPISwapChain* /* pStudioAPISwapChain */, bool /* bChangedImageFormat */ );

	CStudioAPISwapChainVk( const char* pDebugName = "" );
	~CStudioAPISwapChainVk();

	bool Create( windowHandle_t windowHandle, uint32 width, uint32 height, VkFormat vkPixelFormat, VkColorSpaceKHR vkColorSpace, bool bUseVSync );
	void Destroy();

	VkSurfaceFormatKHR GetVkSurfaceFormat() const;
	VkSwapchainKHR	   GetVkSwapChain() const;
	bool			   IsCreated() const;

	// NOTE: The semaphore must be set as wait semaphore at begin of a frame
	// NOTE: The semaphore valid only from AcquireNextImage call and to Present
	CStudioAPISemaphoreVk* GetImageAvailableSemaphore() const;

	// NOTE: The semaphore must be signaled at the end of the frame
	// NOTE: The semaphore valid only from AcquireNextImage call and to Present
	CStudioAPISemaphoreVk* GetRenderFinishedSemaphore() const;

private:
	static void OnStudioAPIVkShutdown( void* pUserData );

	bool									 bUseVSync;
	windowHandle_t							 windowHandle;
	VkSurfaceKHR							 vkSurface;
	VkSwapchainKHR							 vkSwapChain;
	VkSurfaceFormatKHR						 vkSurfaceFormat;
	ivec2_t									 size;
	uint32									 currentImageIndex;
	COnStudioAPIVkShutdown::funcDelegate_t*	 pStudioAPIVkShutdownDelegate;
	COnReCreated							 onReCreated;
	CStudioAPISemaphoreVk*					 pImageAvailableSemaphores[STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT];
	eastl::vector<CStudioAPISemaphoreVk*>		 renderFinishedSemaphores;
	eastl::vector<CStudioAPISwapChainImageVk*> swapChainImages;
};

#include "studiorender/studioapi/vk/vk_studioapi_swapchain.inl"