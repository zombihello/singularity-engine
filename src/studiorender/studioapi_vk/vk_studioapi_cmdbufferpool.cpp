#include "pch_studioapi_vk.h"
#include "studiorender/studioapi_vk/vk_studioapi.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdbuffer.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdbufferpool.h"

/*
==================
CStudioAPICmdBufferPoolVk::CStudioAPICmdBufferPoolVk
==================
*/
CStudioAPICmdBufferPoolVk::CStudioAPICmdBufferPoolVk( CStudioAPIQueueVk& queue )
	: vkCommandPool( VK_NULL_HANDLE )
	, queue( queue )
{
}

/*
==================
CStudioAPICmdBufferPoolVk::~CStudioAPICmdBufferPoolVk
==================
*/
CStudioAPICmdBufferPoolVk::~CStudioAPICmdBufferPoolVk()
{
	Shutdown();
}

/*
==================
CStudioAPICmdBufferPoolVk::Init
==================
*/
void CStudioAPICmdBufferPoolVk::Init()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Create the command pool
	Assert( vkCommandPool == VK_NULL_HANDLE );
	VkCommandPoolCreateInfo vkCommandPoolCreateInfo = {};
	vkCommandPoolCreateInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	vkCommandPoolCreateInfo.flags					= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	vkCommandPoolCreateInfo.queueFamilyIndex		= queue.GetQueueFamilyIndex();
	STUDIOAPI_VK_VERIFY_RESULT( vkCreateCommandPool( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkCommandPoolCreateInfo, NULL, &vkCommandPool ) );
}

/*
==================
CStudioAPICmdBufferPoolVk::Shutdown
==================
*/
void CStudioAPICmdBufferPoolVk::Shutdown()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Destroy all command buffers
	for ( auto it = cmdBuffers.begin(), itEnd = cmdBuffers.end(); it != itEnd; ++it )
	{
		delete *it;
	}
	cmdBuffers.clear();

	for ( auto it = freeCmdBuffers.begin(), itEnd = freeCmdBuffers.end(); it != itEnd; ++it )
	{
		delete *it;
	}
	freeCmdBuffers.clear();

	// Destroy the Vulkan command pool
	if ( vkCommandPool != VK_NULL_HANDLE )
	{
		vkDestroyCommandPool( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkCommandPool, NULL );
		vkCommandPool = VK_NULL_HANDLE;
	}
}

/*
==================
CStudioAPICmdBufferPoolVk::CreateCmdBuffer
==================
*/
CStudioAPICmdBufferVk* CStudioAPICmdBufferPoolVk::CreateCmdBuffer()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Try to find the command buffer in the free list
	CStudioAPICmdBufferVk* pCmdBuffer = NULL;
	if ( !freeCmdBuffers.empty() )
	{
		pCmdBuffer = freeCmdBuffers.front();
		freeCmdBuffers.pop_front();
	}
	// Otherwise create a new one if we haven't any free buffer
	else
	{
		pCmdBuffer = new CStudioAPICmdBufferVk( *this );
	}

	// Allocate memory for the buffer
	cmdBuffers.emplace_back( pCmdBuffer );
	pCmdBuffer->AllocMemory();

	// We are done
	return pCmdBuffer;
}

/*
==================
CStudioAPICmdBufferPoolVk::FreeUnusedCmdBuffers
==================
*/
void CStudioAPICmdBufferPoolVk::FreeUnusedCmdBuffers()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Reset the command pool
	vkResetCommandPool( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkCommandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT );

	// Free all command buffers
	for ( auto it = cmdBuffers.begin(), itEnd = cmdBuffers.end(); it != itEnd; ++it )
	{
		CStudioAPICmdBufferVk* pCmdBuffer = *it;
		pCmdBuffer->FreeMemory();
		freeCmdBuffers.emplace_back( pCmdBuffer );
	}
	cmdBuffers.clear();
}