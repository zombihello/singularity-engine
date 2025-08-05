/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef WIN_VK_PLATFORM_H
#define WIN_VK_PLATFORM_H

#include "stdlib/defines.h"
#include "stdlib/types.h"
#include "stdlib/template.h"
#include "Volk/volk.h"

/**
 * @ingroup studioapi_vk
 * @brief Create Vulkan surface in window
 *
 * @param vkInstance	Vulkan instance
 * @param windowHandle	Window handle
 * @return Return create Vulkan surface, if failed return VK_NULL_HANDLE
 */
FORCEINLINE VkSurfaceKHR VK_Plat_CreateSurfaceKHR( VkInstance vkInstance, windowHandle_t windowHandle )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	Assert( windowHandle && vkCreateWin32SurfaceKHR );
	VkSurfaceKHR	vkSurface;

	// Create Vulkan surface for Windows
	VkWin32SurfaceCreateInfoKHR				vkWin32SurfaceCreateInfoKHR = {};
	vkWin32SurfaceCreateInfoKHR.sType		= VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	vkWin32SurfaceCreateInfoKHR.hwnd		= ( HWND )windowHandle;
	vkWin32SurfaceCreateInfoKHR.hinstance	= GetModuleHandle( NULL );
	vkWin32SurfaceCreateInfoKHR.flags		= 0x0;
	vkWin32SurfaceCreateInfoKHR.pNext		= NULL;

	if ( vkCreateWin32SurfaceKHR( vkInstance, &vkWin32SurfaceCreateInfoKHR, NULL, &vkSurface ) != VK_SUCCESS )
	{
		Warning( "StudioAPIVk: Failed to create Vulkan surface in window handle 0x%X\n", windowHandle );
		return VK_NULL_HANDLE;
	}

	return vkSurface;
}

/**
 * @ingroup studioapi_vk
 * @brief Get required instance extensions
 * 
 * @param size	Output array size with required instance extensions
 * @return Return array with required instance extensions
 */
FORCEINLINE const achar** VK_Plat_GetRequiredInstanceExtensions( uint32& size )
{
	// List of required instance extensions for platform
	static const achar*		s_pPlatInstanceExtensions[] =
	{
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
	};

	size = ARRAYSIZE( s_pPlatInstanceExtensions );
	return s_pPlatInstanceExtensions;
}

/**
 * @ingroup studioapi_vk
 * @brief Is supported surface by physical device
 *
 * @param vkPhysicalDevice	Vulkan physical device
 * @param queueFamilyIndex	Queue family index
 * @return Return TRUE if supported surface by physical device, otherwise return FALSE
 */
FORCEINLINE bool VK_Plat_IsPhysicalDeviceSurfaceSupport( VkPhysicalDevice vkPhysicalDevice, uint32 queueFamilyIndex )
{
	Assert( vkGetPhysicalDeviceWin32PresentationSupportKHR );
	return vkGetPhysicalDeviceWin32PresentationSupportKHR( vkPhysicalDevice, queueFamilyIndex );
}

#endif // !WIN_VK_PLATFORM_H