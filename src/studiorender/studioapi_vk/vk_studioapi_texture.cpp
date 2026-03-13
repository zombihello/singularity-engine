#include "pch_studioapi_vk.h"
#include "utils/pixelformatinfos/pixelformatinfos.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdcontext.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdbuffer.h"
#include "studiorender/studioapi_vk/vk_studioapi_texture.h"

/*
==================
VK_TranslateSamplerAddressMode
==================
*/
static VkSamplerAddressMode VK_TranslateSamplerAddressMode( studioAPISamplerAddressMode_t samplerAddressMode )
{
	switch ( samplerAddressMode )
	{
	case STUDIOAPI_SAMPLER_ADDRESS_MODE_CLAMP: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	case STUDIOAPI_SAMPLER_ADDRESS_MODE_MIRROR_WRAP: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	case STUDIOAPI_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
	case STUDIOAPI_SAMPLER_ADDRESS_MODE_BORDER: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	default: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	}
}

/*
==================
VK_TranslateSamplerFilter
==================
*/
static void VK_TranslateSamplerFilter( studioAPISamplerFilter_t samplerFilter, VkFilter& vkMagFilter, VkFilter& vkMinFilter, VkSamplerMipmapMode& vkMipmapMode )
{
	switch ( samplerFilter )
	{
	case STUDIOAPI_SAMPLER_FILTER_BILINEAR:
		vkMagFilter	 = VK_FILTER_LINEAR;
		vkMinFilter	 = VK_FILTER_LINEAR;
		vkMipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		break;

	case STUDIOAPI_SAMPLER_FILTER_TRILINEAR:
	case STUDIOAPI_SAMPLER_FILTER_ANISOTROPIC:
		vkMagFilter	 = VK_FILTER_LINEAR;
		vkMinFilter	 = VK_FILTER_LINEAR;
		vkMipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		break;

	default:
		vkMagFilter	 = VK_FILTER_NEAREST;
		vkMinFilter	 = VK_FILTER_NEAREST;
		vkMipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		break;
	}
}

/*
==================
Vk_TranslateSamplerCompareOp
==================
*/
static VkCompareOp Vk_TranslateSamplerCompareOp( studioAPISamplerCompareOp_t samplerCompareOp )
{
	switch ( samplerCompareOp )
	{
	case STUDIOAPI_SAMPLER_COMPARE_OP_LESS: return VK_COMPARE_OP_LESS;
	default: return VK_COMPARE_OP_NEVER;
	}
}

/*
==================
VK_TranslateTextureUsageFlags
==================
*/
static void VK_TranslateTextureUsageFlags( uint32& usageFlags, studioAPIPixelFormat_t pixelFormat, VkImageUsageFlags& vkImageUsageFlags, VmaMemoryUsage& vmaMemoryUsage, uint8& memoryFlags )
{
	// Clear output parameters
	vkImageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT;
	vmaMemoryUsage	  = VMA_MEMORY_USAGE_UNKNOWN;

	// Translate texture usage flags
	// Texture
	if ( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_TEXTURE )
	{
		vkImageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	// Render target
	if ( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_RENDER_TARGET )
	{
		vkImageUsageFlags |= VK_IsDepthPixelFormat( pixelFormat ) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}

	// Unordered access
	if ( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_UNORDERED_ACCESS )
	{
		vkImageUsageFlags |= VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	// Transfer source
	if ( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_SRC )
	{
		vkImageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}

	// Transfer destination
	if ( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_DST )
	{
		vkImageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	// Select the right memory type
	if ( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_LINEAR )
	{
		vmaMemoryUsage = VMA_MEMORY_USAGE_GPU_TO_CPU;
		memoryFlags	   = STUDIOAPI_TEXTURE_MEMORY_FLAG_CPU_GPU;
	}
	else
	{
		vmaMemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		memoryFlags	   = STUDIOAPI_TEXTURE_MEMORY_FLAG_GPU;

		usageFlags |= STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_DST;
		vkImageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}
}

/*
==================
VK_TranslateImageType
==================
*/
static VkImageType VK_TranslateImageType( studioAPITextureType_t type )
{
	switch ( type )
	{
	case STUDIOAPI_TEXTURE_TYPE_1D:
	case STUDIOAPI_TEXTURE_TYPE_1D_ARRAY:
		return VK_IMAGE_TYPE_1D;

	case STUDIOAPI_TEXTURE_TYPE_2D:
	case STUDIOAPI_TEXTURE_TYPE_2D_ARRAY:
	case STUDIOAPI_TEXTURE_TYPE_CUBE:
	case STUDIOAPI_TEXTURE_TYPE_CUBE_ARRAY:
		return VK_IMAGE_TYPE_2D;

	case STUDIOAPI_TEXTURE_TYPE_3D:
		return VK_IMAGE_TYPE_3D;

	default:
		AssertMsg( false, "Unknown StudioAPI texture type (0x%X)", type );
		return VK_IMAGE_TYPE_MAX_ENUM;
	}
}

/*
==================
VK_GetUsedQueueFamiliesFromTextureUsage
==================
*/
void VK_GetUsedQueueFamiliesFromTextureUsage( uint32 usageFlags, uint32& graphicsQueueFamilyIndex, uint32& computeQueueFamilyIndex, uint32& transferQueueFamilyIndex )
{
	graphicsQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	computeQueueFamilyIndex	 = VK_QUEUE_FAMILY_IGNORED;
	transferQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	// Do nothing if we haven't STUDIOAPI_TEXTURE_USAGE_FLAG_CONCURRENT
	if ( !( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_CONCURRENT ) )
	{
		return;
	}

	// Graphics queue family
	if ( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_TEXTURE || usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_RENDER_TARGET || usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_UNORDERED_ACCESS )
	{
		graphicsQueueFamilyIndex = g_StudioAPIVk.GetDevice().GetGraphicsQueue().GetQueueFamilyIndex();
	}

	// Compute queue family
	if ( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_UNORDERED_ACCESS )
	{
		computeQueueFamilyIndex = g_StudioAPIVk.GetDevice().GetComputeQueue().GetQueueFamilyIndex();
	}

	// Transfer queue family
	if ( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_SRC || usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_DST || !( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_LINEAR ) )
	{
		transferQueueFamilyIndex = g_StudioAPIVk.GetDevice().GetTransferQueue().GetQueueFamilyIndex();
	}
}

/*
==================
VK_TranslateImageViewType
==================
*/
static VkImageViewType VK_TranslateImageViewType( studioAPITextureType_t type )
{
	switch ( type )
	{
	case STUDIOAPI_TEXTURE_TYPE_1D: return VK_IMAGE_VIEW_TYPE_1D;
	case STUDIOAPI_TEXTURE_TYPE_1D_ARRAY: return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
	case STUDIOAPI_TEXTURE_TYPE_2D: return VK_IMAGE_VIEW_TYPE_2D;
	case STUDIOAPI_TEXTURE_TYPE_2D_ARRAY: return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
	case STUDIOAPI_TEXTURE_TYPE_CUBE: return VK_IMAGE_VIEW_TYPE_CUBE;
	case STUDIOAPI_TEXTURE_TYPE_CUBE_ARRAY: return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
	case STUDIOAPI_TEXTURE_TYPE_3D: return VK_IMAGE_VIEW_TYPE_3D;
	default:
		AssertMsg( false, "Unknown StudioAPI texture type (0x%X)", type );
		return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
	}
}

/*
==================
CStudioAPISamplerVk::CStudioAPISamplerVk
==================
*/
CStudioAPISamplerVk::CStudioAPISamplerVk( const studioAPISamplerCreateInfo_t& createInfo, const char* pDebugName /* = "" */ )
	: vkSampler( VK_NULL_HANDLE )
	, onStudioAPIVkShutdownHandle( INVALID_HANDLE )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	VkSamplerCreateInfo vkSamplerCreateInfo		= {};
	vkSamplerCreateInfo.sType					= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	vkSamplerCreateInfo.addressModeU			= VK_TranslateSamplerAddressMode( createInfo.addressModeU );
	vkSamplerCreateInfo.addressModeV			= VK_TranslateSamplerAddressMode( createInfo.addressModeV );
	vkSamplerCreateInfo.addressModeW			= VK_TranslateSamplerAddressMode( createInfo.addressModeW );
	vkSamplerCreateInfo.mipLodBias				= createInfo.mipBias;
	vkSamplerCreateInfo.anisotropyEnable		= createInfo.filer == STUDIOAPI_SAMPLER_FILTER_ANISOTROPIC;
	vkSamplerCreateInfo.maxAnisotropy			= (float)createInfo.maxAnisotropy;
	vkSamplerCreateInfo.compareEnable			= createInfo.compareOp != STUDIOAPI_SAMPLER_COMPARE_OP_NEVER;
	vkSamplerCreateInfo.compareOp				= Vk_TranslateSamplerCompareOp( createInfo.compareOp );
	vkSamplerCreateInfo.minLod					= createInfo.minLod;
	vkSamplerCreateInfo.maxLod					= createInfo.maxLod;
	vkSamplerCreateInfo.borderColor				= VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
	vkSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	VK_TranslateSamplerFilter( createInfo.filer, vkSamplerCreateInfo.magFilter, vkSamplerCreateInfo.minFilter, vkSamplerCreateInfo.mipmapMode );
	STUDIOAPI_VK_VERIFY_RESULT( vkCreateSampler( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkSamplerCreateInfo, NULL, &vkSampler ) );

	// Register in 'onStudioAPIVkShutodwn' for destroy Vulkan objects when the one is shutdown
	onStudioAPIVkShutdownHandle = g_StudioAPIVk.OnStudioAPIVkShutdown().Subscribe( &CStudioAPISamplerVk::OnStudioAPIVkShutdown, this );
}

/*
==================
CStudioAPISamplerVk::~CStudioAPISamplerVk
==================
*/
CStudioAPISamplerVk::~CStudioAPISamplerVk()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Destroy the sampler
	if ( vkSampler != VK_NULL_HANDLE )
	{
		g_StudioAPIVk.GetMemoryMgr().FreeResource( [vkSampler = vkSampler]()
												   { vkDestroySampler( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkSampler, NULL ); } );
		vkSampler = VK_NULL_HANDLE;
	}

	// Remove CStudioAPITextureVk::OnStudioAPIVkShutdown from event 'onStudioAPIVkShutodwn'
	if ( onStudioAPIVkShutdownHandle != INVALID_HANDLE )
	{
		g_StudioAPIVk.OnStudioAPIVkShutdown().Unsubscribe( onStudioAPIVkShutdownHandle );
		onStudioAPIVkShutdownHandle = INVALID_HANDLE;
	}
}

/*
==================
CStudioAPISamplerVk::OnStudioAPIVkShutdown
==================
*/
void CStudioAPISamplerVk::OnStudioAPIVkShutdown( void* pUserData )
{
	Assert( pUserData );
	CStudioAPISamplerVk* pStudioAPISamplerVk		 = (CStudioAPISamplerVk*)pUserData;
	pStudioAPISamplerVk->onStudioAPIVkShutdownHandle = INVALID_HANDLE;
	pStudioAPISamplerVk->~CStudioAPISamplerVk();
}

/*
==================
CStudioAPITextureVk::CStudioAPITextureVk
==================
*/
CStudioAPITextureVk::CStudioAPITextureVk( studioAPITextureType_t type, uint32 sizeX, uint32 sizeY, uint32 sizeZ, uint32 numLayers, uint32 numMips, uint32 usageFlags, studioAPIPixelFormat_t pixelFormat, const byte* pData /* = NULL */, const char* pDebugName /* = "" */ )
	: usageFlags( usageFlags )
	, memoryFlags( 0 )
	, type( type )
	, pixelFormat( pixelFormat )
	, vkImageAspectFlags( VK_TranslateImageAspectFlags( pixelFormat ) )
	, sizeX( sizeX )
	, sizeY( sizeY )
	, sizeZ( sizeZ )
	, numLayers( numLayers )
	, numMips( numMips )
	, vkImage( VK_NULL_HANDLE )
	, vmaAllocation( VK_NULL_HANDLE )
	, onStudioAPIVkShutdownHandle( INVALID_HANDLE )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	AssertMsg( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_LINEAR ? numMips == 1 && numLayers == 1 : true, "A linear texture must have only 1 mip level and layer" );

	// Convert usage flags into VMA memory usage and Vulkan image usage flags
	VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
	VkImageUsageFlags		vkImageUsageFlags;
	VK_TranslateTextureUsageFlags( CStudioAPITextureVk::usageFlags, pixelFormat, vkImageUsageFlags, vmaAllocationCreateInfo.usage, memoryFlags );

	// Convert a pixel format into Vulkan format and get aspect flags
	VkFormat vkFormat = VK_TranslatePixelFormat( pixelFormat );

	// Allocate memory for image
	VkImageCreateInfo vkImageCreateInfo = {};
	vkImageCreateInfo.sType				= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	vkImageCreateInfo.imageType			= VK_TranslateImageType( type );
	vkImageCreateInfo.format			= vkFormat;
	vkImageCreateInfo.extent.width		= S_Max<uint32>( sizeX, 1 );
	vkImageCreateInfo.extent.height		= S_Max<uint32>( sizeY, 1 );
	vkImageCreateInfo.extent.depth		= S_Max<uint32>( sizeZ, 1 );
	vkImageCreateInfo.mipLevels			= S_Max<uint32>( numMips, 1 );
	vkImageCreateInfo.arrayLayers		= S_Max<uint32>( numLayers, 1 );
	vkImageCreateInfo.samples			= VK_SAMPLE_COUNT_1_BIT;
	vkImageCreateInfo.tiling			= usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_LINEAR ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;
	vkImageCreateInfo.usage				= vkImageUsageFlags;
	vkImageCreateInfo.initialLayout		= VK_IMAGE_LAYOUT_UNDEFINED;

	// Grab queue family indices which the texture should be use
	uint32 graphicsQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	uint32 computeQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
	uint32 transferQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	VK_GetUsedQueueFamiliesFromTextureUsage( usageFlags, graphicsQueueFamilyIndex, computeQueueFamilyIndex, transferQueueFamilyIndex );

	CStudioAPIQueueSharingModeSetupVk queueSharingModeSetup( graphicsQueueFamilyIndex, computeQueueFamilyIndex, transferQueueFamilyIndex );
	queueSharingModeSetup.Setup( vkImageCreateInfo.sharingMode, vkImageCreateInfo.queueFamilyIndexCount, vkImageCreateInfo.pQueueFamilyIndices );
	vmaAllocation = g_StudioAPIVk.GetMemoryMgr().AllocateImage( S_Sprintf( "Texture (type: 0x%X, %ix%ix%i, numLayers: %i, numMips: %i, pixelFormat: '%s', usageFlags: 0x%X)", type, sizeX, sizeY, sizeZ, numLayers, numMips, g_PixelFormatInfos[pixelFormat].pName, usageFlags ).c_str(), vkImageCreateInfo, vmaAllocationCreateInfo, vkImage );
	if ( vmaAllocation == VK_NULL_HANDLE )
	{
		Sys_Error( "Failed to allocate GPU texture (type: 0x%X, %ix%ix%i, numLayers: %i, numMips: %i, pixelFormat: '%s')", type, sizeX, sizeY, sizeZ, numLayers, numMips, g_PixelFormatInfos[pixelFormat].pName );
		return;
	}

	// Create a main image views
	VkImageViewCreateInfo vkImageViewCreateInfo		  = {};
	vkImageViewCreateInfo.sType						  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	vkImageViewCreateInfo.image						  = vkImage;
	vkImageViewCreateInfo.viewType					  = VK_TranslateImageViewType( type );
	vkImageViewCreateInfo.format					  = vkFormat;
	vkImageViewCreateInfo.components.r				  = VK_COMPONENT_SWIZZLE_R;
	vkImageViewCreateInfo.components.g				  = VK_COMPONENT_SWIZZLE_G;
	vkImageViewCreateInfo.components.b				  = VK_COMPONENT_SWIZZLE_B;
	vkImageViewCreateInfo.components.a				  = VK_COMPONENT_SWIZZLE_A;
	vkImageViewCreateInfo.subresourceRange.levelCount = numMips;
	vkImageViewCreateInfo.subresourceRange.layerCount = numLayers;

	// Create a main image views for depth/stencil
	if ( VK_IsDepthPixelFormat( pixelFormat ) )
	{
		// Only depth
		vkImageViews.resize( 3 );
		vkImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		STUDIOAPI_VK_VERIFY_RESULT( vkCreateImageView( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkImageViewCreateInfo, NULL, &vkImageViews[IMAGE_VIEW_INDEX_DEPTH_ONLY] ) );

		// Only stencil
		if ( VK_IsStencilPixelFormat( pixelFormat ) )
		{
			vkImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
			STUDIOAPI_VK_VERIFY_RESULT( vkCreateImageView( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkImageViewCreateInfo, NULL, &vkImageViews[IMAGE_VIEW_INDEX_STENCIL_ONLY] ) );
		}
		else
		{
			vkImageViews[IMAGE_VIEW_INDEX_STENCIL_ONLY] = vkImageViews[IMAGE_VIEW_INDEX_DEPTH_ONLY];
		}

		// Depth and stencil
		if ( VK_IsStencilPixelFormat( pixelFormat ) )
		{
			vkImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			STUDIOAPI_VK_VERIFY_RESULT( vkCreateImageView( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkImageViewCreateInfo, NULL, &vkImageViews[IMAGE_VIEW_INDEX_DEPTH_AND_STENCIL] ) );
		}
		else
		{
			vkImageViews[IMAGE_VIEW_INDEX_DEPTH_AND_STENCIL] = vkImageViews[IMAGE_VIEW_INDEX_DEPTH_ONLY];
		}
	}
	// Otherwise create a main image view for color texture
	else
	{
		// Color texture
		vkImageViews.resize( 1 );
		vkImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		STUDIOAPI_VK_VERIFY_RESULT( vkCreateImageView( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkImageViewCreateInfo, NULL, &vkImageViews[0] ) );
	}

	// Create an image view for each mip and layer (only for color render targets)
	if ( ( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_RENDER_TARGET ) && !VK_IsDepthPixelFormat( pixelFormat ) )
	{
		// We can't render into a cube directly, so threat it as a 2D array
		if ( vkImageViewCreateInfo.viewType == VK_IMAGE_VIEW_TYPE_CUBE || vkImageViewCreateInfo.viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY )
		{
			vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		}

		vkImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		vkImageViewCreateInfo.subresourceRange.levelCount = 1;
		vkImageViewCreateInfo.subresourceRange.layerCount = 1;
		Assert( numMips > 0 && numLayers > 0 );
		vkImageViews.resize( 1 + numMips * numLayers );

		for ( uint32 layerIdx = 0; layerIdx < numLayers; ++layerIdx )
		{
			for ( uint32 mipIdx = 0; mipIdx < numMips; ++mipIdx )
			{
				uint32 imageViewIndex								  = GetVkImageViewIndex( mipIdx, layerIdx );
				vkImageViewCreateInfo.subresourceRange.baseMipLevel	  = mipIdx;
				vkImageViewCreateInfo.subresourceRange.baseArrayLayer = layerIdx;
				STUDIOAPI_VK_VERIFY_RESULT( vkCreateImageView( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkImageViewCreateInfo, NULL, &vkImageViews[imageViewIndex] ) );
			}
		}
	}

	// Initialize synchronization states for each mip level and layer
	syncStates.resize( numMips * numLayers );

	// Copy data if we have it
	if ( pData )
	{
		UpdateData( g_StudioAPIVk.GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_GRAPHICS ), (byte*)pData );
	}

	// Register in 'onStudioAPIVkShutodwn' for destroy Vulkan objects when the one is shutdown
	onStudioAPIVkShutdownHandle = g_StudioAPIVk.OnStudioAPIVkShutdown().Subscribe( &CStudioAPITextureVk::OnStudioAPIVkShutdown, this );
}

/*
==================
CStudioAPITextureVk::~CStudioAPITextureVk
==================
*/
CStudioAPITextureVk::~CStudioAPITextureVk()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Destroy the image
	if ( vkImage != VK_NULL_HANDLE )
	{
		g_StudioAPIVk.GetMemoryMgr().FreeResource( [vkImageViews = vkImageViews, vkImage = vkImage, vmaAllocation = vmaAllocation]()
												   {
			for ( uint32 imageViewIdx = 0, numImageViews = (uint32)vkImageViews.size(); imageViewIdx < numImageViews; ++imageViewIdx )
			{
				vkDestroyImageView( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkImageViews[imageViewIdx], NULL );
			}
			g_StudioAPIVk.GetMemoryMgr().DestroyImage( vkImage, vmaAllocation ); } );
		vkImage		  = VK_NULL_HANDLE;
		vmaAllocation = VK_NULL_HANDLE;
		vkImageViews.clear();
	}

	// Remove CStudioAPITextureVk::OnStudioAPIVkShutdown from event 'onStudioAPIVkShutodwn'
	if ( onStudioAPIVkShutdownHandle != INVALID_HANDLE )
	{
		g_StudioAPIVk.OnStudioAPIVkShutdown().Unsubscribe( onStudioAPIVkShutdownHandle );
		onStudioAPIVkShutdownHandle = INVALID_HANDLE;
	}
}

/*
==================
CStudioAPITextureVk::UpdateData
==================
*/
void CStudioAPITextureVk::UpdateData( IStudioAPICmdContext* pCmdContext, byte* pData, uint32 startLayer /* = 0 */, uint32 startMip /* = 0 */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Copy the data by CStudioAPIDataUploaderVk (only for GPU only memory)
	if ( !( memoryFlags & STUDIOAPI_TEXTURE_MEMORY_FLAG_CPU ) )
	{
		// Upload state structure
		struct uploadState_t
		{
			uint32 currentLayer;	/**< Current layer */
			uint32 currentMip;		/**< Current mip level */
			uint32 currentMipRow;	/**< Current mip row */
			uint32 currentMipDepth; /**< Current mip depth */
		};

		// Make sure that we have STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_DST flag
		Assert( pCmdContext );
		Assert( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_DST );

		// Determine alignment for VkBufferImageCopy::bufferOffset according to specs.
		// * If the queue family used to create the VkCommandPool which commandBuffer was allocated from does not support VK_QUEUE_GRAPHICS_BIT or VK_QUEUE_COMPUTE_BIT,
		//   the bufferOffset member of any element of pRegions must be a multiple of 4.
		// * If srcImage does not have either a depth/stencil or a multi-planar format, then for each element of pRegions, bufferOffset must be a multiple of the format's texel block size
		// * If srcImage is a blocked image, for each element of pRegions, bufferOffset must be a multiple of the compressed texel block size in bytes
		uint32 bufferOffsetAlignment = S_Max<uint32>( g_PixelFormatInfos[pixelFormat].blockBytes, STUDIOAPI_VK_BUFFER_OFFSET_ALIGNMENT );

		// Copy data for each layer by CStudioAPIDataUploaderVk
		uint32					  layerSize				 = GetLayerSize();
		uint32					  startOffset			 = startLayer * layerSize + GetMipOffset( startMip );
		CStudioAPICmdContextVk*	  pStudioAPICmdContext	 = (CStudioAPICmdContextVk*)pCmdContext;
		CStudioAPICmdContextVk*	  pTransferCmdContext	 = (CStudioAPICmdContextVk*)g_StudioAPIVk.GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_TRANSFER );
		CStudioAPIDataUploaderVk& dataUploader			 = g_StudioAPIVk.GetDataUploader();
		bool					  bNeedOwnershipTransfer = !( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_CONCURRENT ) && pStudioAPICmdContext->GetQueue().GetQueueFamilyIndex() != pTransferCmdContext->GetQueue().GetQueueFamilyIndex();
		for ( uint32 layerIdx = startLayer; layerIdx < numLayers; ++layerIdx )
		{
			// Initialize base fields of an image barrier
			VkImageMemoryBarrier vkImageMemoryBarrier			 = {};
			vkImageMemoryBarrier.sType							 = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			vkImageMemoryBarrier.srcQueueFamilyIndex			 = VK_QUEUE_FAMILY_IGNORED;
			vkImageMemoryBarrier.dstQueueFamilyIndex			 = VK_QUEUE_FAMILY_IGNORED;
			vkImageMemoryBarrier.image							 = vkImage;
			vkImageMemoryBarrier.subresourceRange.aspectMask	 = vkImageAspectFlags;
			vkImageMemoryBarrier.subresourceRange.baseMipLevel	 = 0;
			vkImageMemoryBarrier.subresourceRange.levelCount	 = VK_REMAINING_MIP_LEVELS;
			vkImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
			vkImageMemoryBarrier.subresourceRange.layerCount	 = VK_REMAINING_ARRAY_LAYERS;

			// Upload the data
			uploadState_t uploadState = {};
			uploadState.currentLayer  = layerIdx;
			uploadState.currentMip	  = layerIdx == startLayer ? startMip : 0;
			uint64 dataSizeToUpload	  = layerSize - GetMipOffset( uploadState.currentMip );
			dataUploader.Upload( dataSizeToUpload, bufferOffsetAlignment,
								 [this, &vkImageMemoryBarrier, &uploadState, bNeedOwnershipTransfer, pStudioAPICmdContext, pTransferCmdContext, startOffset, pData, dataSizeToUpload]( const CStudioAPIDataUploaderVk::uploadParams_t& uploadParams )
								 {
									 // Make a barrier if it need
									 if ( uploadState.currentLayer == 0 && uploadParams.remainSizeToUpload == dataSizeToUpload )
									 {
										 vkImageMemoryBarrier.srcAccessMask = 0;
										 vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
										 vkImageMemoryBarrier.oldLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
										 vkImageMemoryBarrier.newLayout		= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
										 vkCmdPipelineBarrier( uploadParams.pCmdBuffer->GetVkCommandBuffer(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &vkImageMemoryBarrier );
									 }

									 uint32 mipSizeX	 = S_Max<uint32>( sizeX >> uploadState.currentMip, 1 );
									 uint32 mipSizeY	 = S_Max<uint32>( sizeY >> uploadState.currentMip, 1 );
									 uint32 mipSizeZ	 = S_Max<uint32>( sizeZ >> uploadState.currentMip, 1 );
									 uint32 blockSizeY	 = g_PixelFormatInfos[pixelFormat].blockSizeY;
									 uint32 numRowsInMip = S_Max<uint32>( ( mipSizeY + blockSizeY - 1 ) / blockSizeY, 1 );

									 // Calculate number of rows that fit into partialUploadSize bytes of memory,
									 // we need take in to account granularity constraints
									 uint32		rowPitch		 = GetMipRowPitch( uploadState.currentMip );
									 VkExtent3D vkGranularity	 = pTransferCmdContext->GetQueue().GetVkQueueFamilyProperties().minImageTransferGranularity;
									 uint32		depthGranularity = S_Min( mipSizeZ - uploadState.currentMipDepth, vkGranularity.depth );
									 uint32		numRowsToUpload	 = uploadParams.partialUploadSize / ( rowPitch * depthGranularity );
									 if ( numRowsToUpload + uploadState.currentMipRow < numRowsInMip )
									 {
										 // Align down if we have less rows to upload then one at the mip level
										 numRowsToUpload = ( numRowsToUpload / vkGranularity.height ) * vkGranularity.height;

										 // If numRowsToUpload is zero then partialUploadSize is too small to fit minimum number of rows
										 if ( numRowsToUpload == 0 )
										 {
											 return -1;
										 }
									 }
									 else
									 {
										 numRowsToUpload = numRowsInMip - uploadState.currentMipRow;
									 }

									 // Copy the data into the staging buffer
									 uint32 mipDepthPitch	   = GetMipDepthPitch( uploadState.currentMip );
									 byte*	pStagingBufferData = uploadParams.pStagingBufferData;
									 for ( uint32 mipDepth = uploadState.currentMipDepth, mipDepthSize = uploadState.currentMipDepth + depthGranularity; mipDepth < mipDepthSize; ++mipDepth )
									 {
										 uint32 offset = GetLayerOffset( uploadState.currentLayer ) + GetMipOffset( uploadState.currentMip ) + mipDepthPitch * mipDepth - startOffset;
										 Mem_Memcpy( pStagingBufferData, pData + offset + rowPitch * uploadState.currentMipRow, rowPitch * numRowsToUpload );
										 pStagingBufferData += rowPitch * numRowsToUpload;
									 }

									 // Place vkCmdCopyBufferToImage call
									 VkBufferImageCopy vkBufferImageCopy			   = {};
									 vkBufferImageCopy.bufferOffset					   = uploadParams.stagingBufferOffset;
									 vkBufferImageCopy.imageSubresource.aspectMask	   = vkImageAspectFlags;
									 vkBufferImageCopy.imageSubresource.mipLevel	   = uploadState.currentMip;
									 vkBufferImageCopy.imageSubresource.baseArrayLayer = uploadState.currentLayer;
									 vkBufferImageCopy.imageSubresource.layerCount	   = 1;
									 vkBufferImageCopy.imageOffset.x				   = 0;
									 vkBufferImageCopy.imageOffset.y				   = uploadState.currentMipRow * blockSizeY;
									 vkBufferImageCopy.imageOffset.z				   = uploadState.currentMipDepth;
									 vkBufferImageCopy.imageExtent.width			   = mipSizeX;
									 vkBufferImageCopy.imageExtent.height			   = S_Min( numRowsToUpload * blockSizeY, mipSizeY );  // Handle textures with height is not multiple of block size
									 vkBufferImageCopy.imageExtent.depth			   = depthGranularity;
									 vkCmdCopyBufferToImage( uploadParams.pCmdBuffer->GetVkCommandBuffer(), uploadParams.vkStagingBuffer, vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vkBufferImageCopy );

									 uploadState.currentMipRow += numRowsToUpload;
									 Assert( uploadState.currentMipRow <= numRowsInMip );
									 if ( uploadState.currentMipRow == numRowsInMip )
									 {
										 uploadState.currentMipRow = 0;
										 uploadState.currentMipDepth += depthGranularity;
										 Assert( uploadState.currentMipDepth <= mipSizeZ );
										 if ( uploadState.currentMipDepth == mipSizeZ )
										 {
											 uploadState.currentMipDepth = 0;
											 ++uploadState.currentMip;
										 }
									 }

									 // Make a barrier if it need
									 int32 uploadedSize = numRowsToUpload * rowPitch * depthGranularity;
									 if ( uploadState.currentLayer == numLayers - 1 && uploadParams.remainSizeToUpload - uploadedSize == 0 )
									 {
										 // This either just barrier that waits for copy completion, or if bNeedOwnershipTransfer is TRUE,
										 // then it is a wait and release ownership barrier
										 // NOTE BS yehor.pohuliaka - Maybe no need do transfer ownership here and it must do user on outside?
										 vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
										 vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
										 vkImageMemoryBarrier.oldLayout		= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
										 vkImageMemoryBarrier.newLayout		= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
										 if ( bNeedOwnershipTransfer )
										 {
											 vkImageMemoryBarrier.srcQueueFamilyIndex = pTransferCmdContext->GetQueue().GetQueueFamilyIndex();
											 vkImageMemoryBarrier.dstQueueFamilyIndex = pStudioAPICmdContext->GetQueue().GetQueueFamilyIndex();
										 }
										 vkCmdPipelineBarrier( uploadParams.pCmdBuffer->GetVkCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &vkImageMemoryBarrier );
									 }

									 return uploadedSize;
								 } );

			// Advance the pointer
			pData += dataSizeToUpload;
			startOffset += layerSize;
		}

		// Update the synchronization state
		UpdateSyncState( CStudioAPIImageSubresourceRangeVk::CreateForAllSubresources(),
						 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
						 pTransferCmdContext->GetQueue().GetQueueFamilyIndex() );
		for ( uint32 syncStateIdx = 0, numSyncStates = (uint32)syncStates.size(); syncStateIdx < numSyncStates; ++syncStateIdx )
		{
			syncStates[syncStateIdx].bHasPendingOwnershipTransfer = bNeedOwnershipTransfer;
		}
	}
	// Otherwise use mapped memory
	else
	{
		// Copy the data
		studioAPIMappedTextureData_t mappedData = {};
		MapMemory( startLayer, startMip, mappedData );
		Mem_Memcpy( mappedData.pData, pData, numLayers * GetLayerSize() - ( GetLayerOffset( startLayer ) + GetMipOffset( startMip ) ) );
		UnmapMemory( mappedData );
	}
}

/*
==================
CStudioAPITextureVk::MapMemory
==================
*/
void CStudioAPITextureVk::MapMemory( uint32 layer, uint32 mip, studioAPIMappedTextureData_t& mappedData )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// We can use MapMemory only for CPU shared memory
	AssertMsg( memoryFlags & STUDIOAPI_TEXTURE_MEMORY_FLAG_CPU, "The texture doesn't support the map memory, use IStudioAPICmdList::CopyTextureToBuffer" );
	Assert( !mappedData.pData && layer < numLayers && mip < numMips );
	Mem_Memzero( &mappedData, sizeof( studioAPIMappedTextureData_t ) );

	// For linear textures we can use MapMemory to read/write data in the texture
	mappedData.pData	  = g_StudioAPIVk.GetMemoryMgr().MapMemory<byte>( vmaAllocation ) + GetLayerOffset( layer ) + GetMipOffset( mip );
	mappedData.size		  = GetMipSize( mip );
	mappedData.rowPitch	  = GetMipRowPitch( mip );
	mappedData.depthPitch = GetMipDepthPitch( mip );
}

/*
==================
CStudioAPITextureVk::UnmapMemory
==================
*/
void CStudioAPITextureVk::UnmapMemory( studioAPIMappedTextureData_t& mappedData )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// For linear textures we can use UnmapMemory to read/write data in the texture
	Assert( mappedData.pData );
	g_StudioAPIVk.GetMemoryMgr().UnmapMemory( vmaAllocation );
	mappedData.pData = NULL;
}

/*
==================
CStudioAPITextureVk::UpdateSyncState
==================
*/
void CStudioAPITextureVk::UpdateSyncState( const CStudioAPIImageSubresourceRangeVk& subresourceRange, VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	studioAPISyncStateImageVk_t& syncState0	   = syncStates[GetSyncStateIndex( subresourceRange.GetStartMip(), subresourceRange.GetStartLayer() )];
	studioAPISyncStateImageVk_t	 oldSyncState0 = syncState0;
	VK_UpdateSyncStateImage( vkDstImageLayout, vkDstAccessMask, vkDstStageMask, dstQueueFamilyIndex, syncState0 );

	uint32 startMip	  = 0;
	uint32 startLayer = 0;
	uint32 numMips	  = CStudioAPITextureVk::numMips;
	uint32 numLayers  = CStudioAPITextureVk::numLayers;
	if ( !subresourceRange.IsAllSubresources() )
	{
		startMip   = subresourceRange.GetStartMip();
		startLayer = subresourceRange.GetStartLayer();
		numMips	   = subresourceRange.GetNumMips();
		numLayers  = subresourceRange.GetNumLayers();
	}

	// Mirror the state into all subresources of the range
	for ( uint32 layerIdx = startLayer; layerIdx < numLayers; ++layerIdx )
	{
		for ( uint32 mipIdx = startMip + 1; mipIdx < numMips; ++mipIdx )
		{
			studioAPISyncStateImageVk_t& syncState = syncStates[GetSyncStateIndex( mipIdx, layerIdx )];
			if ( syncState == oldSyncState0 )
			{
				Mem_Memcpy( &syncState, &syncState0, sizeof( studioAPISyncStateImageVk_t ) );
			}
			else
			{
				VK_UpdateSyncStateImage( vkDstImageLayout, vkDstAccessMask, vkDstStageMask, dstQueueFamilyIndex, syncState );
			}
		}
	}
}

/*
==================
CStudioAPITextureVk::UpdateSyncStateWithBarrier
==================
*/
void CStudioAPITextureVk::UpdateSyncStateWithBarrier( CStudioAPICmdListVk* pCmdList, const CStudioAPIImageSubresourceRangeVk& subresourceRange, VkImageLayout vkDstImageLayout, VkAccessFlags vkDstAccessMask, VkPipelineStageFlags vkDstStageMask, uint32 dstQueueFamilyIndex )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Get subresource range
	uint32 startMip	  = 0;
	uint32 startLayer = 0;
	uint32 numMips	  = CStudioAPITextureVk::numMips;
	uint32 numLayers  = CStudioAPITextureVk::numLayers;
	if ( !subresourceRange.IsAllSubresources() )
	{
		startMip   = subresourceRange.GetStartMip();
		startLayer = subresourceRange.GetStartLayer();
		numMips	   = subresourceRange.GetNumMips();
		numLayers  = subresourceRange.GetNumLayers();
	}

	// We determine whether we need individual barriers
	bool						 bIndividualBarriers = false;
	studioAPISyncStateImageVk_t& syncState0			 = syncStates[GetSyncStateIndex( startMip, startLayer )];
	for ( uint32 layerIdx = startLayer; layerIdx < numLayers; ++layerIdx )
	{
		for ( uint32 mipIdx = startMip + 1; mipIdx < numMips; ++mipIdx )
		{
			if ( syncStates[GetSyncStateIndex( mipIdx, layerIdx )] != syncState0 )
			{
				bIndividualBarriers = true;
				break;
			}
		}
	}

	VkImageMemoryBarrier vkImageMemoryBarriers[STUDIOAPI_VK_MAX_NUM_PENDING_IMAGE_BARRIERS];
	VkPipelineStageFlags vkSrcStageBarrierMask = VK_PIPELINE_STAGE_NONE;
	VkPipelineStageFlags vkDstStageBarrierMask = VK_PIPELINE_STAGE_NONE;
	uint32				 numBarriers		   = 0;

	if ( bIndividualBarriers )
	{
		// We are in a state where individual sub-resources have been transitioned individually, but we are now requesting all sub-resources to be in a certain state
		// in that case, Vulkan requires that we individually transition the sub-resources individually (but we can still batch this into a single vkCmdPipelineBarrier call)
		for ( uint32 layerIdx = startLayer; layerIdx < numLayers; ++layerIdx )
		{
			for ( uint32 mipIdx = startMip; mipIdx < numMips; ++mipIdx )
			{
				studioAPIImageMemoryBarrierVk_t imageMemoryBarrier						= {};
				imageMemoryBarrier.vkImageMemoryBarrier.image							= vkImage;
				imageMemoryBarrier.vkImageMemoryBarrier.subresourceRange.aspectMask		= vkImageAspectFlags;
				imageMemoryBarrier.vkImageMemoryBarrier.subresourceRange.baseMipLevel	= mipIdx;
				imageMemoryBarrier.vkImageMemoryBarrier.subresourceRange.levelCount		= 1;
				imageMemoryBarrier.vkImageMemoryBarrier.subresourceRange.baseArrayLayer = layerIdx;
				imageMemoryBarrier.vkImageMemoryBarrier.subresourceRange.layerCount		= 1;
				if ( VK_UpdateSyncStateImageWithBarrier( vkDstImageLayout, vkDstAccessMask, vkDstStageMask, dstQueueFamilyIndex, syncStates[GetSyncStateIndex( mipIdx, layerIdx )], imageMemoryBarrier, usageFlags ) )
				{
					vkImageMemoryBarriers[numBarriers++] = imageMemoryBarrier.vkImageMemoryBarrier;
					vkSrcStageBarrierMask |= imageMemoryBarrier.vkSrcStageMask;
					vkDstStageBarrierMask |= imageMemoryBarrier.vkDstStageMask;

					// This should probably never happen, but this will guard against the case of a lot of of sub-resources needing to be transitioned
					// Submit the barriers we already have, then clear our barrier list and keep going
					if ( numBarriers == STUDIOAPI_VK_MAX_NUM_PENDING_IMAGE_BARRIERS )
					{
						CStudioAPICmdContextVk* pCmdContext = (CStudioAPICmdContextVk*)pCmdList->GetCmdContext();
						pCmdContext->AddPendingImageBarriers( pCmdList, numBarriers, vkImageMemoryBarriers, vkSrcStageBarrierMask, vkDstStageBarrierMask );
						vkSrcStageBarrierMask = VK_PIPELINE_STAGE_NONE;
						vkDstStageBarrierMask = VK_PIPELINE_STAGE_NONE;
						numBarriers			  = 0;
					}
				}
			}
		}
	}
	else
	{
		studioAPIImageMemoryBarrierVk_t imageMemoryBarrier		 = {};
		imageMemoryBarrier.vkImageMemoryBarrier.image			 = vkImage;
		imageMemoryBarrier.vkImageMemoryBarrier.subresourceRange = subresourceRange.AsVkStructure( vkImageAspectFlags );
		if ( VK_UpdateSyncStateImageWithBarrier( vkDstImageLayout, vkDstAccessMask, vkDstStageMask, dstQueueFamilyIndex, syncState0, imageMemoryBarrier, usageFlags ) )
		{
			vkImageMemoryBarriers[numBarriers++] = imageMemoryBarrier.vkImageMemoryBarrier;
			vkSrcStageBarrierMask |= imageMemoryBarrier.vkSrcStageMask;
			vkDstStageBarrierMask |= imageMemoryBarrier.vkDstStageMask;
		}

		// Mirror the state into all subresources of the range
		for ( uint32 layerIdx = startLayer; layerIdx < numLayers; ++layerIdx )
		{
			for ( uint32 mipIdx = startMip + 1; mipIdx < numMips; ++mipIdx )
			{
				studioAPISyncStateImageVk_t& syncState = syncStates[GetSyncStateIndex( mipIdx, layerIdx )];
				Mem_Memcpy( &syncState, &syncState0, sizeof( studioAPISyncStateImageVk_t ) );
			}
		}
	}

	// Issue all image barriers
	if ( numBarriers > 0 )
	{
		CStudioAPICmdContextVk* pCmdContext = (CStudioAPICmdContextVk*)pCmdList->GetCmdContext();
		pCmdContext->AddPendingImageBarriers( pCmdList, numBarriers, vkImageMemoryBarriers, vkSrcStageBarrierMask, vkDstStageBarrierMask );
	}
}

/*
==================
CStudioAPITextureVk::GetSizeX
==================
*/
uint32 CStudioAPITextureVk::GetSizeX() const
{
	return sizeX;
}

/*
==================
CStudioAPITextureVk::GetSizeY
==================
*/
uint32 CStudioAPITextureVk::GetSizeY() const
{
	return sizeY;
}

/*
==================
CStudioAPITextureVk::GetSizeZ
==================
*/
uint32 CStudioAPITextureVk::GetSizeZ() const
{
	return sizeZ;
}

/*
==================
CStudioAPITextureVk::GetNumMips
==================
*/
uint32 CStudioAPITextureVk::GetNumMips() const
{
	return numMips;
}

/*
==================
CStudioAPITextureVk::GetNumLayers
==================
*/
uint32 CStudioAPITextureVk::GetNumLayers() const
{
	return numLayers;
}

/*
==================
CStudioAPITextureVk::GetPixelFormat
==================
*/
studioAPIPixelFormat_t CStudioAPITextureVk::GetPixelFormat() const
{
	return pixelFormat;
}

/*
==================
CStudioAPITextureVk::GetType
==================
*/
studioAPITextureType_t CStudioAPITextureVk::GetType() const
{
	return type;
}

/*
==================
CStudioAPITextureVk::GetUsageFlags
==================
*/
uint32 CStudioAPITextureVk::GetUsageFlags() const
{
	return usageFlags;
}

/*
==================
CStudioAPITextureVk::GetMemoryFlags
==================
*/
uint8 CStudioAPITextureVk::GetMemoryFlags() const
{
	return memoryFlags;
}

/*
==================
CStudioAPITextureVk::OnStudioAPIVkShutdown
==================
*/
void CStudioAPITextureVk::OnStudioAPIVkShutdown( void* pUserData )
{
	Assert( pUserData );
	CStudioAPITextureVk* pStudioAPITextureVk		 = (CStudioAPITextureVk*)pUserData;
	pStudioAPITextureVk->onStudioAPIVkShutdownHandle = INVALID_HANDLE;
	pStudioAPITextureVk->~CStudioAPITextureVk();
}
