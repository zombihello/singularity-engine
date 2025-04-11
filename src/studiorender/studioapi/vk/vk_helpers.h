/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_HELPERS_H
#define VK_HELPERS_H

#include "Volk/volk.h"
#include "core/platform.h"
#include "core/types.h"

/**
 * @ingroup studioapi_vk
 * @brief Macro to verify Vulkan result
 */
#define STUDIOAPI_VK_VERIFY_RESULT( vkFunction ) \
	{ \
		VkResult	__vkResult__ = vkFunction; \
		if ( __vkResult__ != VK_SUCCESS ) \
		{ \
			VK_CheckVkResult( __vkResult__, #vkFunction, __FILE__, __LINE__ ); \
		} \
	}

/**
 * @ingroup studioapi_vk
 * @brief Vulkan begin range of descriptor types
 */
#define STUDIOAPI_VK_DESCRIPTOR_TYPE_BEGIN_RANGE		VK_DESCRIPTOR_TYPE_SAMPLER

/**
 * @ingroup studioapi_vk
 * @brief Vulkan end range of descriptor types
 */
#define STUDIOAPI_VK_DESCRIPTOR_TYPE_END_RANGE			VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT

/**
 * @ingroup studioapi_vk
 * @brief Vulkan range size of descriptor types
 */
#define STUDIOAPI_VK_DESCRIPTOR_TYPE_RANGE_SIZE			(STUDIOAPI_VK_DESCRIPTOR_TYPE_END_RANGE - STUDIOAPI_VK_DESCRIPTOR_TYPE_BEGIN_RANGE + 1)

/**
 * @ingroup studioapi_vk
 * @brief The maximum number of vertex elements which can be used by a vertex declaration
 */
#define STUDIOAPI_VK_MAX_VERTEX_ELEMENT_COUNT			17

/**
 * @ingroup studioapi_vk
 * @brief The number frames to wait before releasing to OS
 */
#define STUDIOAPI_VK_NUM_FRAMES_TO_WAIT_BEFORE_RELEASING_TO_OS		3

/**
 * @ingroup studioapi_vk
 * @brief A ring buffer size of constant buffers
 */
#define STUDIOAPI_VK_CONSTANT_RING_BUFFER_SIZE						(16 * 1024 * 1024)

/**
 * @ingroup studioapi_vk
 * @brief Required alignment for VkBufferImageCopy::bufferOffset
 */
#define STUDIOAPI_VK_BUFFER_OFFSET_ALIGNMENT						4

/**
 * @ingroup studioapi_vk
 * @brief Time to wait before deleting of a command buffer
 */
#define STUDIOAPI_VK_CMDBUFFER_TIME_TO_WAIT_BEFORE_DELETING		10

/**
 * @ingroup studioapi_vk
 * @brief Time to wait before deleting of a command list
 */
#define STUDIOAPI_VK_CMDLIST_TIME_TO_WAIT_BEFORE_DELETING		10

/**
 * @ingroup studioapi_vk
 * @brief Frames in-flight number
 */
#define STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT						3

/**
 * @ingroup studioapi_vk
 * @brief Maximum pending buffer barriers number
 */
#define STUDIOAPI_VK_MAX_NUM_PENDING_BUFFER_BARRIERS			64

/**
 * @ingroup studioapi_vk
 * @brief Maximum pending image barriers number
 */
#define STUDIOAPI_VK_MAX_NUM_PENDING_IMAGE_BARRIERS				128

/**
 * @ingroup studioapi_vk
 * @brief Maximum pending memory barriers number
 */
#define STUDIOAPI_VK_MAX_NUM_PENDING_MEMORY_BARRIERS			64

/**
 * @ingroup studioapi_vk
 * @brief All read VkAccessFlags flags
 */
#define STUDIOAPI_VK_ACCESS_READ_FLAGS							(0 \
																| VK_ACCESS_INDIRECT_COMMAND_READ_BIT \
																| VK_ACCESS_INDEX_READ_BIT \
																| VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT \
																| VK_ACCESS_UNIFORM_READ_BIT \
																| VK_ACCESS_INPUT_ATTACHMENT_READ_BIT \
																| VK_ACCESS_SHADER_READ_BIT \
																| VK_ACCESS_COLOR_ATTACHMENT_READ_BIT \
																| VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT \
																| VK_ACCESS_TRANSFER_READ_BIT \
																| VK_ACCESS_HOST_READ_BIT \
																| VK_ACCESS_MEMORY_READ_BIT \
																| VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT \
																| VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT \
																| VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT \
																| VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR \
																| VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT \
																| VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR \
																| VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV \
																| VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV \
																| VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV)

/**
 * @ingroup studioapi_vk
 * @brief All write VkAccessFlags flags
 */
#define STUDIOAPI_VK_ACCESS_WRITE_FLAGS							(0 \
																| VK_ACCESS_SHADER_WRITE_BIT \
																| VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT \
																| VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT \
																| VK_ACCESS_TRANSFER_WRITE_BIT \
																| VK_ACCESS_HOST_WRITE_BIT \
																| VK_ACCESS_MEMORY_WRITE_BIT \
																| VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT \
																| VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT \
																| VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR \
																| VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV \
																| VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV)

/**
 * @ingroup studioapi_vk
 * @brief All known VkAccessFlags flags by STUDIOAPI_VK_ACCESS_READ_FLAGS and STUDIOAPI_VK_ACCESS_WRITE_FLAGS
 */
#define STUDIOAPI_VK_ACCESS_KNOWN_FLAGS							(STUDIOAPI_VK_ACCESS_READ_FLAGS | STUDIOAPI_VK_ACCESS_WRITE_FLAGS)

/**
 * @ingroup studioapi_vk
 * @brief All known studioAPIBufferState_t flags
 */
#define STUDIOAPI_VK_BUFFER_KNOWN_STATES						(0 \
																| STUDIOAPI_BUFFER_STATE_UNDEFINED \
																| STUDIOAPI_BUFFER_STATE_SHADER_RESOURCE \
																| STUDIOAPI_BUFFER_STATE_SHADER_RESOURCE_READONLY \
																| STUDIOAPI_BUFFER_STATE_TRANSFER_SRC \
																| STUDIOAPI_BUFFER_STATE_TRANSFER_DST \
																| STUDIOAPI_BUFFER_STATE_VERTEX_BUFFER \
																| STUDIOAPI_BUFFER_STATE_INDEX_BUFFER \
																| STUDIOAPI_BUFFER_STATE_CONSTANT_BUFFER \
																| STUDIOAPI_BUFFER_STATE_STRUCTURED_BUFFER)

/**
 * @ingroup studioapi_vk
 * @brief Vulkan shader platform name
 */
#define STUDIOAPI_VK_SHADER_PLATFORM_NAME						"vk"

/**
 * @ingroup studioapi_vk
 * @brief Check Vulkan result
 * Checks that the given Vulkan result isn't a failure. If it is, the application exits 
 * with an appropriate error message
 * 
 * @param vkResult		The result code to check
 * @param pVkFunction	Tested function name
 * @param pFileName		The filename of the source file containing code
 * @param line			The line number of code within Filename
 */
void VK_CheckVkResult( VkResult vkResult, const achar* pVkFunction, const achar* pFileName, uint32 line );

/**
 * @ingroup studioapi_vk
 * @brief Check on is instance extensions is supported
 * 
 * @param pInstanceExtensionNames	Array of instance extension names to check
 * @param instanceExtensionNum		Number of instance extensions
 * @return Return TRUE if all extensions in array is supported, otherwise return FALSE
 */
bool VK_CheckInstanceExtensionsSupported( const achar** pInstanceExtensionNames, uint32 instanceExtensionNum );

/**
 * @ingroup studioapi_vk
 * @brief Check on is validation layers is supported
 * 
 * @param pValidationLayerNames		Array of validation layer names to check
 * @param validationLayerNum		Number of validation layers
 * @return Return TRUE if all validation layers in array is supported, otherwise return FALSE
 */
bool VK_CheckValidationLayersSupported( const achar** pValidationLayerNames, uint32 validationLayerNum );

/**
 * @ingroup studioapi_vk
 * @brief Check on is device extensions is supported
 *
 * @param vkPhysicalDevice		Vulkan physical device
 * @param pDeviceExtensionNames	Array of device extension names to check
 * @param deviceExtensionNum	Number of device extension names
 * @return Return TRUE if all validation layers in array is supported, otherwise return FALSE
 */
bool VK_CheckDeviceExtensionsSupported( VkPhysicalDevice vkPhysicalDevice, const achar** pDeviceExtensionNames, uint32 deviceExtensionNum );

#if !RETAIL
	/**
	 * @ingroup studioapi_vk
	 * @brief Vulkan debug callback
	 * For more about parameters see https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/PFN_vkDebugUtilsMessengerCallbackEXT.html
	 */
	VkBool32 VKAPI_PTR VK_DebugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData );
#endif // !RETAIL

/**
 * @ingroup studioapi_vk
 * @brief Translate VkAccessFlags to VkPipelineStageFlags
 * This is a helper function to try to figure out what stage a resource is (likely) used at given it's current Vulkan access mask. It's usually pretty accurate
 * 
 * @param vkAccessFlags		Vulkan access flags
 * @return Return translated vkAccessFlags into Vulkan pipeline stage flags which a resource is (likely) used
 */
VkPipelineStageFlags VK_TranslateVkAccessFlagsToVkPipelineStageFlags( VkAccessFlags vkAccessFlags );

/**
 * @ingroup studioapi_vk
 * @brief Translate VkImageLayout to VkAccessFlags
 * @param vkImageLayout		Vulkan image layout
 * @return Return translated VkAccessFlags into Vulkan image layout
 */
VkAccessFlags VK_TranslateVkImageLayoutToVkAccessFlags( VkImageLayout vkImageLayout );

/**
 * @ingroup studioapi_vk
 * @brief Copy A and B with return TRUE if they aren't equal
 * @param a		Value A
 * @param b		Value B
 * @return Return TRUE if A and B aren't equal, otherwise FALSE
 */
template<typename TType>
FORCEINLINE bool VK_CopyAndReturnNotEqual( TType& a, TType b )
{
	bool	bIsNotEqual = a != b;
	a = b;
	return bIsNotEqual;
}

/**
 * @ingroup studioapi_vk
 * brief Is a depth format
 * @param pixelFormat	Pixel format to check	
 * @return Return TRUE if the format is depth, otherwise FALSE
 */
FORCEINLINE bool VK_IsDepthPixelFormat( studioAPIPixelFormat_t pixelFormat )
{
	return	pixelFormat == STUDIOAPI_PIXEL_FORMAT_DEPTH16 ||
			pixelFormat == STUDIOAPI_PIXEL_FORMAT_DEPTH32F ||
			pixelFormat == STUDIOAPI_PIXEL_FORMAT_DEPTH16_STENCIL8 ||
			pixelFormat == STUDIOAPI_PIXEL_FORMAT_DEPTH24_STENCIL8 ||
			pixelFormat == STUDIOAPI_PIXEL_FORMAT_DEPTH32F_STENCIL8;
}

/**
 * @ingroup studioapi_vk
 * brief Is a stencil format
 * @param pixelFormat	Pixel format to check
 * @return Return TRUE if the format is stencil, otherwise FALSE
 */
FORCEINLINE bool VK_IsStencilPixelFormat( studioAPIPixelFormat_t pixelFormat )
{
	return	pixelFormat == STUDIOAPI_PIXEL_FORMAT_DEPTH16_STENCIL8 ||
			pixelFormat == STUDIOAPI_PIXEL_FORMAT_DEPTH24_STENCIL8 ||
			pixelFormat == STUDIOAPI_PIXEL_FORMAT_DEPTH32F_STENCIL8;
}

/**
 * @ingroup studioapi_vk
 * @brief Translate StudioAPI pixel format to VkFormat
 * @return Return translated StudioAPI pixel format to VkFormat
 */
VkFormat VK_TranslatePixelFormat( studioAPIPixelFormat_t pixelFormat );

/**
 * @ingroup studioapi_vk
 * @brief Translate StudioAPI texture layout to VkImageLayout
 * @param textureLayout		StudioAPI texture layout
 * @return Return translated the texture layout to VkImageLayout
 */
VkImageLayout VK_TranslateTextureLayout( studioAPITextureLayout_t textureLayout );

/**
 * @ingroup studioapi_vk
 * @brief Translate StudioAPI pixel format to VkImageAspectFlags
 * @return Return translated StudioAPI pixel format to VkImageAspectFlags
 */
FORCEINLINE VkImageAspectFlags VK_TranslateImageAspectFlags( studioAPIPixelFormat_t pixelFormat )
{
	VkImageAspectFlags		vkImageAspectFlags = 0;
	if ( VK_IsDepthPixelFormat( pixelFormat ) )
	{
		vkImageAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
		if ( VK_IsStencilPixelFormat( pixelFormat ) )
		{
			vkImageAspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else
	{
		vkImageAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	return vkImageAspectFlags;
}

/**
 * @ingroup studioapi_vk
 * @brief Convert Vulkan result to human readable text
 * 
 * @param vkResult	Vulkan result
 * @return Return result in human readable text
 */
FORCEINLINE const achar* VK_ConvVkResultToText( VkResult vkResult )
{
	switch ( vkResult )
	{
	case VK_SUCCESS:												return "VK_SUCCESS";
	case VK_NOT_READY:												return "VK_NOT_READY";
	case VK_TIMEOUT:												return "VK_TIMEOUT";
	case VK_EVENT_SET:												return "VK_EVENT_SET";
	case VK_EVENT_RESET:											return "VK_EVENT_RESET";
	case VK_INCOMPLETE:												return "VK_INCOMPLETE";
	case VK_ERROR_OUT_OF_HOST_MEMORY:								return "VK_ERROR_OUT_OF_HOST_MEMORY";
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:								return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
	case VK_ERROR_INITIALIZATION_FAILED:							return "VK_ERROR_INITIALIZATION_FAILED";
	case VK_ERROR_DEVICE_LOST:										return "VK_ERROR_DEVICE_LOST";
	case VK_ERROR_MEMORY_MAP_FAILED:								return "VK_ERROR_MEMORY_MAP_FAILED";
	case VK_ERROR_LAYER_NOT_PRESENT:								return "VK_ERROR_LAYER_NOT_PRESENT";
	case VK_ERROR_EXTENSION_NOT_PRESENT:							return "VK_ERROR_EXTENSION_NOT_PRESENT";
	case VK_ERROR_FEATURE_NOT_PRESENT:								return "VK_ERROR_FEATURE_NOT_PRESENT";
	case VK_ERROR_INCOMPATIBLE_DRIVER:								return "VK_ERROR_INCOMPATIBLE_DRIVER";
	case VK_ERROR_TOO_MANY_OBJECTS:									return "VK_ERROR_TOO_MANY_OBJECTS";
	case VK_ERROR_FORMAT_NOT_SUPPORTED:								return "VK_ERROR_FORMAT_NOT_SUPPORTED";
	case VK_ERROR_FRAGMENTED_POOL:									return "VK_ERROR_FRAGMENTED_POOL";
	case VK_ERROR_UNKNOWN:											return "VK_ERROR_UNKNOWN";
	case VK_ERROR_OUT_OF_POOL_MEMORY:								return "VK_ERROR_OUT_OF_POOL_MEMORY";
	case VK_ERROR_INVALID_EXTERNAL_HANDLE:							return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
	case VK_ERROR_FRAGMENTATION:									return "VK_ERROR_FRAGMENTATION";
	case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:					return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
	case VK_ERROR_SURFACE_LOST_KHR:									return "VK_ERROR_SURFACE_LOST_KHR";
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:							return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
	case VK_SUBOPTIMAL_KHR:											return "VK_SUBOPTIMAL_KHR";
	case VK_ERROR_OUT_OF_DATE_KHR:									return "VK_ERROR_OUT_OF_DATE_KHR";
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:							return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
	case VK_ERROR_VALIDATION_FAILED_EXT:							return "VK_ERROR_VALIDATION_FAILED_EXT";
	case VK_ERROR_INVALID_SHADER_NV:								return "VK_ERROR_INVALID_SHADER_NV";
	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:		return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
	case VK_ERROR_NOT_PERMITTED_EXT:								return "VK_ERROR_NOT_PERMITTED_EXT";
	case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:				return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
	case VK_THREAD_IDLE_KHR:										return "VK_THREAD_IDLE_KHR";
	case VK_THREAD_DONE_KHR:										return "VK_THREAD_DONE_KHR";
	case VK_OPERATION_DEFERRED_KHR:									return "VK_OPERATION_DEFERRED_KHR";
	case VK_OPERATION_NOT_DEFERRED_KHR:								return "VK_OPERATION_NOT_DEFERRED_KHR";
	case VK_PIPELINE_COMPILE_REQUIRED_EXT:							return "VK_PIPELINE_COMPILE_REQUIRED_EXT";
	default:														return "UNKNOWN";
	}
}

/**
 * @ingroup studioapi_vk
 * @brief Convert Vulkan present mode to human readable text
 *
 * @param vkPresentMode	Vulkan present mode
 * @return Return present mode in human readable text
 */
FORCEINLINE const achar* VK_ConvVkPresentModeToText( VkPresentModeKHR vkPresentMode )
{
	switch ( vkPresentMode )
	{
	case VK_PRESENT_MODE_IMMEDIATE_KHR:						return "VK_PRESENT_MODE_IMMEDIATE_KHR";
	case VK_PRESENT_MODE_MAILBOX_KHR:						return "VK_PRESENT_MODE_MAILBOX_KHR";
	case VK_PRESENT_MODE_FIFO_KHR:							return "VK_PRESENT_MODE_FIFO_KHR";
	case VK_PRESENT_MODE_FIFO_RELAXED_KHR:					return "VK_PRESENT_MODE_FIFO_RELAXED_KHR";
	case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:			return "VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR";
	case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:		return "VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR";
	default:												return "UNKNOWN";
	}
}

/**
 * @ingroup studioapi_vk
 * @brief Convert Vulkan format to human readable text
 *
 * @param vkFormat	Vulkan format
 * @return Return Vulkan format in human readable text
 */
FORCEINLINE const achar* VK_ConvVkFormatToText( VkFormat vkFormat )
{
	switch ( vkFormat )
	{
	case VK_FORMAT_UNDEFINED:										return "VK_FORMAT_UNDEFINED";
	case VK_FORMAT_R4G4_UNORM_PACK8:								return "VK_FORMAT_R4G4_UNORM_PACK8";
	case VK_FORMAT_R4G4B4A4_UNORM_PACK16:							return "VK_FORMAT_R4G4B4A4_UNORM_PACK16";
	case VK_FORMAT_B4G4R4A4_UNORM_PACK16:							return "VK_FORMAT_B4G4R4A4_UNORM_PACK16";
	case VK_FORMAT_R5G6B5_UNORM_PACK16:								return "VK_FORMAT_R5G6B5_UNORM_PACK16";
	case VK_FORMAT_B5G6R5_UNORM_PACK16:								return "VK_FORMAT_B5G6R5_UNORM_PACK16";
	case VK_FORMAT_R5G5B5A1_UNORM_PACK16:							return "VK_FORMAT_R5G5B5A1_UNORM_PACK16";
	case VK_FORMAT_B5G5R5A1_UNORM_PACK16:							return "VK_FORMAT_B5G5R5A1_UNORM_PACK16";
	case VK_FORMAT_A1R5G5B5_UNORM_PACK16:							return "VK_FORMAT_A1R5G5B5_UNORM_PACK16";
	case VK_FORMAT_R8_UNORM:										return "VK_FORMAT_R8_UNORM";
	case VK_FORMAT_R8_SNORM:										return "VK_FORMAT_R8_SNORM";
	case VK_FORMAT_R8_USCALED:										return "VK_FORMAT_R8_USCALED";
	case VK_FORMAT_R8_SSCALED:										return "VK_FORMAT_R8_SSCALED";
	case VK_FORMAT_R8_UINT:											return "VK_FORMAT_R8_UINT";
	case VK_FORMAT_R8_SINT:											return "VK_FORMAT_R8_SINT";
	case VK_FORMAT_R8_SRGB:											return "VK_FORMAT_R8_SRGB";
	case VK_FORMAT_R8G8_UNORM:										return "VK_FORMAT_R8G8_UNORM";
	case VK_FORMAT_R8G8_SNORM:										return "VK_FORMAT_R8G8_SNORM";
	case VK_FORMAT_R8G8_USCALED:									return "VK_FORMAT_R8G8_USCALED";
	case VK_FORMAT_R8G8_SSCALED:									return "VK_FORMAT_R8G8_SSCALED";
	case VK_FORMAT_R8G8_UINT:										return "VK_FORMAT_R8G8_UINT";
	case VK_FORMAT_R8G8_SINT:										return "VK_FORMAT_R8G8_SINT";
	case VK_FORMAT_R8G8_SRGB:										return "VK_FORMAT_R8G8_SRGB";
	case VK_FORMAT_R8G8B8_UNORM:									return "VK_FORMAT_R8G8B8_UNORM";
	case VK_FORMAT_R8G8B8_SNORM:									return "VK_FORMAT_R8G8B8_SNORM";
	case VK_FORMAT_R8G8B8_USCALED:									return "VK_FORMAT_R8G8B8_USCALED";
	case VK_FORMAT_R8G8B8_SSCALED:									return "VK_FORMAT_R8G8B8_SSCALED";
	case VK_FORMAT_R8G8B8_UINT:										return "VK_FORMAT_R8G8B8_UINT";
	case VK_FORMAT_R8G8B8_SINT:										return "VK_FORMAT_R8G8B8_SINT";
	case VK_FORMAT_R8G8B8_SRGB:										return "VK_FORMAT_R8G8B8_SRGB";
	case VK_FORMAT_B8G8R8_UNORM:									return "VK_FORMAT_B8G8R8_UNORM";
	case VK_FORMAT_B8G8R8_SNORM:									return "VK_FORMAT_B8G8R8_SNORM";
	case VK_FORMAT_B8G8R8_USCALED:									return "VK_FORMAT_B8G8R8_USCALED";
	case VK_FORMAT_B8G8R8_SSCALED:									return "VK_FORMAT_B8G8R8_SSCALED";
	case VK_FORMAT_B8G8R8_UINT:										return "VK_FORMAT_B8G8R8_UINT";
	case VK_FORMAT_B8G8R8_SINT:										return "VK_FORMAT_B8G8R8_SINT";
	case VK_FORMAT_B8G8R8_SRGB:										return "VK_FORMAT_B8G8R8_SRGB";
	case VK_FORMAT_R8G8B8A8_UNORM:									return "VK_FORMAT_R8G8B8A8_UNORM";
	case VK_FORMAT_R8G8B8A8_SNORM:									return "VK_FORMAT_R8G8B8A8_SNORM";
	case VK_FORMAT_R8G8B8A8_USCALED:								return "VK_FORMAT_R8G8B8A8_USCALED";
	case VK_FORMAT_R8G8B8A8_SSCALED:								return "VK_FORMAT_R8G8B8A8_SSCALED";
	case VK_FORMAT_R8G8B8A8_UINT:									return "VK_FORMAT_R8G8B8A8_UINT";
	case VK_FORMAT_R8G8B8A8_SINT:									return "VK_FORMAT_R8G8B8A8_SINT";
	case VK_FORMAT_R8G8B8A8_SRGB:									return "VK_FORMAT_R8G8B8A8_SRGB";
	case VK_FORMAT_B8G8R8A8_UNORM:									return "VK_FORMAT_B8G8R8A8_UNORM";
	case VK_FORMAT_B8G8R8A8_SNORM:									return "VK_FORMAT_B8G8R8A8_SNORM";
	case VK_FORMAT_B8G8R8A8_USCALED:								return "VK_FORMAT_B8G8R8A8_USCALED";
	case VK_FORMAT_B8G8R8A8_SSCALED:								return "VK_FORMAT_B8G8R8A8_SSCALED";
	case VK_FORMAT_B8G8R8A8_UINT:									return "VK_FORMAT_B8G8R8A8_UINT";
	case VK_FORMAT_B8G8R8A8_SINT:									return "VK_FORMAT_B8G8R8A8_SINT";
	case VK_FORMAT_B8G8R8A8_SRGB:									return "VK_FORMAT_B8G8R8A8_SRGB";
	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:							return "VK_FORMAT_A8B8G8R8_UNORM_PACK32";
	case VK_FORMAT_A8B8G8R8_SNORM_PACK32:							return "VK_FORMAT_A8B8G8R8_SNORM_PACK32";
	case VK_FORMAT_A8B8G8R8_USCALED_PACK32:							return "VK_FORMAT_A8B8G8R8_USCALED_PACK32";
	case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:							return "VK_FORMAT_A8B8G8R8_SSCALED_PACK32";
	case VK_FORMAT_A8B8G8R8_UINT_PACK32:							return "VK_FORMAT_A8B8G8R8_UINT_PACK32";
	case VK_FORMAT_A8B8G8R8_SINT_PACK32:							return "VK_FORMAT_A8B8G8R8_SINT_PACK32";
	case VK_FORMAT_A8B8G8R8_SRGB_PACK32:							return "VK_FORMAT_A8B8G8R8_SRGB_PACK32";
	case VK_FORMAT_A2R10G10B10_UNORM_PACK32:						return "VK_FORMAT_A2R10G10B10_UNORM_PACK32";
	case VK_FORMAT_A2R10G10B10_SNORM_PACK32:						return "VK_FORMAT_A2R10G10B10_SNORM_PACK32";
	case VK_FORMAT_A2R10G10B10_USCALED_PACK32:						return "VK_FORMAT_A2R10G10B10_USCALED_PACK32";
	case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:						return "VK_FORMAT_A2R10G10B10_SSCALED_PACK32";
	case VK_FORMAT_A2R10G10B10_UINT_PACK32:							return "VK_FORMAT_A2R10G10B10_UINT_PACK32";
	case VK_FORMAT_A2R10G10B10_SINT_PACK32:							return "VK_FORMAT_A2R10G10B10_SINT_PACK32";
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32:						return "VK_FORMAT_A2B10G10R10_UNORM_PACK32";
	case VK_FORMAT_A2B10G10R10_SNORM_PACK32:						return "VK_FORMAT_A2B10G10R10_SNORM_PACK32";
	case VK_FORMAT_A2B10G10R10_USCALED_PACK32:						return "VK_FORMAT_A2B10G10R10_USCALED_PACK32";
	case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:						return "VK_FORMAT_A2B10G10R10_SSCALED_PACK32";
	case VK_FORMAT_A2B10G10R10_UINT_PACK32:							return "VK_FORMAT_A2B10G10R10_UINT_PACK32";
	case VK_FORMAT_A2B10G10R10_SINT_PACK32:							return "VK_FORMAT_A2B10G10R10_SINT_PACK32";
	case VK_FORMAT_R16_UNORM:										return "VK_FORMAT_R16_UNORM";
	case VK_FORMAT_R16_SNORM:										return "VK_FORMAT_R16_SNORM";
	case VK_FORMAT_R16_USCALED:										return "VK_FORMAT_R16_USCALED";
	case VK_FORMAT_R16_SSCALED:										return "VK_FORMAT_R16_SSCALED";
	case VK_FORMAT_R16_UINT:										return "VK_FORMAT_R16_UINT";
	case VK_FORMAT_R16_SINT:										return "VK_FORMAT_R16_SINT";
	case VK_FORMAT_R16_SFLOAT:										return "VK_FORMAT_R16_SFLOAT";
	case VK_FORMAT_R16G16_UNORM:									return "VK_FORMAT_R16G16_UNORM";
	case VK_FORMAT_R16G16_SNORM:									return "VK_FORMAT_R16G16_SNORM";
	case VK_FORMAT_R16G16_USCALED:									return "VK_FORMAT_R16G16_USCALED";
	case VK_FORMAT_R16G16_SSCALED:									return "VK_FORMAT_R16G16_SSCALED";
	case VK_FORMAT_R16G16_UINT:										return "VK_FORMAT_R16G16_UINT";
	case VK_FORMAT_R16G16_SINT:										return "VK_FORMAT_R16G16_SINT";
	case VK_FORMAT_R16G16_SFLOAT:									return "VK_FORMAT_R16G16_SFLOAT";
	case VK_FORMAT_R16G16B16_UNORM:									return "VK_FORMAT_R16G16B16_UNORM";
	case VK_FORMAT_R16G16B16_SNORM:									return "VK_FORMAT_R16G16B16_SNORM";
	case VK_FORMAT_R16G16B16_USCALED:								return "VK_FORMAT_R16G16B16_USCALED";
	case VK_FORMAT_R16G16B16_SSCALED:								return "VK_FORMAT_R16G16B16_SSCALED";
	case VK_FORMAT_R16G16B16_UINT:									return "VK_FORMAT_R16G16B16_UINT";
	case VK_FORMAT_R16G16B16_SINT:									return "VK_FORMAT_R16G16B16_SINT";
	case VK_FORMAT_R16G16B16_SFLOAT:								return "VK_FORMAT_R16G16B16_SFLOAT";
	case VK_FORMAT_R16G16B16A16_UNORM:								return "VK_FORMAT_R16G16B16A16_UNORM";
	case VK_FORMAT_R16G16B16A16_SNORM:								return "VK_FORMAT_R16G16B16A16_SNORM";
	case VK_FORMAT_R16G16B16A16_USCALED:							return "VK_FORMAT_R16G16B16A16_USCALED";
	case VK_FORMAT_R16G16B16A16_SSCALED:							return "VK_FORMAT_R16G16B16A16_SSCALED";
	case VK_FORMAT_R16G16B16A16_UINT:								return "VK_FORMAT_R16G16B16A16_UINT";
	case VK_FORMAT_R16G16B16A16_SINT:								return "VK_FORMAT_R16G16B16A16_SINT";
	case VK_FORMAT_R16G16B16A16_SFLOAT:								return "VK_FORMAT_R16G16B16A16_SFLOAT";
	case VK_FORMAT_R32_UINT:										return "VK_FORMAT_R32_UINT";
	case VK_FORMAT_R32_SINT:										return "VK_FORMAT_R32_SINT";
	case VK_FORMAT_R32_SFLOAT:										return "VK_FORMAT_R32_SFLOAT";
	case VK_FORMAT_R32G32_UINT:										return "VK_FORMAT_R32G32_UINT";
	case VK_FORMAT_R32G32_SINT:										return "VK_FORMAT_R32G32_SINT";
	case VK_FORMAT_R32G32_SFLOAT:									return "VK_FORMAT_R32G32_SFLOAT";
	case VK_FORMAT_R32G32B32_UINT:									return "VK_FORMAT_R32G32B32_UINT";
	case VK_FORMAT_R32G32B32_SINT:									return "VK_FORMAT_R32G32B32_SINT";
	case VK_FORMAT_R32G32B32_SFLOAT:								return "VK_FORMAT_R32G32B32_SFLOAT";
	case VK_FORMAT_R32G32B32A32_UINT:								return "VK_FORMAT_R32G32B32A32_UINT";
	case VK_FORMAT_R32G32B32A32_SINT:								return "VK_FORMAT_R32G32B32A32_SINT";
	case VK_FORMAT_R32G32B32A32_SFLOAT:								return "VK_FORMAT_R32G32B32A32_SFLOAT";
	case VK_FORMAT_R64_UINT:										return "VK_FORMAT_R64_UINT";
	case VK_FORMAT_R64_SINT:										return "VK_FORMAT_R64_SINT";
	case VK_FORMAT_R64_SFLOAT:										return "VK_FORMAT_R64_SFLOAT";
	case VK_FORMAT_R64G64_UINT:										return "VK_FORMAT_R64G64_UINT";
	case VK_FORMAT_R64G64_SINT:										return "VK_FORMAT_R64G64_SINT";
	case VK_FORMAT_R64G64_SFLOAT:									return "VK_FORMAT_R64G64_SFLOAT";
	case VK_FORMAT_R64G64B64_UINT:									return "VK_FORMAT_R64G64B64_UINT";
	case VK_FORMAT_R64G64B64_SINT:									return "VK_FORMAT_R64G64B64_SINT";
	case VK_FORMAT_R64G64B64_SFLOAT:								return "VK_FORMAT_R64G64B64_SFLOAT";
	case VK_FORMAT_R64G64B64A64_UINT:								return "VK_FORMAT_R64G64B64A64_UINT";
	case VK_FORMAT_R64G64B64A64_SINT:								return "VK_FORMAT_R64G64B64A64_SINT";
	case VK_FORMAT_R64G64B64A64_SFLOAT:								return "VK_FORMAT_R64G64B64A64_SFLOAT";
	case VK_FORMAT_B10G11R11_UFLOAT_PACK32:							return "VK_FORMAT_B10G11R11_UFLOAT_PACK32";
	case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:							return "VK_FORMAT_E5B9G9R9_UFLOAT_PACK32";
	case VK_FORMAT_D16_UNORM:										return "VK_FORMAT_D16_UNORM";
	case VK_FORMAT_X8_D24_UNORM_PACK32:								return "VK_FORMAT_X8_D24_UNORM_PACK32";
	case VK_FORMAT_D32_SFLOAT:										return "VK_FORMAT_D32_SFLOAT";
	case VK_FORMAT_S8_UINT:											return "VK_FORMAT_S8_UINT";
	case VK_FORMAT_D16_UNORM_S8_UINT:								return "VK_FORMAT_D16_UNORM_S8_UINT";
	case VK_FORMAT_D24_UNORM_S8_UINT:								return "VK_FORMAT_D24_UNORM_S8_UINT";
	case VK_FORMAT_D32_SFLOAT_S8_UINT:								return "VK_FORMAT_D32_SFLOAT_S8_UINT";
	case VK_FORMAT_BC1_RGB_UNORM_BLOCK:								return "VK_FORMAT_BC1_RGB_UNORM_BLOCK";
	case VK_FORMAT_BC1_RGB_SRGB_BLOCK:								return "VK_FORMAT_BC1_RGB_SRGB_BLOCK";
	case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:							return "VK_FORMAT_BC1_RGBA_UNORM_BLOCK";
	case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:								return "VK_FORMAT_BC1_RGBA_SRGB_BLOCK";
	case VK_FORMAT_BC2_UNORM_BLOCK:									return "VK_FORMAT_BC2_UNORM_BLOCK";
	case VK_FORMAT_BC2_SRGB_BLOCK:									return "VK_FORMAT_BC2_SRGB_BLOCK";
	case VK_FORMAT_BC3_UNORM_BLOCK:									return "VK_FORMAT_BC3_UNORM_BLOCK";
	case VK_FORMAT_BC3_SRGB_BLOCK:									return "VK_FORMAT_BC3_SRGB_BLOCK";
	case VK_FORMAT_BC4_UNORM_BLOCK:									return "VK_FORMAT_BC4_UNORM_BLOCK";
	case VK_FORMAT_BC4_SNORM_BLOCK:									return "VK_FORMAT_BC4_SNORM_BLOCK";
	case VK_FORMAT_BC5_UNORM_BLOCK:									return "VK_FORMAT_BC5_UNORM_BLOCK";
	case VK_FORMAT_BC5_SNORM_BLOCK:									return "VK_FORMAT_BC5_SNORM_BLOCK";
	case VK_FORMAT_BC6H_UFLOAT_BLOCK:								return "VK_FORMAT_BC6H_UFLOAT_BLOCK";
	case VK_FORMAT_BC6H_SFLOAT_BLOCK:								return "VK_FORMAT_BC6H_SFLOAT_BLOCK";
	case VK_FORMAT_BC7_UNORM_BLOCK:									return "VK_FORMAT_BC7_UNORM_BLOCK";
	case VK_FORMAT_BC7_SRGB_BLOCK:									return "VK_FORMAT_BC7_SRGB_BLOCK";
	case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:							return "VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK";
	case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:							return "VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK";
	case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:						return "VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK";
	case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:						return "VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK";
	case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:						return "VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK";
	case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:						return "VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK";
	case VK_FORMAT_EAC_R11_UNORM_BLOCK:								return "VK_FORMAT_EAC_R11_UNORM_BLOCK";
	case VK_FORMAT_EAC_R11_SNORM_BLOCK:								return "VK_FORMAT_EAC_R11_SNORM_BLOCK";
	case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:							return "VK_FORMAT_EAC_R11G11_UNORM_BLOCK";
	case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:							return "VK_FORMAT_EAC_R11G11_SNORM_BLOCK";
	case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:							return "VK_FORMAT_ASTC_4x4_UNORM_BLOCK";
	case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:								return "VK_FORMAT_ASTC_4x4_SRGB_BLOCK";
	case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:							return "VK_FORMAT_ASTC_5x4_UNORM_BLOCK";
	case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:								return "VK_FORMAT_ASTC_5x4_SRGB_BLOCK";
	case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:							return "VK_FORMAT_ASTC_5x5_UNORM_BLOCK";
	case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:								return "VK_FORMAT_ASTC_5x5_SRGB_BLOCK";
	case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:							return "VK_FORMAT_ASTC_6x5_UNORM_BLOCK";
	case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:								return "VK_FORMAT_ASTC_6x5_SRGB_BLOCK";
	case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:							return "VK_FORMAT_ASTC_6x6_UNORM_BLOCK";
	case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:								return "VK_FORMAT_ASTC_6x6_SRGB_BLOCK";
	case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:							return "VK_FORMAT_ASTC_8x5_UNORM_BLOCK";
	case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:								return "VK_FORMAT_ASTC_8x5_SRGB_BLOCK";
	case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:							return "VK_FORMAT_ASTC_8x6_UNORM_BLOCK";
	case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:								return "VK_FORMAT_ASTC_8x6_SRGB_BLOCK";
	case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:							return "VK_FORMAT_ASTC_8x8_UNORM_BLOCK";
	case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:								return "VK_FORMAT_ASTC_8x8_SRGB_BLOCK";
	case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:							return "VK_FORMAT_ASTC_10x5_UNORM_BLOCK";
	case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:							return "VK_FORMAT_ASTC_10x5_SRGB_BLOCK";
	case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:							return "VK_FORMAT_ASTC_10x6_UNORM_BLOCK";
	case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:							return "VK_FORMAT_ASTC_10x6_SRGB_BLOCK";
	case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:							return "VK_FORMAT_ASTC_10x8_UNORM_BLOCK";
	case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:							return "VK_FORMAT_ASTC_10x8_SRGB_BLOCK";
	case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:							return "VK_FORMAT_ASTC_10x10_UNORM_BLOCK";
	case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:							return "VK_FORMAT_ASTC_10x10_SRGB_BLOCK";
	case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:							return "VK_FORMAT_ASTC_12x10_UNORM_BLOCK";
	case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:							return "VK_FORMAT_ASTC_12x10_SRGB_BLOCK";
	case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:							return "VK_FORMAT_ASTC_12x12_UNORM_BLOCK";
	case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:							return "VK_FORMAT_ASTC_12x12_SRGB_BLOCK";
	case VK_FORMAT_G8B8G8R8_422_UNORM:								return "VK_FORMAT_G8B8G8R8_422_UNORM";
	case VK_FORMAT_B8G8R8G8_422_UNORM:								return "VK_FORMAT_B8G8R8G8_422_UNORM";
	case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:						return "VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM";
	case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:						return "VK_FORMAT_G8_B8R8_2PLANE_420_UNORM";
	case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:						return "VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM";
	case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:						return "VK_FORMAT_G8_B8R8_2PLANE_422_UNORM";
	case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:						return "VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM";
	case VK_FORMAT_R10X6_UNORM_PACK16:								return "VK_FORMAT_R10X6_UNORM_PACK16";
	case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:						return "VK_FORMAT_R10X6G10X6_UNORM_2PACK16";
	case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:				return "VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16";
	case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:			return "VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16";
	case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:			return "VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16";
	case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:		return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16";
	case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:		return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16";
	case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:		return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16";
	case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:		return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16";
	case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:		return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16";
	case VK_FORMAT_R12X4_UNORM_PACK16:								return "VK_FORMAT_R12X4_UNORM_PACK16";
	case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:						return "VK_FORMAT_R12X4G12X4_UNORM_2PACK16";
	case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:				return "VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16";
	case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:			return "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16";
	case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:			return "VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16";
	case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:		return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16";
	case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:		return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16";
	case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:		return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16";
	case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:		return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16";
	case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:		return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16";
	case VK_FORMAT_G16B16G16R16_422_UNORM:							return "VK_FORMAT_G16B16G16R16_422_UNORM";
	case VK_FORMAT_B16G16R16G16_422_UNORM:							return "VK_FORMAT_B16G16R16G16_422_UNORM";
	case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:					return "VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM";
	case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:						return "VK_FORMAT_G16_B16R16_2PLANE_420_UNORM";
	case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:					return "VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM";
	case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:						return "VK_FORMAT_G16_B16R16_2PLANE_422_UNORM";
	case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:					return "VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM";
	case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:						return "VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG";
	case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:						return "VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG";
	case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:						return "VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG";
	case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:						return "VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG";
	case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:						return "VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG";
	case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:						return "VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG";
	case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:						return "VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG";
	case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:						return "VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG";
	case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT:						return "VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT";
	case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT:						return "VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT";
	case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT:						return "VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT";
	case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT:						return "VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT";
	case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT:						return "VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT";
	case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT:						return "VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT";
	case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT:						return "VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT";
	case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT:						return "VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT";
	case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT:						return "VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT";
	case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT:						return "VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT";
	case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT:						return "VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT";
	case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT:						return "VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT";
	case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT:						return "VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT";
	case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT:						return "VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT";
	case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT:					return "VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT";
	case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT:	return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT";
	case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT:	return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT";
	case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT:					return "VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT";
	case VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT:						return "VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT";
	case VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT:						return "VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT";
	default:														return "UNKNOWN";
	}
}

/**
 * @ingroup studioapi_vk
 * @brief Convert Vulkan color space to human readable text
 *
 * @param vkColorSpace		Vulkan color space
 * @return Return Vulkan color space in human readable text
 */
FORCEINLINE const achar* VK_ConvVkColorSpaceToText( VkColorSpaceKHR vkColorSpace )
{
	switch ( vkColorSpace )
	{
	case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:				return "VK_COLOR_SPACE_SRGB_NONLINEAR_KHR";
	case VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT:		return "VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT";
	case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:		return "VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT";
	case VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT:			return "VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT";
	case VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT:			return "VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT";
	case VK_COLOR_SPACE_BT709_LINEAR_EXT:				return "VK_COLOR_SPACE_BT709_LINEAR_EXT";
	case VK_COLOR_SPACE_BT709_NONLINEAR_EXT:			return "VK_COLOR_SPACE_BT709_NONLINEAR_EXT";
	case VK_COLOR_SPACE_BT2020_LINEAR_EXT:				return "VK_COLOR_SPACE_BT2020_LINEAR_EXT";
	case VK_COLOR_SPACE_HDR10_ST2084_EXT:				return "VK_COLOR_SPACE_HDR10_ST2084_EXT";
	case VK_COLOR_SPACE_DOLBYVISION_EXT:				return "VK_COLOR_SPACE_DOLBYVISION_EXT";
	case VK_COLOR_SPACE_HDR10_HLG_EXT:					return "VK_COLOR_SPACE_HDR10_HLG_EXT";
	case VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT:			return "VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT";
	case VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT:			return "VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT";
	case VK_COLOR_SPACE_PASS_THROUGH_EXT:				return "VK_COLOR_SPACE_PASS_THROUGH_EXT";
	case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT:	return "VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT";
	case VK_COLOR_SPACE_DISPLAY_NATIVE_AMD:				return "VK_COLOR_SPACE_DISPLAY_NATIVE_AMD";
	case VK_COLOR_SPACE_MAX_ENUM_KHR:					return "VK_COLOR_SPACE_MAX_ENUM_KHR";
	default:											return "UNKNOWN";
	}
}

#endif // !VK_HELPERS_H