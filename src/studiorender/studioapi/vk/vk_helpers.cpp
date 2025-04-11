#include "pch_studioapi.h"
#include "core/debug.h"
#include "pixelformatinfos/pixelformatinfos.h"
#include "studiorender/studioapi/vk/vk_helpers.h"

/*
==================
VK_CheckVkResult
==================
*/
void VK_CheckVkResult( VkResult vkResult, const achar* pVkFunction, const achar* pFileName, uint32 line )
{
	Sys_Error( "Vulkan function failed\n%s\n\nVkResult: %d (%s)\nFile: %s\nLine: %u", pVkFunction, ( int32 )vkResult, VK_ConvVkResultToText( vkResult ), pFileName, line );
}

/*
==================
VK_CheckInstanceExtensionsSupported
==================
*/
bool VK_CheckInstanceExtensionsSupported( const achar** pInstanceExtensionNames, uint32 instanceExtensionNum )
{
	// Get all available extensions
	std::vector<VkExtensionProperties>		availableExtensions;
	uint32									instanceExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties( NULL, &instanceExtensionCount, NULL );
	
	// If we nothing is supported, exit from function with false value
	if ( instanceExtensionCount == 0 )
	{
		return false;
	}

	// Otherwise we get all supported extensions
	availableExtensions.resize( instanceExtensionCount );
	vkEnumerateInstanceExtensionProperties( NULL, &instanceExtensionCount, availableExtensions.data() );

	// Check is supported extensions who we got in function's arguments
	for ( uint32 index = 0, count = instanceExtensionNum; index < count; ++index )
	{
		bool			bFound = false;
		const achar*	pRequiredExtName = pInstanceExtensionNames[index];

		for ( uint32 idxSupportedExt = 0, countSuportedExts = ( uint32 )availableExtensions.size(); idxSupportedExt < countSuportedExts; ++idxSupportedExt )
		{
			const VkExtensionProperties&	vkExtProperties = availableExtensions[idxSupportedExt];
			if ( !S_Stricmp( pRequiredExtName, vkExtProperties.extensionName ) )
			{
				bFound = true;
				break;
			}
		}

		if ( !bFound )
		{
			return false;
		}
	}
	
	return true;
}

/*
==================
VK_CheckValidationLayersSupported
==================
*/
bool VK_CheckValidationLayersSupported( const achar** pValidationLayerNames, uint32 validationLayerNum )
{
	// Get all available layers
	std::vector<VkLayerProperties>		availableLayers;
	uint32								layersCount = 0;
	vkEnumerateInstanceLayerProperties( &layersCount, NULL );

	// If we nothing is supported, exit from function with false value
	if ( layersCount == 0 )
	{
		return false;
	}

	// Otherwise we get all supported layers
	availableLayers.resize( layersCount );
	vkEnumerateInstanceLayerProperties( &layersCount, availableLayers.data() );

	// Check is supported layers who we got in function's arguments
	for ( uint32 index = 0, count = validationLayerNum; index < count; ++index )
	{
		bool			bFound = false;
		const achar*	pRequiredLayerName = pValidationLayerNames[index];

		for ( uint32 idxSupportedLayer = 0, countSuportedLayers = ( uint32 )availableLayers.size(); idxSupportedLayer < countSuportedLayers; ++idxSupportedLayer )
		{
			const VkLayerProperties&	vkLayerProperties = availableLayers[idxSupportedLayer];
			if ( !S_Stricmp( pRequiredLayerName, vkLayerProperties.layerName ) )
			{
				bFound = true;
				break;
			}
		}

		if ( !bFound )
		{
			return false;
		}
	}
	
	return true;
}

/*
==================
VK_CheckValidationLayersSupported
==================
*/
bool VK_CheckDeviceExtensionsSupported( VkPhysicalDevice vkPhysicalDevice, const achar** pDeviceExtensionNames, uint32 deviceExtensionNum )
{
	// Get all supported extensions by physical device
	uint32		availableDeviceExtCount = 0;
	vkEnumerateDeviceExtensionProperties( vkPhysicalDevice, NULL, &availableDeviceExtCount, NULL );

	// If we nothing is supported, exit from function with false value
	if ( availableDeviceExtCount == 0 )
	{
		return false;
	}

	// Otherwise we get all supported layers and check they
	std::vector<VkExtensionProperties>		availableDeviceExtensions( availableDeviceExtCount );
	vkEnumerateDeviceExtensionProperties( vkPhysicalDevice, NULL, &availableDeviceExtCount, availableDeviceExtensions.data() );

	for ( uint32 index = 0; index < deviceExtensionNum; ++index )
	{
		bool			bFound = false;
		const achar*	pRequiredExtName = pDeviceExtensionNames[index];

		for ( uint32 idxSupportedExt = 0; idxSupportedExt < availableDeviceExtCount; ++idxSupportedExt )
		{
			const VkExtensionProperties&	vkExtProperties = availableDeviceExtensions[idxSupportedExt];
			if ( !S_Stricmp( pRequiredExtName, vkExtProperties.extensionName ) )
			{
				bFound = true;
				break;
			}
		}

		if ( !bFound )
		{
			return false;
		}
	}

	return true;
}

#if !RETAIL
/*
==================
VK_DebugCallback
==================
*/
VkBool32 VKAPI_PTR VK_DebugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData )
{
	if ( messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT ||
		 messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT )
	{
		Msg( "StudioAPIVk: Vulkan validation: %s", pCallbackData->pMessage );
	}
	else if ( messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT )
	{
		Warning( "StudioAPIVk: Vulkan validation: %s", pCallbackData->pMessage );
	}
	else if ( messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT )
	{
		Error( "StudioAPIVk: Vulkan validation: %s", pCallbackData->pMessage );
		Sys_DebugBreak();
	}
	return VK_FALSE;
}
#endif // !RETAIL

/*
==================
VK_TranslateVkAccessFlagsToVkPipelineStageFlags
==================
*/
VkPipelineStageFlags VK_TranslateVkAccessFlagsToVkPipelineStageFlags( VkAccessFlags vkAccessFlags )
{
	VkPipelineStageFlags	vkPipelineStageFlags = 0;
	
	// Indirect command read
	if ( vkAccessFlags & VK_ACCESS_INDIRECT_COMMAND_READ_BIT )
	{
		vkPipelineStageFlags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
	}

	// Color attachment read/write
	if ( vkAccessFlags & ( VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT ) )
	{
		vkPipelineStageFlags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}

	// Shader read/write
	if ( vkAccessFlags & ( VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_UNIFORM_READ_BIT ) )
	{
		vkPipelineStageFlags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
							 |  VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT
							 |  VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT
							 |  VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT
							 |  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
							 |  VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	}

	// Depth stencil attachment read/write
	if ( vkAccessFlags & ( VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT ) )
	{
		vkPipelineStageFlags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	}

	// Transfer read/write
	if ( vkAccessFlags & ( VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT ) )
	{
		vkPipelineStageFlags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
	}

	// Host read/write
	if ( vkAccessFlags & ( VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT ) )
	{
		vkPipelineStageFlags |= VK_PIPELINE_STAGE_HOST_BIT;
	}

	// Vertex attribute read or index read
	if ( vkAccessFlags & ( VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT ) )
	{
		vkPipelineStageFlags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
	}

	// If no one flag not set use VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
	if ( vkPipelineStageFlags == 0 )
	{
		vkPipelineStageFlags |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	}

	// We are done!
	return vkPipelineStageFlags;
}

/*
==================
VK_TranslateVkImageLayoutToVkAccessFlags
==================
*/
VkAccessFlags VK_TranslateVkImageLayoutToVkAccessFlags( VkImageLayout vkImageLayout )
{
	switch ( vkImageLayout )
	{
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:						return VK_ACCESS_SHADER_READ_BIT;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:						return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	case VK_IMAGE_LAYOUT_GENERAL:										return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:							return VK_ACCESS_TRANSFER_READ_BIT;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:							return VK_ACCESS_TRANSFER_WRITE_BIT;	
	case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
	case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
	case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
	case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:				return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
	case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:				return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:								return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	default:															return VK_IMAGE_LAYOUT_UNDEFINED;
	}
}

/*
==================
VK_TranslatePixelFormat
==================
*/
VkFormat VK_TranslatePixelFormat( studioAPIPixelFormat_t pixelFormat )
{
	// Convert the pixel format
	switch ( pixelFormat )
	{
	case STUDIOAPI_PIXEL_FORMAT_RGBA8:					return VK_FORMAT_R8G8B8A8_UNORM;
	case STUDIOAPI_PIXEL_FORMAT_RG8:					return VK_FORMAT_R8G8_UNORM;
	case STUDIOAPI_PIXEL_FORMAT_R8:						return VK_FORMAT_R8_UNORM;	
	case STUDIOAPI_PIXEL_FORMAT_RGBA16:					return VK_FORMAT_R16G16B16A16_UNORM;
	case STUDIOAPI_PIXEL_FORMAT_RG16:					return VK_FORMAT_R16G16_UNORM;
	case STUDIOAPI_PIXEL_FORMAT_R16:					return VK_FORMAT_R16_UNORM;
	case STUDIOAPI_PIXEL_FORMAT_RGBA32F:				return VK_FORMAT_R32G32B32A32_SFLOAT;
	case STUDIOAPI_PIXEL_FORMAT_RG32F:					return VK_FORMAT_R32G32_SFLOAT;
	case STUDIOAPI_PIXEL_FORMAT_R32F:					return VK_FORMAT_R32_SFLOAT;
	case STUDIOAPI_PIXEL_FORMAT_RGBA16F:				return VK_FORMAT_R16G16B16A16_SFLOAT;
	case STUDIOAPI_PIXEL_FORMAT_RG16F:					return VK_FORMAT_R16G16_SFLOAT;
	case STUDIOAPI_PIXEL_FORMAT_R16F:					return VK_FORMAT_R16_SFLOAT;
	case STUDIOAPI_PIXEL_FORMAT_BC1:					return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
	case STUDIOAPI_PIXEL_FORMAT_BC2:					return VK_FORMAT_BC2_UNORM_BLOCK;
	case STUDIOAPI_PIXEL_FORMAT_BC3:					return VK_FORMAT_BC3_UNORM_BLOCK;
	case STUDIOAPI_PIXEL_FORMAT_BC4:					return VK_FORMAT_BC4_UNORM_BLOCK;
	case STUDIOAPI_PIXEL_FORMAT_BC5:					return VK_FORMAT_BC5_UNORM_BLOCK;
	case STUDIOAPI_PIXEL_FORMAT_BC6H:					return VK_FORMAT_BC6H_SFLOAT_BLOCK;
	case STUDIOAPI_PIXEL_FORMAT_BC7:					return VK_FORMAT_BC7_UNORM_BLOCK;
	case STUDIOAPI_PIXEL_FORMAT_DEPTH16:				return VK_FORMAT_D16_UNORM;
	case STUDIOAPI_PIXEL_FORMAT_DEPTH32F:				return VK_FORMAT_D32_SFLOAT;
	case STUDIOAPI_PIXEL_FORMAT_DEPTH16_STENCIL8:		return VK_FORMAT_D16_UNORM_S8_UINT;
	case STUDIOAPI_PIXEL_FORMAT_DEPTH24_STENCIL8:		return VK_FORMAT_D24_UNORM_S8_UINT;
	case STUDIOAPI_PIXEL_FORMAT_DEPTH32F_STENCIL8:		return VK_FORMAT_D32_SFLOAT_S8_UINT;
	default:
		AssertMsg( false, "Unknown StudioAPI pixel format '%s'", g_PixelFormatInfos[pixelFormat].pName );
		return VK_FORMAT_UNDEFINED;
	}
}

/*
==================
VK_TranslateTextureLayout
==================
*/
VkImageLayout VK_TranslateTextureLayout( studioAPITextureLayout_t textureLayout )
{
	switch ( textureLayout )
	{
	case STUDIOAPI_TEXTURE_LAYOUT_UNDEFINED:								return VK_IMAGE_LAYOUT_UNDEFINED;
	case STUDIOAPI_TEXTURE_LAYOUT_SHADER_RESOURCE:							return VK_IMAGE_LAYOUT_GENERAL;
	case STUDIOAPI_TEXTURE_LAYOUT_SHADER_RESOURCE_READONLY:					return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	case STUDIOAPI_TEXTURE_LAYOUT_TRANSFER_SRC:								return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	case STUDIOAPI_TEXTURE_LAYOUT_TRANSFER_DST:								return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	case STUDIOAPI_TEXTURE_LAYOUT_COLOR_RENDER_TARGET:						return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	case STUDIOAPI_TEXTURE_LAYOUT_DEPTH_STENCIL_RENDER_TARGET:				return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	case STUDIOAPI_TEXTURE_LAYOUT_DEPTH_READONLY_STENCIL_RENDER_TARGET:		return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
	case STUDIOAPI_TEXTURE_LAYOUT_DEPTH_RENDER_TARGET_STENCIL_READONLY:		return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
	case STUDIOAPI_TEXTURE_LAYOUT_DEPTH_READONLY_STENCIL_READONLY:			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
	case STUDIOAPI_TEXTURE_LAYOUT_DEPTH_RENDER_TARGET:						return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
	case STUDIOAPI_TEXTURE_LAYOUT_DEPTH_READONLY:							return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
	case STUDIOAPI_TEXTURE_LAYOUT_STENCIL_RENDER_TARGET:					return VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
	case STUDIOAPI_TEXTURE_LAYOUT_STENCIL_READONLY:							return VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL;
	case STUDIOAPI_TEXTURE_LAYOUT_PRESENT:									return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	default:
		AssertMsg( false, "Unknown StudioAPI texture layout 0x%X", textureLayout );
		return VK_IMAGE_LAYOUT_GENERAL;
	}
}