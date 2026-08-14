#include "pch_studioapi_vk.h"
#include "studiorender/studioapi_vk/vk_studioapi_swapchain.h"
#include "studiorender/studioapi_vk/vk_studioapi_renderpass.h"

/*
==================
VK_TranslateRenderTargetLoadOp
==================
*/
static VkAttachmentLoadOp VK_TranslateRenderTargetLoadOp( studioAPIRenderTargetLoadOp_t loadOp )
{
	switch ( loadOp )
	{
	case STUDIOAPI_RENDER_TARGET_LOAD_OP_LOAD: return VK_ATTACHMENT_LOAD_OP_LOAD;
	case STUDIOAPI_RENDER_TARGET_LOAD_OP_CLEAR: return VK_ATTACHMENT_LOAD_OP_CLEAR;
	case STUDIOAPI_RENDER_TARGET_LOAD_OP_DONT_CARE: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	default:
		AssertMsg( false, "Unknown render target load operation 0x%X", loadOp );
		return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
	}
}

/*
==================
VK_TranslateRenderTargetStoreOp
==================
*/
static VkAttachmentStoreOp VK_TranslateRenderTargetStoreOp( studioAPIRenderTargetStoreOp_t storeOp )
{
	switch ( storeOp )
	{
	case STUDIOAPI_RENDER_TARGET_STORE_OP_STORE: return VK_ATTACHMENT_STORE_OP_STORE;
	case STUDIOAPI_RENDER_TARGET_STORE_OP_DONT_CARE: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
	default:
		AssertMsg( false, "Unknown render target store operation 0x%X", storeOp );
		return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
	}
}

/*
==================
VK_TranslateDepthStencilAccessFlagsToVkImageLayout
==================
*/
static VkImageLayout VK_TranslateDepthStencilAccessFlagsToVkImageLayout( uint8 depthStencilAccessFlags )
{
	switch ( depthStencilAccessFlags )
	{
	case STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_STENCIL_NOP: return VK_IMAGE_LAYOUT_UNDEFINED;
	case STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_READ_STENCIL_NOP: return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
	case STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_WRITE_STENCIL_NOP: return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
	case STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_NOP_STENCIL_READ: return VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL;
	case STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_STENCIL_READ: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
	case STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_WRITE_STENCIL_READ: return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
	case STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_NOP_STENCIL_WRITE: return VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
	case STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_READ_STENCIL_WRITE: return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
	case STUDIOAPI_DEPTHSTENCIL_ACCESS_FLAG_DEPTH_STENCIL_WRITE: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	default:
		AssertMsg( false, "Unknwon combination of StudioAPI depth stencil access flags 0x%X", depthStencilAccessFlags );
		return VK_IMAGE_LAYOUT_UNDEFINED;
	}
}

/*
==================
CStudioAPIRenderPassVk::CStudioAPIRenderPassVk
==================
*/
CStudioAPIRenderPassVk::CStudioAPIRenderPassVk( const studioAPIRenderPassCreateInfo_t& createInfo, const char* pDebugName /* = "" */ )
	: vkRenderPass( VK_NULL_HANDLE )
	, onStudioAPIVkShutdownHandle( INVALID_HANDLE )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Fill information about attachments (render targets)
	eastl::vector<VkAttachmentDescription> vkAttachments;
	eastl::vector<VkAttachmentReference>   vkColorAttachmentReferences;
	VkAttachmentReference				   vkDepthStencilAttachmentReference = {};
	bool								   bHasDepthStencilAttachment		 = false;
	bool								   bHasColorAttachments				 = false;

	// Color attachments
	for ( uint32 renderTargetIdx = 0; renderTargetIdx < STUDIOAPI_MAX_SIMULTANEOUS_RENDER_TARGETS; ++renderTargetIdx )
	{
		const studioAPIColorRenderTargetInfo_t& studioAPIColorRenderTarget = createInfo.colorRenderTargets[renderTargetIdx];
		if ( studioAPIColorRenderTarget.pixelFormat != STUDIOAPI_PIXEL_FORMAT_UNKNOWN )
		{
			AssertMsg( studioAPIColorRenderTarget.bSwapChain ? studioAPIColorRenderTarget.pSwapChain : studioAPIColorRenderTarget.pixelFormat != STUDIOAPI_PIXEL_FORMAT_UNKNOWN, "A color render target must have valid pSwapChain (if bSwapChain is TRUE) or pixelFormat (if bSwapChain is FALSE)" );
			AssertMsg( studioAPIColorRenderTarget.bSwapChain || !VK_IsDepthPixelFormat( studioAPIColorRenderTarget.pixelFormat ), "A color render target can't have a depth pixel format" );
			VkAttachmentDescription& vkCurrentAttachment = vkAttachments.emplace_back();
			vkCurrentAttachment.format					 = !studioAPIColorRenderTarget.bSwapChain ? VK_TranslatePixelFormat( studioAPIColorRenderTarget.pixelFormat ) : ( (CStudioAPISwapChainVk*)studioAPIColorRenderTarget.pSwapChain )->GetVkSurfaceFormat().format;
			vkCurrentAttachment.samples					 = VK_SAMPLE_COUNT_1_BIT;
			vkCurrentAttachment.loadOp					 = VK_TranslateRenderTargetLoadOp( studioAPIColorRenderTarget.loadOp );
			vkCurrentAttachment.storeOp					 = VK_TranslateRenderTargetStoreOp( studioAPIColorRenderTarget.storeOp );
			vkCurrentAttachment.stencilLoadOp			 = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			vkCurrentAttachment.stencilStoreOp			 = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			vkCurrentAttachment.initialLayout			 = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;  // At now we're handling all layout transitions external from render passes, so render passes can just assume their render targets are in VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL layout and leave layout transitions to external barriers
			vkCurrentAttachment.finalLayout				 = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkAttachmentReference& vkCurrentAttachmentReference = vkColorAttachmentReferences.emplace_back();
			vkCurrentAttachmentReference.attachment				= (uint32)vkAttachments.size() - 1;
			vkCurrentAttachmentReference.layout					= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			bHasColorAttachments								= true;
		}
	}

	// Depth attachment
	if ( createInfo.depthStencilRenderTarget.pixelFormat != STUDIOAPI_PIXEL_FORMAT_UNKNOWN )
	{
		AssertMsg( VK_IsDepthPixelFormat( createInfo.depthStencilRenderTarget.pixelFormat ), "A depth stencil render target must have only a depth pixel format" );
		VkImageLayout			 vkDepthStencilImageLayout = VK_TranslateDepthStencilAccessFlagsToVkImageLayout( createInfo.depthStencilRenderTarget.accessFlags );
		VkAttachmentDescription& vkCurrentAttachment	   = vkAttachments.emplace_back();
		vkCurrentAttachment.format						   = VK_TranslatePixelFormat( createInfo.depthStencilRenderTarget.pixelFormat );
		vkCurrentAttachment.samples						   = VK_SAMPLE_COUNT_1_BIT;
		vkCurrentAttachment.loadOp						   = VK_TranslateRenderTargetLoadOp( createInfo.depthStencilRenderTarget.depthLoadOp );
		vkCurrentAttachment.storeOp						   = VK_TranslateRenderTargetStoreOp( createInfo.depthStencilRenderTarget.depthStoreOp );
		vkCurrentAttachment.stencilLoadOp				   = VK_TranslateRenderTargetLoadOp( createInfo.depthStencilRenderTarget.stencilLoadOp );
		vkCurrentAttachment.stencilStoreOp				   = VK_TranslateRenderTargetStoreOp( createInfo.depthStencilRenderTarget.stencilStoreOp );
		vkCurrentAttachment.initialLayout				   = vkDepthStencilImageLayout;	 // At now we're handling all layout transitions external from render passes, so render passes can just assume their render targets are in vkDepthStencilImageLayout layout and leave layout transitions to external barriers
		vkCurrentAttachment.finalLayout					   = vkDepthStencilImageLayout;

		vkDepthStencilAttachmentReference.attachment = (uint32)vkAttachments.size() - 1;
		vkDepthStencilAttachmentReference.layout	 = vkDepthStencilImageLayout;
		bHasDepthStencilAttachment					 = true;
	}

	// Fill information about main sub-pass
	eastl::vector<VkSubpassDescription> vkSubpassDescriptions;
	eastl::vector<VkSubpassDependency>	vkSubpassDependencies;
	{
		VkSubpassDescription& vkSubpassDescription	 = vkSubpassDescriptions.emplace_back();
		vkSubpassDescription.pipelineBindPoint		 = VK_PIPELINE_BIND_POINT_GRAPHICS;
		vkSubpassDescription.colorAttachmentCount	 = (uint32)vkColorAttachmentReferences.size();
		vkSubpassDescription.pColorAttachments		 = vkColorAttachmentReferences.data();
		vkSubpassDescription.pDepthStencilAttachment = bHasDepthStencilAttachment ? &vkDepthStencilAttachmentReference : NULL;
	}

	// Create a render pass
	VkRenderPassCreateInfo vkRenderPassCreateInfo = {};
	vkRenderPassCreateInfo.sType				  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	vkRenderPassCreateInfo.attachmentCount		  = (uint32)vkAttachments.size();
	vkRenderPassCreateInfo.pAttachments			  = vkAttachments.data();
	vkRenderPassCreateInfo.subpassCount			  = (uint32)vkSubpassDescriptions.size();
	vkRenderPassCreateInfo.pSubpasses			  = vkSubpassDescriptions.data();
	vkRenderPassCreateInfo.dependencyCount		  = (uint32)vkSubpassDependencies.size();
	vkRenderPassCreateInfo.pDependencies		  = vkSubpassDependencies.data();
	STUDIOAPI_VK_VERIFY_RESULT( vkCreateRenderPass( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkRenderPassCreateInfo, NULL, &vkRenderPass ) );

	// Set debug name for the render pass
#if !RETAIL
	VK_SetDebugName( VK_OBJECT_TYPE_RENDER_PASS, (uint64)vkRenderPass, pDebugName );
#endif	// !RETAIL

	// Register in 'onStudioAPIVkShutodwn' for destroy Vulkan objects when the one is shutdown
	onStudioAPIVkShutdownHandle = g_StudioAPIVk.OnStudioAPIVkShutdown().Subscribe( &CStudioAPIRenderPassVk::OnStudioAPIVkShutdown, this );
}

/*
==================
CStudioAPIRenderPassVk::CStudioAPIRenderPassVk
==================
*/
CStudioAPIRenderPassVk::~CStudioAPIRenderPassVk()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Destroy the Vulkan render pass
	if ( vkRenderPass != VK_NULL_HANDLE )
	{
		g_StudioAPIVk.GetMemoryMgr().FreeResource( [vkRenderPass = vkRenderPass]()
												   { vkDestroyRenderPass( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkRenderPass, NULL ); } );
		vkRenderPass = VK_NULL_HANDLE;
	}

	// Remove CStudioAPIRenderPassVk::OnStudioAPIVkShutdown from event 'onStudioAPIVkShutodwn'
	if ( onStudioAPIVkShutdownHandle != INVALID_HANDLE )
	{
		g_StudioAPIVk.OnStudioAPIVkShutdown().Unsubscribe( onStudioAPIVkShutdownHandle );
		onStudioAPIVkShutdownHandle = INVALID_HANDLE;
	}
}

/*
==================
CStudioAPIRenderPassVk::OnStudioAPIVkShutdown
==================
*/
void CStudioAPIRenderPassVk::OnStudioAPIVkShutdown( void* pUserData )
{
	Assert( pUserData );
	CStudioAPIRenderPassVk* pStudioAPIRenderPass	  = (CStudioAPIRenderPassVk*)pUserData;
	pStudioAPIRenderPass->onStudioAPIVkShutdownHandle = INVALID_HANDLE;
	pStudioAPIRenderPass->~CStudioAPIRenderPassVk();
}
