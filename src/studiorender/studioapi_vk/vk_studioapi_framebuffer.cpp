#include "pch_studioapi_vk.h"
#include "studiorender/studioapi_vk/vk_studioapi_framebuffer.h"
#include "studiorender/studioapi_vk/vk_studioapi_texture.h"
#include "studiorender/studioapi_vk/vk_studioapi_swapchain.h"
#include "studiorender/studioapi_vk/vk_studioapi_renderpass.h"

/*
==================
CStudioAPIFrameBufferVk::CStudioAPIFrameBufferVk
==================
*/
CStudioAPIFrameBufferVk::CStudioAPIFrameBufferVk( const studioAPIFrameBufferCreateInfo_t& createInfo, const char* pDebugName /* = "" */ )
	: size( createInfo.size )
	, vkFrameBuffer( VK_NULL_HANDLE )
	, onStudioAPIVkShutdownHandle( INVALID_HANDLE )
	, numClearValues( 0 )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	uint32					numImageViews		   = 0;
	CStudioAPIRenderPassVk* pStudioAPIRenderPassVk = (CStudioAPIRenderPassVk*)createInfo.pRenderPass;
	VkImageView				vkImageViews[STUDIOAPI_MAX_SIMULTANEOUS_RENDER_TARGETS + 1];

	// Get all color render targets
	for ( uint32 renderTargetIdx = 0; renderTargetIdx < STUDIOAPI_MAX_SIMULTANEOUS_RENDER_TARGETS; ++renderTargetIdx )
	{
		const studioAPIRenderTarget_t& studioAPIColorRenderTarget = createInfo.colorRenderTargets[renderTargetIdx];
		if ( studioAPIColorRenderTarget.pTexture || studioAPIColorRenderTarget.pSwapChainImage )
		{
			// If we have STUDIOAPI_RENDER_TARGET_FLAG_SWAPCHAIN_IMAGE then the render target is a swap chain image
			VkImageView vkImageView = VK_NULL_HANDLE;
			if ( studioAPIColorRenderTarget.flags & STUDIOAPI_RENDER_TARGET_FLAG_SWAPCHAIN_IMAGE )
			{
				AssertMsg( createInfo.renderTargetMip == 0 && createInfo.renderTargetLayer == 0, "A swap chain image have onle one mip level and layer" );
				CStudioAPISwapChainImageVk* pStudioAPISwapChainImage = (CStudioAPISwapChainImageVk*)studioAPIColorRenderTarget.pSwapChainImage;
				vkImageView											 = pStudioAPISwapChainImage->GetVkImageView();
			}
			// Otherwise it is texture
			else
			{
				CStudioAPITextureVk* pStudioAPITexture = (CStudioAPITextureVk*)studioAPIColorRenderTarget.pTexture;
				vkImageView							   = pStudioAPITexture->GetVkImageView( createInfo.renderTargetMip, createInfo.renderTargetLayer );
			}
			Assert( vkImageView != VK_NULL_HANDLE );
			vkImageViews[numImageViews++] = vkImageView;

			// Initialize a clear color if it need
			if ( createInfo.bClearColor )
			{
				VkClearValue& vkClearValue	  = vkClearValues[numClearValues++];
				vector4_t	  normalizedColor = createInfo.clearColor.ToVector4();
				Mem_Memzero( &vkClearValue, sizeof( VkClearValue ) );
				Mem_Memcpy( vkClearValue.color.float32, &normalizedColor.x, 4 * sizeof( float ) );
			}
		}
	}

	// Get a depth stencil render target
	if ( createInfo.depthStencilRenderTarget.pTexture || createInfo.depthStencilRenderTarget.pSwapChainImage )
	{
		AssertMsg( !( createInfo.depthStencilRenderTarget.flags & STUDIOAPI_RENDER_TARGET_FLAG_SWAPCHAIN_IMAGE ), "A depth stencil render target can't be a swap chain image" );
		CStudioAPITextureVk* pStudioAPITexture = (CStudioAPITextureVk*)createInfo.depthStencilRenderTarget.pTexture;
		VkImageView			 vkImageView	   = pStudioAPITexture->GetVkImageView( ( createInfo.depthStencilRenderTarget.flags & STUDIOAPI_RENDER_TARGET_FLAG_DEPTH_ONLY ) != 0, ( createInfo.depthStencilRenderTarget.flags & STUDIOAPI_RENDER_TARGET_FLAG_STENCIL_ONLY ) != 0 );
		Assert( vkImageView != VK_NULL_HANDLE );
		vkImageViews[numImageViews++] = vkImageView;

		// Initialize a clear color if it need
		if ( createInfo.bDepthStencilClearValue )
		{
			VkClearValue& vkClearValue = vkClearValues[numClearValues++];
			Mem_Memzero( &vkClearValue, sizeof( VkClearValue ) );
			vkClearValue.depthStencil.depth	  = createInfo.depthClearValue;
			vkClearValue.depthStencil.stencil = createInfo.stencilClearValue;
		}
	}

	// Create a frame buffer
	Assert( pStudioAPIRenderPassVk );
	VkFramebufferCreateInfo vkFramebufferCreateInfo = {};
	vkFramebufferCreateInfo.sType					= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	vkFramebufferCreateInfo.renderPass				= pStudioAPIRenderPassVk->GetVkRenderPass();
	vkFramebufferCreateInfo.attachmentCount			= numImageViews;
	vkFramebufferCreateInfo.pAttachments			= vkImageViews;
	vkFramebufferCreateInfo.width					= createInfo.size.x;
	vkFramebufferCreateInfo.height					= createInfo.size.y;
	vkFramebufferCreateInfo.layers					= 1;
	STUDIOAPI_VK_VERIFY_RESULT( vkCreateFramebuffer( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkFramebufferCreateInfo, NULL, &vkFrameBuffer ) );

	// Set debug name for the frame buffer
#if !RETAIL
	VK_SetDebugName( VK_OBJECT_TYPE_FRAMEBUFFER, (uint64)vkFrameBuffer, pDebugName );
#endif	// !RETAIL

	// Register in 'onStudioAPIVkShutodwn' for destroy Vulkan objects when the one is shutdown
	onStudioAPIVkShutdownHandle = g_StudioAPIVk.OnStudioAPIVkShutdown().Subscribe( &CStudioAPIFrameBufferVk::OnStudioAPIVkShutdown, this );
}

/*
==================
CStudioAPIFrameBufferVk::~CStudioAPIFrameBufferVk
==================
*/
CStudioAPIFrameBufferVk::~CStudioAPIFrameBufferVk()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Destroy the Vulkan frame buffer
	if ( vkFrameBuffer != VK_NULL_HANDLE )
	{
		g_StudioAPIVk.GetMemoryMgr().FreeResource( [vkFrameBuffer = vkFrameBuffer]()
												   { vkDestroyFramebuffer( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkFrameBuffer, NULL ); } );
		vkFrameBuffer = VK_NULL_HANDLE;
	}

	// Remove CStudioAPIFrameBufferVk::OnStudioAPIVkShutdown from event 'onStudioAPIVkShutodwn'
	if ( onStudioAPIVkShutdownHandle != INVALID_HANDLE )
	{
		g_StudioAPIVk.OnStudioAPIVkShutdown().Unsubscribe( onStudioAPIVkShutdownHandle );
		onStudioAPIVkShutdownHandle = INVALID_HANDLE;
	}
}

/*
==================
CStudioAPIFrameBufferVk::GetSize
==================
*/
vector2i_t CStudioAPIFrameBufferVk::GetSize() const
{
	return size;
}

/*
==================
CStudioAPIFrameBufferVk::OnStudioAPIVkShutdown
==================
*/
void CStudioAPIFrameBufferVk::OnStudioAPIVkShutdown( void* pUserData )
{
	Assert( pUserData );
	CStudioAPIFrameBufferVk* pStudioAPIFrameBuffer	   = (CStudioAPIFrameBufferVk*)pUserData;
	pStudioAPIFrameBuffer->onStudioAPIVkShutdownHandle = INVALID_HANDLE;
	pStudioAPIFrameBuffer->~CStudioAPIFrameBufferVk();
}
