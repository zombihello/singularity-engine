#include "pch_studioapi.h"
#include "studiorender/studioapi/vk/vk_studioapi_cmdbuffer.h"
#include "studiorender/studioapi/vk/vk_studioapi_cmdcontext.h"
#include "studiorender/studioapi/vk/vk_studioapi_swapchain.h"

/*
==================
CStudioAPISwapChainImageVk::CStudioAPISwapChainImageVk
==================
*/
CStudioAPISwapChainImageVk::CStudioAPISwapChainImageVk( CStudioAPISwapChainVk* pSwapChain, uint32 imageIndex, VkImage vkImage, VkSurfaceFormatKHR vkSurfaceFormat )
	: imageIndex( imageIndex )
	, vkImage( vkImage )
	, vkImageView( VK_NULL_HANDLE )
	, pSwapChain( pSwapChain )
{
	VkImageViewCreateInfo vkImageViewCreateInfo			  = {};
	vkImageViewCreateInfo.sType							  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	vkImageViewCreateInfo.image							  = vkImage;
	vkImageViewCreateInfo.viewType						  = VK_IMAGE_VIEW_TYPE_2D;
	vkImageViewCreateInfo.format						  = vkSurfaceFormat.format;
	vkImageViewCreateInfo.components.r					  = VK_COMPONENT_SWIZZLE_R;
	vkImageViewCreateInfo.components.g					  = VK_COMPONENT_SWIZZLE_G;
	vkImageViewCreateInfo.components.b					  = VK_COMPONENT_SWIZZLE_B;
	vkImageViewCreateInfo.components.a					  = VK_COMPONENT_SWIZZLE_A;
	vkImageViewCreateInfo.subresourceRange.aspectMask	  = VK_IMAGE_ASPECT_COLOR_BIT;
	vkImageViewCreateInfo.subresourceRange.baseMipLevel	  = 0;
	vkImageViewCreateInfo.subresourceRange.levelCount	  = 1;
	vkImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	vkImageViewCreateInfo.subresourceRange.layerCount	  = 1;
	STUDIOAPI_VK_VERIFY_RESULT( vkCreateImageView( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkImageViewCreateInfo, NULL, &vkImageView ) );
}

/*
==================
CStudioAPISwapChainImageVk::~CStudioAPISwapChainImageVk
==================
*/
CStudioAPISwapChainImageVk::~CStudioAPISwapChainImageVk()
{
	// Destroy the image view
	if ( vkImageView != VK_NULL_HANDLE )
	{
		g_StudioAPIVk.GetMemoryMgr().FreeResource( [vkImageView = vkImageView]() {
			vkDestroyImageView( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkImageView, NULL );
		} );
		vkImageView = VK_NULL_HANDLE;
	}
}

/*
==================
CStudioAPISwapChainImageVk::UpdateSyncStateWithBarrier
==================
*/
void CStudioAPISwapChainImageVk::UpdateSyncStateWithBarrier( CStudioAPICmdListVk* pCmdList, VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	studioAPIImageMemoryBarrierVk_t imageMemoryBarrier					= {};
	imageMemoryBarrier.vkImageMemoryBarrier.image						= vkImage;
	imageMemoryBarrier.vkImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	if ( VK_UpdateSyncStateImageWithBarrier( vkDstImageLayout, vkDstAccessMask, vkDstStageMask, dstQueueFamilyIndex, syncState, imageMemoryBarrier, 0 ) )
	{
		CStudioAPICmdContextVk* pCmdContext = (CStudioAPICmdContextVk*)pCmdList->GetCmdContext();
		pCmdContext->AddPendingImageBarriers( pCmdList, 1, &imageMemoryBarrier.vkImageMemoryBarrier, imageMemoryBarrier.vkSrcStageMask, imageMemoryBarrier.vkDstStageMask );
	}
}

/*
==================
CStudioAPISwapChainImageVk::GetSwapChain
==================
*/
IStudioAPISwapChain* CStudioAPISwapChainImageVk::GetSwapChain() const
{
	return pSwapChain;
}

/*
==================
CStudioAPISwapChainImageVk::GetIndex
==================
*/
uint32 CStudioAPISwapChainImageVk::GetIndex() const
{
	return imageIndex;
}

/*
==================
CStudioAPISwapChainVk::CStudioAPISwapChainVk
==================
*/
CStudioAPISwapChainVk::CStudioAPISwapChainVk( const char* pDebugName /* = "" */ )
	: bUseVSync( false )
	, windowHandle( INVALID_WINDOW_HANDLE )
	, vkSurface( VK_NULL_HANDLE )
	, vkSwapChain( VK_NULL_HANDLE )
	, size( 0, 0 )
	, currentImageIndex( 0 )
	, pStudioAPIVkShutdownDelegate( NULL )
{
	Mem_Memzero( pImageAvailableSemaphores, STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT * sizeof( CStudioAPISemaphoreVk* ) );
	Mem_Memzero( &vkSurfaceFormat, sizeof( VkSurfaceFormatKHR ) );
}

/*
==================
CStudioAPISwapChainVk::~CStudioAPISwapChainVk
==================
*/
CStudioAPISwapChainVk::~CStudioAPISwapChainVk()
{
	Destroy();
}

/*
==================
CStudioAPISwapChainVk::Create
==================
*/
bool CStudioAPISwapChainVk::Create( windowHandle_t windowHandle, uint32 width, uint32 height, VkFormat vkPixelFormat, VkColorSpaceKHR vkColorSpace, bool bUseVSync )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// If swap chain already created, we have to destroy the one
	bool bReCreate =
		vkSwapChain != VK_NULL_HANDLE
		&& CStudioAPISwapChainVk::windowHandle == windowHandle
		&& CStudioAPISwapChainVk::vkSurfaceFormat.format == vkPixelFormat
		&& CStudioAPISwapChainVk::vkSurfaceFormat.colorSpace == vkColorSpace;
	if ( IsCreated() && !bReCreate )
	{
		Destroy();
	}
	VkSurfaceFormatKHR vkOldSurfaceFormat = vkSurfaceFormat;

	// Create Vulkan surface in the window (only for non re-create case)
	if ( !bReCreate )
	{
		Assert( windowHandle );
		vkSurface = VK_Plat_CreateSurfaceKHR( g_StudioAPIVk.GetDevice().GetVkInstance(), windowHandle );
		if ( vkSurface == VK_NULL_HANDLE )
		{
			Error( "StudioAPIVk: Failed to create a surface in the window (0x%X)", windowHandle );
			Destroy();
			return false;
		}

		// Find supported surface format by current physical device
		{
			bool   bSurfaceFormatFound = false;
			uint32 surfaceFormatCount  = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR( g_StudioAPIVk.GetDevice().GetVkPhysicalDevice(), vkSurface, &surfaceFormatCount, NULL );
			std::vector<VkSurfaceFormatKHR> availableSurfaceFormats( surfaceFormatCount );
			vkGetPhysicalDeviceSurfaceFormatsKHR( g_StudioAPIVk.GetDevice().GetVkPhysicalDevice(), vkSurface, &surfaceFormatCount, availableSurfaceFormats.data() );

			// If exist in array only one item with format VK_FORMAT_UNDEFINED, it mean what we can select any format (we are lucky!)
			if ( availableSurfaceFormats.size() == 1 && availableSurfaceFormats[0].format == VK_FORMAT_UNDEFINED )
			{
				vkSurfaceFormat.format	   = vkPixelFormat;
				vkSurfaceFormat.colorSpace = vkColorSpace;
			}
			// Otherwise this day isn't ours, we have to find supported format and color space
			else
			{
				for ( uint32 index = 0; index < surfaceFormatCount; ++index )
				{
					// If this format is what we need, take!
					const VkSurfaceFormatKHR& currentSurfaceFormat = availableSurfaceFormats[index];
					if ( currentSurfaceFormat.format == vkPixelFormat && currentSurfaceFormat.colorSpace == vkColorSpace )
					{
						bSurfaceFormatFound = true;
						vkSurfaceFormat		= currentSurfaceFormat;
						break;
					}
				}
			}

			// Print all supported surface formats for debug
#if ENABLE_LOGGING
			Msg( "StudioAPIVk: Supported surface formats: %u", availableSurfaceFormats.size() );
			for ( uint32 index = 0; index < surfaceFormatCount; ++index )
			{
				const VkSurfaceFormatKHR& currentSurfaceFormat = availableSurfaceFormats[index];
				Msg( "StudioAPIVk:\t%u: %s(%u) %s(%u)", index,
					 VK_ConvVkFormatToText( currentSurfaceFormat.format ), currentSurfaceFormat.format,
					 VK_ConvVkColorSpaceToText( currentSurfaceFormat.colorSpace ), currentSurfaceFormat.colorSpace );
			}
#endif	// ENABLE_LOGGING

			// Fail, we not found format and color space which us need. In this case we take first available surface format
			if ( !bSurfaceFormatFound )
			{
				vkSurfaceFormat = availableSurfaceFormats[0];
				Warning( "StudioAPIVk: Failed to find requested surface format %s(%u) %s(%u), that's why we take %s(%u) %s(%u)",
						 VK_ConvVkFormatToText( vkPixelFormat ), vkPixelFormat,
						 VK_ConvVkColorSpaceToText( vkColorSpace ), vkColorSpace,
						 VK_ConvVkFormatToText( vkSurfaceFormat.format ), vkSurfaceFormat.format,
						 VK_ConvVkColorSpaceToText( vkSurfaceFormat.colorSpace ), vkSurfaceFormat.colorSpace );
			}
		}
	}

	// Get information about surface capabilities from current physical device
	VkSurfaceCapabilitiesKHR vkSurfaceCapabilities = {};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR( g_StudioAPIVk.GetDevice().GetVkPhysicalDevice(), vkSurface, &vkSurfaceCapabilities );

	// If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
	if ( vkSurfaceCapabilities.currentExtent.width == (uint32)-1 )
	{
		// If the surface size is undefined, the size is set to the size of the images requested
		vkSurfaceCapabilities.currentExtent.width  = width;
		vkSurfaceCapabilities.currentExtent.height = height;
	}
	else
	{
		// If the surface size is defined, the swap chain size must match
	}

	// Check on valid window size
	if ( vkSurfaceCapabilities.currentExtent.width <= 0 || vkSurfaceCapabilities.currentExtent.height <= 0 )
	{
		Error( "StudioAPIVk: Failed to create a swap chain, invalid window size (%ix%i) in the window (0x%X)", vkSurfaceCapabilities.currentExtent.width, vkSurfaceCapabilities.currentExtent.height, windowHandle );
		if ( !bReCreate )
		{
			Destroy();
		}

		size = ivec2_t( 0, 0 );
		return false;
	}

	// Remember image size
	size.x = vkSurfaceCapabilities.currentExtent.width;
	size.y = vkSurfaceCapabilities.currentExtent.height;

	// Get present mode which is supported by physical device
	// The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
	// This mode waits for the vertical blank (VSync)
	VkPresentModeKHR vkPresentMode = VK_PRESENT_MODE_FIFO_KHR;

	// If VSync is not requested, try to find a mailbox mode
	// It's the lowest latency non-tearing present mode available
	if ( !bUseVSync )
	{
		uint32 presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR( g_StudioAPIVk.GetDevice().GetVkPhysicalDevice(), vkSurface, &presentModeCount, NULL );
		std::vector<VkPresentModeKHR> availablePresentModes( presentModeCount );
		vkGetPhysicalDeviceSurfacePresentModesKHR( g_StudioAPIVk.GetDevice().GetVkPhysicalDevice(), vkSurface, &presentModeCount, availablePresentModes.data() );

		// Try to find supported present mode 'VK_PRESENT_MODE_MAILBOX_KHR'
		for ( uint32 index = 0; index < presentModeCount; ++index )
		{
			const VkPresentModeKHR& currentPresentMode = availablePresentModes[index];
			if ( currentPresentMode == VK_PRESENT_MODE_MAILBOX_KHR )  // Mailbox it's very good variant
			{
				vkPresentMode = currentPresentMode;
				break;
			}
			else if ( currentPresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR )	 // Immediate it's fallback variant
			{
				vkPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}

		// Print all available present modes for debug
#if ENABLE_LOGGING
		Msg( "StudioAPIVk: Supported surface present modes: %u", presentModeCount );
		for ( uint32 index = 0; index < presentModeCount; ++index )
		{
			const VkPresentModeKHR& currentPresentMode = availablePresentModes[index];
			Msg( "StudioAPIVk:\t%u: %s (%u)", index, VK_ConvVkPresentModeToText( currentPresentMode ), currentPresentMode );
		}
#endif	// ENABLE_LOGGING
	}
	Msg( "StudioAPIVk: Present mode for swap chain: %s (%u)", VK_ConvVkPresentModeToText( vkPresentMode ), vkPresentMode );

	// Remember VSync flag
	CStudioAPISwapChainVk::bUseVSync = bUseVSync;

	// Determine the number of images
	uint32 desiredNumberOfSwapchainImages = vkSurfaceCapabilities.minImageCount + 1;
	if ( vkSurfaceCapabilities.maxImageCount > 0 && desiredNumberOfSwapchainImages > vkSurfaceCapabilities.maxImageCount )
	{
		desiredNumberOfSwapchainImages = vkSurfaceCapabilities.maxImageCount;
	}

	// Find the transformation of the surface
	VkSurfaceTransformFlagsKHR vkPreTransform;
	if ( vkSurfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR )
	{
		// We prefer a non-rotated transform
		vkPreTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		vkPreTransform = vkSurfaceCapabilities.currentTransform;
	}

	// Create Vulkan swap chain
	VkSwapchainCreateInfoKHR vkSwapchainCreateInfo = {};
	vkSwapchainCreateInfo.sType					   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	vkSwapchainCreateInfo.surface				   = vkSurface;
	vkSwapchainCreateInfo.minImageCount			   = desiredNumberOfSwapchainImages;
	vkSwapchainCreateInfo.imageFormat			   = vkSurfaceFormat.format;
	vkSwapchainCreateInfo.imageColorSpace		   = vkSurfaceFormat.colorSpace;
	vkSwapchainCreateInfo.imageExtent			   = vkSurfaceCapabilities.currentExtent;
	vkSwapchainCreateInfo.imageArrayLayers		   = 1;
	vkSwapchainCreateInfo.imageUsage			   = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// If swapchain will be use in more one queues then we have to set sharing mode to VK_SHARING_MODE_CONCURRENT
	CStudioAPIQueueSharingModeSetupVk queueSharingModeSetup( g_StudioAPIVk.GetDevice().GetGraphicsQueue().GetQueueFamilyIndex(),
															 g_StudioAPIVk.GetDevice().GetPresentQueue().GetQueueFamilyIndex() );
	queueSharingModeSetup.Setup( vkSwapchainCreateInfo.imageSharingMode, vkSwapchainCreateInfo.queueFamilyIndexCount, vkSwapchainCreateInfo.pQueueFamilyIndices );

	vkSwapchainCreateInfo.preTransform	 = (VkSurfaceTransformFlagBitsKHR)vkPreTransform;
	vkSwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	vkSwapchainCreateInfo.presentMode	 = vkPresentMode;
	vkSwapchainCreateInfo.clipped		 = VK_TRUE;	 // Setting clipped to VK_TRUE allows the implementation to discard rendering outside of the surface area
	vkSwapchainCreateInfo.oldSwapchain	 = vkSwapChain;

	VkSwapchainKHR vkNewSwapChain = VK_NULL_HANDLE;
	STUDIOAPI_VK_VERIFY_RESULT( vkCreateSwapchainKHR( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkSwapchainCreateInfo, NULL, &vkNewSwapChain ) );

	// Destroy old Vulkan resources
	if ( bReCreate )
	{
		for ( uint32 swapChainImageIdx = 0, numSwapChainImages = (uint32)swapChainImages.size(); swapChainImageIdx < numSwapChainImages; ++swapChainImageIdx )
		{
			delete swapChainImages[swapChainImageIdx];
		}
		swapChainImages.clear();

		if ( vkSwapChain != VK_NULL_HANDLE )
		{
			vkDestroySwapchainKHR( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkSwapChain, NULL );
			vkSwapChain = VK_NULL_HANDLE;
		}
	}

	// Save the new Vulkan swap chain
	vkSwapChain = vkNewSwapChain;

	// Initialize all swap chain images
	uint32				 numImages = 0;
	std::vector<VkImage> vkImages;
	vkGetSwapchainImagesKHR( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkSwapChain, &numImages, NULL );
	vkImages.resize( numImages );
	vkGetSwapchainImagesKHR( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkSwapChain, &numImages, vkImages.data() );

	swapChainImages.resize( numImages );
	for ( uint32 imageIdx = 0; imageIdx < numImages; ++imageIdx )
	{
		swapChainImages[imageIdx] = new CStudioAPISwapChainImageVk( this, imageIdx, vkImages[imageIdx], vkSurfaceFormat );
	}

	CStudioAPISwapChainVk::windowHandle = windowHandle;
	currentImageIndex					= 0;
	Msg( "StudioAPIVk: Swap chain is created for window handle (0x%X), count images: %u", windowHandle, numImages );

	// Re-create semaphores
	renderFinishedSemaphores.resize( numImages );

	// To correct reusing swapchain semaphores we create semaphores the next way:
	// * Image available semaphore per frame in flight
	// * Render finished semaphore per swapchain image
	//
	// For more information:
	// https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html
	for ( uint32 imageAvailableSemaphoreIdx = 0; imageAvailableSemaphoreIdx < STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT; ++imageAvailableSemaphoreIdx )
	{
		CStudioAPISemaphoreVk*& pImageAvailableSemaphore = pImageAvailableSemaphores[imageAvailableSemaphoreIdx];
		if ( pImageAvailableSemaphore )
		{
			g_StudioAPIVk.GetSyncMgr().ReleaseSemaphore( pImageAvailableSemaphore );
		}
		pImageAvailableSemaphore = g_StudioAPIVk.GetSyncMgr().CreateSemaphore();
	}

	for ( uint32 renderFinishedSemaphoreIdx = 0; renderFinishedSemaphoreIdx < numImages; ++renderFinishedSemaphoreIdx )
	{
		CStudioAPISemaphoreVk*& pRenderFinishedSemaphores = renderFinishedSemaphores[renderFinishedSemaphoreIdx];
		if ( pRenderFinishedSemaphores )
		{
			g_StudioAPIVk.GetSyncMgr().ReleaseSemaphore( pRenderFinishedSemaphores );
		}
		pRenderFinishedSemaphores = g_StudioAPIVk.GetSyncMgr().CreateSemaphore();
	}

	// Broadcast about re-create event
	onReCreated.Broadcast( this, vkSurfaceFormat.format != vkOldSurfaceFormat.format );

	// Register in 'onStudioDeviceVkShutodwn' for destroy Vulkan objects when the one is shutdown (only for non re-create case)
	if ( !bReCreate )
	{
		pStudioAPIVkShutdownDelegate = g_StudioAPIVk.OnStudioAPIVkShutdown().AddFunc( &CStudioAPISwapChainVk::OnStudioAPIVkShutdown, this );
	}
	return true;
}

/*
==================
CStudioAPISwapChainVk::Destroy
==================
*/
void CStudioAPISwapChainVk::Destroy()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Free Vulkan resources
	g_StudioAPIVk.GetMemoryMgr().FreeResource( [vkSwapChain = vkSwapChain, vkSurface = vkSurface]() {
		// Destroy the Vulkan swap chain
		if ( vkSwapChain != VK_NULL_HANDLE )
		{
			vkDestroySwapchainKHR( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkSwapChain, NULL );
		}

		// Destroy the Vulkan surface
		if ( vkSurface != VK_NULL_HANDLE )
		{
			vkDestroySurfaceKHR( g_StudioAPIVk.GetDevice().GetVkInstance(), vkSurface, NULL );
		}
	} );
	// Destroy swap chain images
	for ( uint32 swapChainImageIdx = 0, numSwapChainImages = (uint32)swapChainImages.size(); swapChainImageIdx < numSwapChainImages; ++swapChainImageIdx )
	{
		delete swapChainImages[swapChainImageIdx];
	}
	swapChainImages.clear();

	// Release all semaphores and fences
	for ( uint32 index = 0; index < STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT; ++index )
	{
		g_StudioAPIVk.GetSyncMgr().ReleaseSemaphore( pImageAvailableSemaphores[index] );
	}
	Mem_Memzero( pImageAvailableSemaphores, STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT * sizeof( CStudioAPISemaphoreVk* ) );

	for ( uint32 index = 0, count = (uint32)renderFinishedSemaphores.size(); index < count; ++index )
	{
		g_StudioAPIVk.GetSyncMgr().ReleaseSemaphore( renderFinishedSemaphores[index] );
	}
	renderFinishedSemaphores.clear();

	// Reset values
	Mem_Memzero( &vkSurfaceFormat, sizeof( VkSurfaceFormatKHR ) );
	size			  = ivec2_t( 0, 0 );
	currentImageIndex = 0;
	windowHandle	  = INVALID_WINDOW_HANDLE;
	bUseVSync		  = false;

	if ( pStudioAPIVkShutdownDelegate )
	{
		g_StudioAPIVk.OnStudioAPIVkShutdown().RemoveFunc( pStudioAPIVkShutdownDelegate );
		pStudioAPIVkShutdownDelegate = NULL;
	}
}

/*
==================
CStudioAPISwapChainVk::Resize
==================
*/
void CStudioAPISwapChainVk::Resize( uint32 width, uint32 height )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Save the new size
	size.x = width;
	size.y = height;

	// Re-create the swap chain
	if ( !ReCreate() )
	{
		Error( "StudioAPIVk: Failed to re-create the swap chain with size %ix%i", width, height );
		Destroy();
		return;
	}
}

/*
==================
CStudioAPISwapChainVk::AcquireNextImage
==================
*/
bool CStudioAPISwapChainVk::AcquireNextImage()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Acquire the next image only if the swap chain is created
	Assert( IsCreated() );

	// Get the index of the next swap chain image we should render to
	// We'll wait with an "infinite" timeout, the function will block until an image is ready
	// The imageAvailableSemaphores[g_StudioAPIVk.GetCurrentFrameInFlight()] will get signaled when the image is ready (upon function return)
	uint32		 imageIndex	   = 0;
	const uint32 maxImageIndex = GetNumImages() - 1;

	// Get the current image available semaphore
	CStudioAPISemaphoreVk* pImageAvailableSemaphore = GetImageAvailableSemaphore();

	// Acquire the next image
	VkResult vkResult = vkAcquireNextImageKHR(
		g_StudioAPIVk.GetDevice().GetVkLogicalDevice(),
		vkSwapChain,
		UINT64_MAX,
		pImageAvailableSemaphore->GetVkSemaphore(),
		VK_NULL_HANDLE,
		&imageIndex );

	// The swapchain may have more images than we have requested on creating it. Ignore all extra images
	while ( imageIndex > maxImageIndex && ( vkResult == VK_SUCCESS || vkResult == VK_SUBOPTIMAL_KHR ) )
	{
		vkResult = vkAcquireNextImageKHR(
			g_StudioAPIVk.GetDevice().GetVkLogicalDevice(),
			vkSwapChain,
			UINT64_MAX,
			pImageAvailableSemaphore->GetVkSemaphore(),
			VK_NULL_HANDLE,
			&imageIndex );
	}

	// Revert semaphore index if the image is out of date
	if ( vkResult == VK_ERROR_OUT_OF_DATE_KHR )
	{
		return false;
	}
	// Other result except VK_SUBOPTIMAL_KHR is critical
	else if ( vkResult != VK_SUBOPTIMAL_KHR )
	{
		STUDIOAPI_VK_VERIFY_RESULT( vkResult );
	}
	currentImageIndex = imageIndex;

	// We are done
	pImageAvailableSemaphore->Signal();
	return true;
}

/*
==================
CStudioAPISwapChainVk::ReCreate
==================
*/
bool CStudioAPISwapChainVk::ReCreate()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Re-create the swap chain
	vkDeviceWaitIdle( g_StudioAPIVk.GetDevice().GetVkLogicalDevice() );
	bool bResult = Create( windowHandle, size.x, size.y, vkSurfaceFormat.format, vkSurfaceFormat.colorSpace, bUseVSync );
	vkDeviceWaitIdle( g_StudioAPIVk.GetDevice().GetVkLogicalDevice() );
	return bResult;
}

/*
==================
CStudioAPISwapChainVk::Present
==================
*/
bool CStudioAPISwapChainVk::Present()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Present the frame if the swap chain is created
	Assert( IsCreated() );

	// Get the current render finished semaphore
	CStudioAPISemaphoreVk* pRenderFinishedSemaphore = GetRenderFinishedSemaphore();
	Assert( pRenderFinishedSemaphore->IsSignaled() );

	// Present the back buffer with synchronization of render finished signal
	VkSemaphore		 vkPresentWaitSemaphore = pRenderFinishedSemaphore->GetVkSemaphore();
	VkPresentInfoKHR vkPresentInfo			= {};
	vkPresentInfo.sType						= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	vkPresentInfo.pWaitSemaphores			= &vkPresentWaitSemaphore;
	vkPresentInfo.waitSemaphoreCount		= 1;
	vkPresentInfo.pSwapchains				= &vkSwapChain;
	vkPresentInfo.swapchainCount			= 1;
	vkPresentInfo.pImageIndices				= &currentImageIndex;
	vkPresentInfo.pResults					= NULL;
	VkResult vkResult						= vkQueuePresentKHR( g_StudioAPIVk.GetDevice().GetPresentQueue().GetVkQueue(), &vkPresentInfo );
	pRenderFinishedSemaphore->Unsignal();

	// Exit from the method if the swap chain is out of date
	if ( vkResult == VK_ERROR_OUT_OF_DATE_KHR )
	{
		return false;
	}
	// Other result except VK_SUBOPTIMAL_KHR is critical
	else if ( vkResult != VK_SUBOPTIMAL_KHR )
	{
		STUDIOAPI_VK_VERIFY_RESULT( vkResult );
	}

	// We are done
	return true;
}

/*
==================
CStudioAPISwapChainVk::GetSize
==================
*/
ivec2_t CStudioAPISwapChainVk::GetSize() const
{
	return size;
}

/*
==================
CStudioAPISwapChainVk::GetCurrentImageIndex
==================
*/
uint32 CStudioAPISwapChainVk::GetCurrentImageIndex() const
{
	return currentImageIndex;
}

/*
==================
CStudioAPISwapChainVk::GetCurrentImage
==================
*/
IStudioAPISwapChainImage* CStudioAPISwapChainVk::GetCurrentImage() const
{
	if ( !swapChainImages.empty() )
	{
		return (IStudioAPISwapChainImage*)swapChainImages[currentImageIndex];
	}
	return NULL;
}

/*
==================
CStudioAPISwapChainVk::GetNumImages
==================
*/
uint32 CStudioAPISwapChainVk::GetNumImages() const
{
	return (uint32)swapChainImages.size();
}

/*
==================
CStudioAPISwapChainVk::GetImage
==================
*/
IStudioAPISwapChainImage* CStudioAPISwapChainVk::GetImage( uint32 index ) const
{
	Assert( index < (uint32)swapChainImages.size() );
	return (IStudioAPISwapChainImage*)swapChainImages[index];
}

/*
==================
CStudioAPISwapChainVk::GetWindowHandle
==================
*/
windowHandle_t CStudioAPISwapChainVk::GetWindowHandle() const
{
	return windowHandle;
}

/*
==================
CStudioAPISwapChainVk::OnStudioAPIVkShutdown
==================
*/
void CStudioAPISwapChainVk::OnStudioAPIVkShutdown( void* pUserData )
{
	Assert( pUserData );
	CStudioAPISwapChainVk* pStudioAPISwapChain		  = (CStudioAPISwapChainVk*)pUserData;
	pStudioAPISwapChain->pStudioAPIVkShutdownDelegate = NULL;
	pStudioAPISwapChain->Destroy();
}

/*
==================
CStudioAPISwapChainVk::OnReCreated
==================
*/
IStudioAPISwapChain::IOnReCreated* CStudioAPISwapChainVk::OnReCreated() const
{
	return (IOnReCreated*)&onReCreated;
}

/*
==================
CStudioAPISwapChainVk::IsUseVSync
==================
*/
bool CStudioAPISwapChainVk::IsUseVSync() const
{
	return bUseVSync;
}

/*
==================
CStudioAPISwapChainVk::IsValid
==================
*/
bool CStudioAPISwapChainVk::IsValid() const
{
	return IsCreated();
}