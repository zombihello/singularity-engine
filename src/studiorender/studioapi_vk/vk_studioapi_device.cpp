#include "pch_studioapi_vk.h"
#include "tier0/icommandline.h"
#include "studiorender/studioapi_vk/vk_studioapi_device.h"
#include "studiorender/studioapi_vk/vk_helpers.h"

// ----------------
// GLOBALS
// ----------------
#if !RETAIL
// List of validation layers
static const char* s_pValidationLayers[] = {
	"VK_LAYER_KHRONOS_validation"
};
#endif	// !RETAIL

// List of requesting instance extensions
static const char* s_pInstanceExtensions[] = {
	VK_KHR_SURFACE_EXTENSION_NAME
};

// List of requesting device extensions
static const char* s_pDeviceExtensions[] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

/*
==================
VK_TranslateGPUVendorId
==================
*/
static FORCEINLINE studioAPIGPUVendorId_t VK_TranslateGPUVendorId( uint32 gpuVendorId )
{
	switch ( (studioAPIGPUVendorId_t)gpuVendorId )
	{
	case STUDIOAPI_GPU_VENDOR_ID_NOT_QUERIED:
		return STUDIOAPI_GPU_VENDOR_ID_NOT_QUERIED;

	case STUDIOAPI_GPU_VENDOR_ID_AMD:
	case STUDIOAPI_GPU_VENDOR_ID_MESA:
	case STUDIOAPI_GPU_VENDOR_ID_IMGTEC:
	case STUDIOAPI_GPU_VENDOR_ID_NVIDIA:
	case STUDIOAPI_GPU_VENDOR_ID_ARM:
	case STUDIOAPI_GPU_VENDOR_ID_BROADCOM:
	case STUDIOAPI_GPU_VENDOR_ID_QUALCOMM:
	case STUDIOAPI_GPU_VENDOR_ID_INTEL:
	case STUDIOAPI_GPU_VENDOR_ID_SAMSUNG_AMD:
	case STUDIOAPI_GPU_VENDOR_ID_MICROSOFT:
		return (studioAPIGPUVendorId_t)gpuVendorId;
	}
	return STUDIOAPI_GPU_VENDOR_ID_UNKNOWN;
}

/*
==================
CStudioAPIDeviceVk::CStudioAPIDeviceVk
==================
*/
CStudioAPIDeviceVk::CStudioAPIDeviceVk()
	: vkInstance( VK_NULL_HANDLE )
	, vkPhysicalDevice( VK_NULL_HANDLE )
	, vkLogicalDevice( VK_NULL_HANDLE )
	, gpuVendorId( STUDIOAPI_GPU_VENDOR_ID_UNKNOWN )
#if !RETAIL
	, bDebug( false )
	, vkDebugMessenger( VK_NULL_HANDLE )
#endif	// !RETAIL
{
	Mem_Memzero( &vkMemoryInfo, sizeof( VkPhysicalDeviceMemoryProperties ) );
	Mem_Memzero( &vkDeviceInfo, sizeof( VkPhysicalDeviceProperties ) );
}

/*
==================
CStudioAPIDeviceVk::~CStudioAPIDeviceVk
==================
*/
CStudioAPIDeviceVk::~CStudioAPIDeviceVk()
{
	Shutdown();
}

/*
==================
CStudioAPIDeviceVk::Init
==================
*/
void CStudioAPIDeviceVk::Init( uint32 engineMajorVersion, uint32 engineMinorVersion, uint32 enginePatchVersion )
{
	// Try to load Vulkan loader
	if ( volkInitialize() == VK_ERROR_INITIALIZATION_FAILED )
	{
		Sys_Error( "Failed to load Vulkan functions" );
		return;
	}

	// Print to log Vulkan version
	uint32 vkVersion = 0;
	if ( vkEnumerateInstanceVersion )
	{
		vkEnumerateInstanceVersion( &vkVersion );
	}
	else
	{
		vkVersion = VK_MAKE_API_VERSION( 0, 1, 0, 0 );
	}
	Msg( "StudioAPIVk: Vulkan %u.%u.%u.%u", VK_API_VERSION_VARIANT( vkVersion ), VK_API_VERSION_MAJOR( vkVersion ), VK_API_VERSION_MINOR( vkVersion ), VK_API_VERSION_PATCH( vkVersion ) );

	// Init struct of info about application
	VkApplicationInfo vkAppInfo	 = {};
	vkAppInfo.sType				 = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vkAppInfo.pApplicationName	 = "";
	vkAppInfo.pEngineName		 = "Singularity Engine";
	vkAppInfo.applicationVersion = VK_MAKE_API_VERSION( 0, 1, 0, 0 );
	vkAppInfo.engineVersion		 = PACK_VERSION( engineMajorVersion, engineMinorVersion, enginePatchVersion );
	vkAppInfo.apiVersion		 = VK_API_VERSION_1_3;

	// Init struct of info about creating instance
	VkInstanceCreateInfo vkInstanceCreateInfo = {};
	vkInstanceCreateInfo.sType				  = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vkInstanceCreateInfo.pApplicationInfo	  = &vkAppInfo;

	// Create list of required instance extensions
	eastl::vector<const char*> extensionsRequired;
	for ( uint32 index = 0, count = ARRAYSIZE( s_pInstanceExtensions ); index < count; ++index )
	{
		extensionsRequired.push_back( s_pInstanceExtensions[index] );
	}

	// Add to list required instance extensions of current platform
	uint32		 platformExtensionCount		= 0;
	const char** pPatformExtensionsRequired = VK_Plat_GetRequiredInstanceExtensions( platformExtensionCount );
	for ( uint32 index = 0; index < platformExtensionCount; ++index )
	{
		extensionsRequired.push_back( pPatformExtensionsRequired[index] );
	}

	// If application started with -vkdebug then we request VK_EXT_DEBUG_UTILS_EXTENSION_NAME (Only for non-retail build)
#if !RETAIL
	bool bVkDebug = CommandLine()->HasParam( "vkdebug" ) || DEBUG;
	if ( bVkDebug )
	{
		extensionsRequired.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
		bDebug = true;
	}
#endif	// !RETAIL

	Msg( "StudioAPIVk: Requested instance extensions: %i", extensionsRequired.size() );
	for ( uint32 index = 0, count = (uint32)extensionsRequired.size(); index < count; ++index )
	{
		Msg( "StudioAPIVk:\t%i: %s", index, extensionsRequired[index] );
	}

	// ONLY FOR NON-RETAIL BUILD!
	// Debug create info struct
#if !RETAIL
	VkDebugUtilsMessengerCreateInfoEXT vkDebugCreateInfo			= {};
	VkValidationFeatureEnableEXT	   vkValidationFeatureEnables[] = { VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT };
	VkValidationFeaturesEXT			   vkValidationFeatures			= {};
#endif	// !RETAIL

	// Check required extensions and add they to vkInstanceCreateInfo
	if ( !extensionsRequired.empty() )
	{
		if ( !VK_CheckInstanceExtensionsSupported( extensionsRequired.data(), (uint32)extensionsRequired.size() ) )
		{
			Sys_Error( "One or more required instance extensions not supported" );
			return;
		}

		vkInstanceCreateInfo.ppEnabledExtensionNames = extensionsRequired.data();
		vkInstanceCreateInfo.enabledExtensionCount	 = (uint32)extensionsRequired.size();

		// ONLY FOR NON-RETAIL BUILD!
		// We check is supported required validation layers and in successfully case
		// we add they to vkInstanceCreateInfo
#if !RETAIL
		if ( bVkDebug )
		{
			Msg( "StudioAPIVk: Requested validation layers: %i", ARRAYSIZE( s_pValidationLayers ) );
			for ( uint32 index = 0, count = ARRAYSIZE( s_pValidationLayers ); index < count; ++index )
			{
				Msg( "StudioAPIVk:\t%i: %s", index, s_pValidationLayers[index] );
			}

			if ( VK_CheckValidationLayersSupported( s_pValidationLayers, ARRAYSIZE( s_pValidationLayers ) ) )
			{
				vkDebugCreateInfo.sType			  = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				vkDebugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				vkDebugCreateInfo.messageType	  = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				vkDebugCreateInfo.pfnUserCallback = VK_DebugCallback;

				vkValidationFeatures.sType						   = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
				vkValidationFeatures.enabledValidationFeatureCount = ARRAYSIZE( vkValidationFeatureEnables );
				vkValidationFeatures.pEnabledValidationFeatures	   = vkValidationFeatureEnables;
				vkValidationFeatures.pNext						   = &vkDebugCreateInfo;

				vkInstanceCreateInfo.ppEnabledLayerNames = s_pValidationLayers;
				vkInstanceCreateInfo.enabledLayerCount	 = ARRAYSIZE( s_pValidationLayers );
				vkInstanceCreateInfo.pNext				 = &vkValidationFeatures;
				Warning( "StudioAPIVk: Using Vulkan validation layers, expect severely degraded performance" );
			}
			else
			{
				Warning( "StudioAPIVk: Validation layers requested, but not available" );
			}
		}
#endif	// !RETAIL
	}

	// Try to create vulkan instance
	STUDIOAPI_VK_VERIFY_RESULT( vkCreateInstance( &vkInstanceCreateInfo, NULL, &vkInstance ) );
	Msg( "StudioAPIVk: Instance successfully created" );

	// Load all supported extensions
	volkLoadInstance( vkInstance );

	// ONLY FOR NON-RETAIL BUILD!
	// Init debug report callback
#if !RETAIL
	if ( bVkDebug )
	{
		VkResult vkResult = vkCreateDebugUtilsMessengerEXT( vkInstance, &vkDebugCreateInfo, NULL, &vkDebugMessenger );
		if ( vkResult != VK_SUCCESS )
		{
			Warning( "StudioAPIVk: Failed to create debug messenger, vkResult: %s", VK_ConvVkResultToText( vkResult ) );
		}
	}
#endif	// !RETAIL

	// Create Vulkan device
	CreateVkDevice();
}

/*
==================
CVkDevice::CreateVkDevice
==================
*/
void CStudioAPIDeviceVk::CreateVkDevice()
{
	uint32 physicalDevicesCount = 0;
	vkEnumeratePhysicalDevices( vkInstance, &physicalDevicesCount, NULL );

	// If number of physical devices is zero it's mean what in system not exist GPUs who supported Vulkan
	if ( physicalDevicesCount == 0 )
	{
		Sys_Error( "Not found any GPU that supports Vulkan" );
		return;
	}

	// Info about physical device who we will selected and graphics family index
	VkPhysicalDeviceProperties vkPhysDeviceProperties;
	VkPhysicalDeviceFeatures   vkPhysDeviceFeatures;
	queueFamilyIndices_t	   queueFamilyIndices;

	// Get all list of physical devices
	eastl::vector<VkPhysicalDevice> physicalDevices( physicalDevicesCount );
	vkEnumeratePhysicalDevices( vkInstance, &physicalDevicesCount, physicalDevices.data() );

	Msg( "StudioAPIVk: Have %u devices:", physicalDevicesCount );

	// Looking for a device that suits us better
	for ( uint32 index = 0, count = (uint32)physicalDevices.size(); index < count; ++index )
	{
		// Ger properties and features of the physical device
		const VkPhysicalDevice& vkPhysicalDevice = physicalDevices[index];
		vkGetPhysicalDeviceProperties( vkPhysicalDevice, &vkPhysDeviceProperties );
		vkGetPhysicalDeviceFeatures( vkPhysicalDevice, &vkPhysDeviceFeatures );

		Msg( "StudioAPIVk:\t%u: %04x:%04x %d %s %u.%u.%u.%u %u.%u.%u.%u",
			 index, vkPhysDeviceProperties.vendorID, vkPhysDeviceProperties.deviceID, vkPhysDeviceProperties.deviceType, vkPhysDeviceProperties.deviceName,
			 VK_API_VERSION_VARIANT( vkPhysDeviceProperties.driverVersion ), VK_API_VERSION_MAJOR( vkPhysDeviceProperties.driverVersion ), VK_API_VERSION_MINOR( vkPhysDeviceProperties.driverVersion ), VK_API_VERSION_PATCH( vkPhysDeviceProperties.driverVersion ),
			 VK_API_VERSION_VARIANT( vkPhysDeviceProperties.apiVersion ), VK_API_VERSION_MAJOR( vkPhysDeviceProperties.apiVersion ), VK_API_VERSION_MINOR( vkPhysDeviceProperties.apiVersion ), VK_API_VERSION_PATCH( vkPhysDeviceProperties.apiVersion ) );

		// Check on supporting extensions which us need by the physical device
		if ( !VK_CheckDeviceExtensionsSupported( vkPhysicalDevice, s_pDeviceExtensions, ARRAYSIZE( s_pDeviceExtensions ) ) )
		{
			Msg( "StudioAPIVk:\t\tSkipping this device as requested extensions not supported" );
			continue;
		}

		// Find queue family indices that us need
		uint32 queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties( vkPhysicalDevice, &queueFamilyCount, NULL );
		eastl::vector<VkQueueFamilyProperties> vkQueueFamiliesProperties( queueFamilyCount );
		vkGetPhysicalDeviceQueueFamilyProperties( vkPhysicalDevice, &queueFamilyCount, vkQueueFamiliesProperties.data() );
		queueFamilyIndices = FindQueueFamilyIndices( vkPhysicalDevice, vkQueueFamiliesProperties,
													 STUDIOAPI_VK_QUEUE_FLAG_GRAPHICS | STUDIOAPI_VK_QUEUE_FLAG_PRESENT | STUDIOAPI_VK_QUEUE_FLAG_TRANSFER | STUDIOAPI_VK_QUEUE_FLAG_COMPUTE );

		// Print information about all queue families
#if ENABLE_LOGGING
		Msg( "StudioAPIVk:\t\tQueues count: %i", queueFamilyCount );
		for ( uint32 indexQueueFamily = 0; indexQueueFamily < queueFamilyCount; ++indexQueueFamily )
		{
			const VkQueueFamilyProperties& vkQueueFamilyProperties = vkQueueFamiliesProperties[indexQueueFamily];
			Msg( "StudioAPIVk:\t\t\tQueue %i/%i: count %i graphics %i present %i transfer %i compute %i",
				 indexQueueFamily + 1, queueFamilyCount,
				 vkQueueFamilyProperties.queueCount,
				 !!( vkQueueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT ),
				 VK_Plat_IsPhysicalDeviceSurfaceSupport( vkPhysicalDevice, indexQueueFamily ),
				 !!( vkQueueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT ),
				 !!( vkQueueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT ) );
		}
#endif	// ENABLE_LOGGING

		// Queue families not found and we skipping this device
		if ( !queueFamilyIndices.IsValid( STUDIOAPI_VK_QUEUE_FLAG_GRAPHICS | STUDIOAPI_VK_QUEUE_FLAG_PRESENT | STUDIOAPI_VK_QUEUE_FLAG_TRANSFER | STUDIOAPI_VK_QUEUE_FLAG_COMPUTE ) )
		{
			Msg( "StudioAPIVk:\t\tSkipping this device as compatible queue not found" );
			continue;
		}

		// Print extensions which is supported by physical device
#if ENABLE_LOGGING
		{
			uint32 deviceExtensionCount = 0;
			vkEnumerateDeviceExtensionProperties( vkPhysicalDevice, NULL, &deviceExtensionCount, NULL );
			eastl::vector<VkExtensionProperties> deviceExtensions( deviceExtensionCount );
			vkEnumerateDeviceExtensionProperties( vkPhysicalDevice, NULL, &deviceExtensionCount, deviceExtensions.data() );

			Msg( "StudioAPIVk:\t\tSupported device extensions: %u", deviceExtensionCount );
			for ( uint32 indexDeviceExt = 0; indexDeviceExt < deviceExtensionCount; ++indexDeviceExt )
			{
				const VkExtensionProperties& vkExtProperties = deviceExtensions[indexDeviceExt];
				Msg( "StudioAPIVk:\t\t\t%s: %u.%u.%u.%u", vkExtProperties.extensionName,
					 VK_API_VERSION_VARIANT( vkExtProperties.specVersion ), VK_API_VERSION_MAJOR( vkExtProperties.specVersion ), VK_API_VERSION_MINOR( vkExtProperties.specVersion ), VK_API_VERSION_PATCH( vkExtProperties.specVersion ) );
			}
		}
#endif	// ENABLE_LOGGING

		// TODO yehor.pohuliaka - Implement score of physical device to select more best variant (e.g: which have max image dimension, device type and other)

		// We found physical device which we need
		CStudioAPIDeviceVk::vkPhysicalDevice = vkPhysicalDevice;
		vkDeviceInfo						 = vkPhysDeviceProperties;
		gpuVendorId							 = VK_TranslateGPUVendorId( vkPhysDeviceProperties.vendorID );
		break;
	}

	// If physical device is not found, we exit with error
	if ( vkPhysicalDevice == VK_NULL_HANDLE )
	{
		Sys_Error( "Not found any compatible GPU" );
		return;
	}
	Msg( "StudioAPIVk: Selected GPU: %s", vkPhysDeviceProperties.deviceName );
	Msg( "StudioAPIVk: Selected queue family indices:" );
	Msg( "StudioAPIVk:\tGraphics: %i", queueFamilyIndices.IsValid( STUDIOAPI_VK_QUEUE_FLAG_GRAPHICS ) ? queueFamilyIndices.graphicsFamilyIndex : -1 );
	Msg( "StudioAPIVk:\tPresent: %i", queueFamilyIndices.IsValid( STUDIOAPI_VK_QUEUE_FLAG_PRESENT ) ? queueFamilyIndices.presentFamilyIndex : -1 );
	Msg( "StudioAPIVk:\tTransfer: %i", queueFamilyIndices.IsValid( STUDIOAPI_VK_QUEUE_FLAG_TRANSFER ) ? queueFamilyIndices.transferFamilyIndex : -1 );
	Msg( "StudioAPIVk:\tCompute: %i", queueFamilyIndices.IsValid( STUDIOAPI_VK_QUEUE_FLAG_COMPUTE ) ? queueFamilyIndices.computeFamilyIndex : -1 );

	// Get information about the physical device memory
	vkGetPhysicalDeviceMemoryProperties( vkPhysicalDevice, &vkMemoryInfo );

	// Print information about GPU memory size
	uint64 totalVideoMemory = 0;
	for ( uint32 heapIdx = 0; heapIdx < vkMemoryInfo.memoryHeapCount; ++heapIdx )
	{
		if ( vkMemoryInfo.memoryHeaps[heapIdx].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT )
		{
			totalVideoMemory += vkMemoryInfo.memoryHeaps[heapIdx].size;
		}
	}
	Msg( "StudioAPIVk:\tThe adapter has %lluMB of video memory", totalVideoMemory / ( 1024 * 1024 ) );

	// Otherwise we found physical device and try create logical device
	// Init descriptor device queue infos
	eastl::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	eastl::set<uint32>					   uniqueQueueFamilies = {
		queueFamilyIndices.graphicsFamilyIndex,
		queueFamilyIndices.presentFamilyIndex,
		queueFamilyIndices.transferFamilyIndex,
		queueFamilyIndices.computeFamilyIndex
	};

	float queuePriority = 1.f;
	for ( auto it = uniqueQueueFamilies.begin(), itEnd = uniqueQueueFamilies.end(); it != itEnd; ++it )
	{
		VkDeviceQueueCreateInfo vkQueueCreateInfo = {};
		vkQueueCreateInfo.sType					  = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		vkQueueCreateInfo.queueFamilyIndex		  = *it;
		vkQueueCreateInfo.queueCount			  = 1;
		vkQueueCreateInfo.pQueuePriorities		  = &queuePriority;
		queueCreateInfos.push_back( vkQueueCreateInfo );
	}

	// Init descriptor device features
	VkPhysicalDeviceFeatures vkDeviceFeatures = {};
#if !RETAIL	 // Feature 'fill mode non solid' need only for wireframe
	vkDeviceFeatures.fillModeNonSolid = VK_TRUE;
#endif	// !RETAIL
	vkDeviceFeatures.geometryShader		= VK_TRUE;
	vkDeviceFeatures.tessellationShader = VK_TRUE;
	vkDeviceFeatures.samplerAnisotropy	= VK_TRUE;

	// Init descriptor create logical device
	VkDeviceCreateInfo vkDeviceCreateInfo	   = {};
	vkDeviceCreateInfo.sType				   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	vkDeviceCreateInfo.pQueueCreateInfos	   = queueCreateInfos.data();
	vkDeviceCreateInfo.queueCreateInfoCount	   = (uint32)queueCreateInfos.size();
	vkDeviceCreateInfo.pEnabledFeatures		   = &vkDeviceFeatures;
	vkDeviceCreateInfo.ppEnabledExtensionNames = s_pDeviceExtensions;
	vkDeviceCreateInfo.enabledExtensionCount   = ARRAYSIZE( s_pDeviceExtensions );

	// It need for backwards compatible with the old Vulkan drivers
#if !RETAIL
	if ( vkDebugMessenger != VK_NULL_HANDLE )
	{
		vkDeviceCreateInfo.ppEnabledLayerNames = s_pValidationLayers;
		vkDeviceCreateInfo.enabledLayerCount   = ARRAYSIZE( s_pValidationLayers );
	}
	else
#endif	// !RETAIL
	{
		vkDeviceCreateInfo.enabledLayerCount = 0;
	}

	Msg( "StudioAPIVk: Requested device extensions: %i", vkDeviceCreateInfo.enabledExtensionCount );
	for ( uint32 index = 0; index < vkDeviceCreateInfo.enabledExtensionCount; ++index )
	{
		Msg( "StudioAPIVk:\t%i: %s", index, s_pDeviceExtensions[index] );
	}

	// Try create the logical device
	STUDIOAPI_VK_VERIFY_RESULT( vkCreateDevice( vkPhysicalDevice, &vkDeviceCreateInfo, NULL, &vkLogicalDevice ) );

	// Initialize queues
	graphicsQueue.Init( queueFamilyIndices.graphicsFamilyIndex, queueFamilyIndices.graphicsFamilyProperties );
	presentQueue.Init( queueFamilyIndices.presentFamilyIndex, queueFamilyIndices.presentFamilyProperties );
	transferQueue.Init( queueFamilyIndices.transferFamilyIndex, queueFamilyIndices.transferFamilyProperties );
	computeQueue.Init( queueFamilyIndices.computeFamilyIndex, queueFamilyIndices.computeFamilyProperties );
}

/*
==================
CStudioAPIDeviceVk::FindQueueFamilyIndices
==================
*/
CStudioAPIDeviceVk::queueFamilyIndices_t CStudioAPIDeviceVk::FindQueueFamilyIndices( const VkPhysicalDevice& vkPhysicalDevice, const eastl::vector<VkQueueFamilyProperties>& vkQueueFamiliesProperties, uint32 queueFamilyTypes ) const
{
	// Dedicated queue for compute
	// Try to find a queue family index that supports compute but not graphics
	queueFamilyIndices_t result;
	if ( queueFamilyTypes & STUDIOAPI_VK_QUEUE_FLAG_COMPUTE )
	{
		for ( uint32 indexQueueFamily = 0, numQueueFamily = (uint32)vkQueueFamiliesProperties.size(); indexQueueFamily < numQueueFamily; ++indexQueueFamily )
		{
			const VkQueueFamilyProperties& vkQueueFamilyProperties = vkQueueFamiliesProperties[indexQueueFamily];
			if ( vkQueueFamilyProperties.queueCount > 0 && ( vkQueueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT ) && !( vkQueueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT ) )
			{
				result.computeFamilyIndex	   = indexQueueFamily;
				result.computeFamilyProperties = vkQueueFamilyProperties;
				break;
			}
		}
	}

	// Dedicated queue for transfer
	// Try to find a queue family index that supports transfer but not graphics and compute
	if ( queueFamilyTypes & STUDIOAPI_VK_QUEUE_FLAG_TRANSFER )
	{
		for ( uint32 indexQueueFamily = 0, numQueueFamily = (uint32)vkQueueFamiliesProperties.size(); indexQueueFamily < numQueueFamily; ++indexQueueFamily )
		{
			const VkQueueFamilyProperties& vkQueueFamilyProperties = vkQueueFamiliesProperties[indexQueueFamily];
			if ( vkQueueFamilyProperties.queueCount > 0 && ( vkQueueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT ) && !( vkQueueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT ) && !( vkQueueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT ) )
			{
				result.transferFamilyIndex		= indexQueueFamily;
				result.transferFamilyProperties = vkQueueFamilyProperties;
				break;
			}
		}
	}

	// Dedicated queue for graphics and present
	// Try to find a queue family index that supports present and graphics
	if ( queueFamilyTypes & ( STUDIOAPI_VK_QUEUE_FLAG_GRAPHICS | STUDIOAPI_VK_QUEUE_FLAG_PRESENT ) )
	{
		for ( uint32 indexQueueFamily = 0, numQueueFamily = (uint32)vkQueueFamiliesProperties.size(); indexQueueFamily < numQueueFamily; ++indexQueueFamily )
		{
			const VkQueueFamilyProperties& vkQueueFamilyProperties = vkQueueFamiliesProperties[indexQueueFamily];
			if ( vkQueueFamilyProperties.queueCount > 0 && ( vkQueueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT ) && VK_Plat_IsPhysicalDeviceSurfaceSupport( vkPhysicalDevice, indexQueueFamily ) )
			{
				result.graphicsFamilyIndex		= indexQueueFamily;
				result.presentFamilyIndex		= indexQueueFamily;
				result.graphicsFamilyProperties = vkQueueFamilyProperties;
				result.presentFamilyProperties	= vkQueueFamilyProperties;
				break;
			}
		}
	}

	// For other queue types or if no separate a queue is present, return the first one to support the requested flags
	for ( uint32 indexQueueFamily = 0, numQueueFamily = (uint32)vkQueueFamiliesProperties.size(); indexQueueFamily < numQueueFamily; ++indexQueueFamily )
	{
		const VkQueueFamilyProperties& vkQueueFamilyProperties = vkQueueFamiliesProperties[indexQueueFamily];
		if ( vkQueueFamilyProperties.queueCount <= 0 )
		{
			continue;
		}

		// Get the transfer queue family index if we haven't it yet
		if ( queueFamilyTypes & STUDIOAPI_VK_QUEUE_FLAG_TRANSFER && !result.IsValid( STUDIOAPI_VK_QUEUE_FLAG_TRANSFER ) && ( vkQueueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT ) )
		{
			result.transferFamilyIndex		= indexQueueFamily;
			result.transferFamilyProperties = vkQueueFamilyProperties;
		}

		// Get the compute queue family index if we haven't it yet
		if ( queueFamilyTypes & STUDIOAPI_VK_QUEUE_FLAG_COMPUTE && !result.IsValid( STUDIOAPI_VK_QUEUE_FLAG_COMPUTE ) && ( vkQueueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT ) )
		{
			result.computeFamilyIndex	   = indexQueueFamily;
			result.computeFamilyProperties = vkQueueFamilyProperties;
		}

		// Get the graphics queue family index if we haven't it yet
		if ( queueFamilyTypes & STUDIOAPI_VK_QUEUE_FLAG_GRAPHICS && !result.IsValid( STUDIOAPI_VK_QUEUE_FLAG_GRAPHICS ) && ( vkQueueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT ) )
		{
			result.graphicsFamilyIndex		= indexQueueFamily;
			result.graphicsFamilyProperties = vkQueueFamilyProperties;
		}

		// Get the present queue family index if we haven't it yet
		if ( queueFamilyTypes & STUDIOAPI_VK_QUEUE_FLAG_PRESENT && !result.IsValid( STUDIOAPI_VK_QUEUE_FLAG_PRESENT ) && VK_Plat_IsPhysicalDeviceSurfaceSupport( vkPhysicalDevice, indexQueueFamily ) )
		{
			result.presentFamilyIndex	   = indexQueueFamily;
			result.presentFamilyProperties = vkQueueFamilyProperties;
		}
	}

	return result;
}

/*
==================
CStudioAPIDeviceVk::Shutdown
==================
*/
void CStudioAPIDeviceVk::Shutdown()
{
	// Destroy all queues
	graphicsQueue.Shutdown();
	presentQueue.Shutdown();
	transferQueue.Shutdown();
	computeQueue.Shutdown();

	// Destroy logical device
	if ( vkLogicalDevice != VK_NULL_HANDLE )
	{
		vkDestroyDevice( vkLogicalDevice, NULL );
		vkLogicalDevice = VK_NULL_HANDLE;
	}

#if !RETAIL
	// Destroy debug messenger if it was created
	if ( vkDebugMessenger != VK_NULL_HANDLE )
	{
		vkDestroyDebugUtilsMessengerEXT( vkInstance, vkDebugMessenger, NULL );
		vkDebugMessenger = VK_NULL_HANDLE;
	}
#endif	// !RETAIL

	// Destroy Vulkan instance
	if ( vkInstance != VK_NULL_HANDLE )
	{
		vkDestroyInstance( vkInstance, NULL );
		vkInstance = VK_NULL_HANDLE;
	}

	// Clear other fields
	gpuVendorId = STUDIOAPI_GPU_VENDOR_ID_UNKNOWN;
	Mem_Memzero( &vkMemoryInfo, sizeof( VkPhysicalDeviceMemoryProperties ) );
	Mem_Memzero( &vkDeviceInfo, sizeof( VkPhysicalDeviceProperties ) );
	Msg( "StudioAPIVk: StudioAPI device is destroyed" );
}
