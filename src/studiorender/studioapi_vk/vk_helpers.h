#pragma once
#include "Volk/volk.h"
#include "tier0/defines.h"
#include "tier0/types.h"

//-----------------------------------------------------------------------------
// Vulkan macrosses and constants
//-----------------------------------------------------------------------------
#define STUDIOAPI_VK_VERIFY_RESULT( vkFunction )                               \
	{                                                                          \
		VkResult __vkResult__ = vkFunction;                                    \
		if ( __vkResult__ != VK_SUCCESS )                                      \
		{                                                                      \
			VK_CheckVkResult( __vkResult__, #vkFunction, __FILE__, __LINE__ ); \
		}                                                                      \
	}

#define STUDIOAPI_VK_DESCRIPTOR_TYPE_BEGIN_RANGE			   VK_DESCRIPTOR_TYPE_SAMPLER
#define STUDIOAPI_VK_DESCRIPTOR_TYPE_END_RANGE				   VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
#define STUDIOAPI_VK_DESCRIPTOR_TYPE_RANGE_SIZE				   ( STUDIOAPI_VK_DESCRIPTOR_TYPE_END_RANGE - STUDIOAPI_VK_DESCRIPTOR_TYPE_BEGIN_RANGE + 1 )
#define STUDIOAPI_VK_MAX_VERTEX_ELEMENT_COUNT				   17
#define STUDIOAPI_VK_NUM_FRAMES_TO_WAIT_BEFORE_RELEASING_TO_OS 3
#define STUDIOAPI_VK_TEMPALLOC_BLOCK_SIZE					   ( 4 * 1024 * 1024 )
#define STUDIOAPI_VK_MAX_PUSH_CONSTANT_SIZE					   128
#define STUDIOAPI_VK_BUFFER_OFFSET_ALIGNMENT				   4
#define STUDIOAPI_VK_CMDBUFFER_TIME_TO_WAIT_BEFORE_DELETING	   10
#define STUDIOAPI_VK_CMDLIST_TIME_TO_WAIT_BEFORE_DELETING	   10
#define STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT					   3
#define STUDIOAPI_VK_MAX_NUM_PENDING_BUFFER_BARRIERS		   64
#define STUDIOAPI_VK_MAX_NUM_PENDING_IMAGE_BARRIERS			   128
#define STUDIOAPI_VK_MAX_NUM_PENDING_MEMORY_BARRIERS		   64
#define STUDIOAPI_VK_ACCESS_READ_FLAGS						   ( 0                                                         \
																 | VK_ACCESS_INDIRECT_COMMAND_READ_BIT                     \
																 | VK_ACCESS_INDEX_READ_BIT                                \
																 | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT                     \
																 | VK_ACCESS_UNIFORM_READ_BIT                              \
																 | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT                     \
																 | VK_ACCESS_SHADER_READ_BIT                               \
																 | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT                     \
																 | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT             \
																 | VK_ACCESS_TRANSFER_READ_BIT                             \
																 | VK_ACCESS_HOST_READ_BIT                                 \
																 | VK_ACCESS_MEMORY_READ_BIT                               \
																 | VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT       \
																 | VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT            \
																 | VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT     \
																 | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR           \
																 | VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT             \
																 | VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR \
																 | VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV                \
																 | VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV                \
																 | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV )
#define STUDIOAPI_VK_ACCESS_WRITE_FLAGS						   ( 0                                                    \
																 | VK_ACCESS_SHADER_WRITE_BIT                         \
																 | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT               \
																 | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT       \
																 | VK_ACCESS_TRANSFER_WRITE_BIT                       \
																 | VK_ACCESS_HOST_WRITE_BIT                           \
																 | VK_ACCESS_MEMORY_WRITE_BIT                         \
																 | VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT         \
																 | VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT \
																 | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR     \
																 | VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV          \
																 | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV )
#define STUDIOAPI_VK_ACCESS_KNOWN_FLAGS						   ( STUDIOAPI_VK_ACCESS_READ_FLAGS | STUDIOAPI_VK_ACCESS_WRITE_FLAGS )
#define STUDIOAPI_VK_BUFFER_KNOWN_STATES					   ( 0                                                 \
																 | STUDIOAPI_BUFFER_STATE_UNDEFINED                \
																 | STUDIOAPI_BUFFER_STATE_SHADER_RESOURCE          \
																 | STUDIOAPI_BUFFER_STATE_SHADER_RESOURCE_READONLY \
																 | STUDIOAPI_BUFFER_STATE_TRANSFER_SRC             \
																 | STUDIOAPI_BUFFER_STATE_TRANSFER_DST             \
																 | STUDIOAPI_BUFFER_STATE_VERTEX_BUFFER            \
																 | STUDIOAPI_BUFFER_STATE_INDEX_BUFFER             \
																 | STUDIOAPI_BUFFER_STATE_CONSTANT_BUFFER          \
																 | STUDIOAPI_BUFFER_STATE_STRUCTURED_BUFFER )
#define STUDIOAPI_VK_SHADER_PLATFORM_NAME					   "vk"

//-----------------------------------------------------------------------------
// Vulkan functions
//-----------------------------------------------------------------------------
void VK_CheckVkResult( VkResult vkResult, const char* pVkFunction, const char* pFileName, uint32 line );
bool VK_CheckInstanceExtensionsSupported( const char** pInstanceExtensionNames, uint32 instanceExtensionNum );
bool VK_CheckValidationLayersSupported( const char** pValidationLayerNames, uint32 validationLayerNum );
bool VK_CheckDeviceExtensionsSupported( VkPhysicalDevice vkPhysicalDevice, const char** pDeviceExtensionNames, uint32 deviceExtensionNum );

#if !RETAIL
// For more about parameters see https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/PFN_vkDebugUtilsMessengerCallbackEXT.html
VkBool32 VKAPI_PTR VK_DebugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData );
void			   VK_SetDebugName( VkObjectType vkObjectType, uint64 handle, const char* pName );
#endif	// !RETAIL

// This is a helper function to try to figure out what stage a resource is (likely) used at given it's current Vulkan access mask. It's usually pretty accurate
VkPipelineStageFlags VK_TranslateVkAccessFlagsToVkPipelineStageFlags( VkAccessFlags vkAccessFlags );
VkAccessFlags		 VK_TranslateVkImageLayoutToVkAccessFlags( VkImageLayout vkImageLayout );

// Copy A and B with return TRUE if they aren't equal
template<typename TType>
bool VK_CopyAndReturnNotEqual( TType& a, TType b );

bool	 VK_IsDepthPixelFormat( studioAPIPixelFormat_t pixelFormat );
bool	 VK_IsStencilPixelFormat( studioAPIPixelFormat_t pixelFormat );
VkFormat VK_TranslatePixelFormat( studioAPIPixelFormat_t pixelFormat );

VkImageLayout	   VK_TranslateTextureLayout( studioAPITextureLayout_t textureLayout );
VkImageAspectFlags VK_TranslateImageAspectFlags( studioAPIPixelFormat_t pixelFormat );

const char* VK_ConvVkResultToText( VkResult vkResult );
const char* VK_ConvVkPresentModeToText( VkPresentModeKHR vkPresentMode );
const char* VK_ConvVkFormatToText( VkFormat vkFormat );
const char* VK_ConvVkColorSpaceToText( VkColorSpaceKHR vkColorSpace );

#include "studiorender/studioapi_vk/vk_helpers.inl"
