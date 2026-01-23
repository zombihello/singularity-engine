#include "pch_studioapi_vk.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdbufferpool.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdbuffer.h"
#include "studiorender/studioapi_vk/vk_studioapi_renderpass.h"
#include "studiorender/studioapi_vk/vk_studioapi_framebuffer.h"

/*
==================
CStudioAPICmdBufferVk::CStudioAPICmdBufferVk
==================
*/
CStudioAPICmdBufferVk::CStudioAPICmdBufferVk( CStudioAPICmdBufferPoolVk& cmdBufferPool )
	: bIsAllocatedMemory( false )
	, vkCommandBuffer( VK_NULL_HANDLE )
	, cmdBufferPool( cmdBufferPool )
	, pCurrentDescriptorPoolSetContainer( NULL )
{
}

/*
==================
CStudioAPICmdBufferVk::CStudioAPICmdBufferVk
==================
*/
CStudioAPICmdBufferVk::~CStudioAPICmdBufferVk()
{
	FreeMemory();
}

/*
==================
CStudioAPICmdBufferVk::Begin
==================
*/
void CStudioAPICmdBufferVk::Begin()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Begin write into the command buffer
	VkCommandBufferBeginInfo vkCommandBufferBeginInfo = {};
	vkCommandBufferBeginInfo.sType					  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkCommandBufferBeginInfo.flags					  = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	STUDIOAPI_VK_VERIFY_RESULT( vkBeginCommandBuffer( vkCommandBuffer, &vkCommandBufferBeginInfo ) );
}

/*
==================
CStudioAPICmdBufferVk::End
==================
*/
void CStudioAPICmdBufferVk::End()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	STUDIOAPI_VK_VERIFY_RESULT( vkEndCommandBuffer( vkCommandBuffer ) );
}

/*
==================
CStudioAPICmdBufferVk::AcquirePoolSetAndDescriptorsIfNeed
==================
*/
bool CStudioAPICmdBufferVk::AcquirePoolSetAndDescriptorsIfNeed( const CStudioAPIDescriptorSetsLayoutVk& descriptorSetsLayout, bool bNeedDescriptorSets, VkDescriptorSet* pDestDescriptorSets )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Acquire a pool set container if we haven't it
	if ( !pCurrentDescriptorPoolSetContainer )
	{
		Assert( !pCurrentDescriptorPoolSetContainer );
		pCurrentDescriptorPoolSetContainer = g_StudioAPIVk.GetDescriptorPoolsMgr().AcquirePoolSet();
	}

	// Allocate descriptor sets if it need
	if ( bNeedDescriptorSets )
	{
		CStudioAPITypedDescriptorPoolSetVk* pTypedPoolSet = pCurrentDescriptorPoolSetContainer->AcquireTypedPoolSet( descriptorSetsLayout );
		return pTypedPoolSet->AllocateDescriptorSets( descriptorSetsLayout, pDestDescriptorSets );
	}

	return false;
}

/*
==================
CStudioAPICmdBufferVk::AllocMemory
==================
*/
void CStudioAPICmdBufferVk::AllocMemory()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Create the Vulkan command buffer
	if ( !bIsAllocatedMemory )
	{
		Assert( cmdBufferPool.IsValid() );
		VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo = {};
		vkCommandBufferAllocateInfo.sType						= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		vkCommandBufferAllocateInfo.level						= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		vkCommandBufferAllocateInfo.commandBufferCount			= 1;
		vkCommandBufferAllocateInfo.commandPool					= cmdBufferPool.GetVkCommandPool();
		STUDIOAPI_VK_VERIFY_RESULT( vkAllocateCommandBuffers( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkCommandBufferAllocateInfo, &vkCommandBuffer ) );
		bIsAllocatedMemory = true;
	}
}

/*
==================
CStudioAPICmdBufferVk::FreeMemory
==================
*/
void CStudioAPICmdBufferVk::FreeMemory()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Free allocated the Vulkan command buffer
	if ( bIsAllocatedMemory )
	{
		// Free command buffer
		Assert( vkCommandBuffer != VK_NULL_HANDLE );
		vkFreeCommandBuffers( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), cmdBufferPool.GetVkCommandPool(), 1, &vkCommandBuffer );

		// Release the descriptor pool set container
		if ( pCurrentDescriptorPoolSetContainer )
		{
			g_StudioAPIVk.GetDescriptorPoolsMgr().ReleasePoolSet( pCurrentDescriptorPoolSetContainer );
			pCurrentDescriptorPoolSetContainer = NULL;
		}

		// We are done, change state and clear the handle
		vkCommandBuffer	   = VK_NULL_HANDLE;
		bIsAllocatedMemory = false;
	}
}