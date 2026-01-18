#include "pch_studioapi.h"
#include "studiorender/studioapi/vk/vk_studioapi_queue.h"
#include "studiorender/studioapi/vk/vk_helpers.h"
#include "studiorender/studioapi/vk/vk_studioapi.h"

/*
==================
CStudioAPIQueueSharingModeSetupVk::CStudioAPIQueueSharingModeSetupVk
==================
*/
CStudioAPIQueueSharingModeSetupVk::CStudioAPIQueueSharingModeSetupVk( uint32 queueFamilyIndex0, uint32 queueFamilyIndex1, uint32 queueFamilyIndex2 /* = VK_QUEUE_FAMILY_IGNORED */, uint32 queueFamilyIndex3 /* = VK_QUEUE_FAMILY_IGNORED */ )
	: numQueueFamilyIndices( 0 )
{
	// Clear queue family indices
	uint32 queueFamilyIndexParams[4] = { queueFamilyIndex0, queueFamilyIndex1, queueFamilyIndex2, queueFamilyIndex3 };
	Mem_Memset( queueFamilyIndices, (uint8)VK_QUEUE_FAMILY_IGNORED, 4 * sizeof( uint32 ) );

	// Initialize queue family indices
	for ( uint32 paramIdx = 0; paramIdx < 4; ++paramIdx )
	{
		uint32 param = queueFamilyIndexParams[paramIdx];
		if ( param == VK_QUEUE_FAMILY_IGNORED )
		{
			continue;
		}

		for ( uint32 queueFamilyIndexIdx = 0; queueFamilyIndexIdx < 4; ++queueFamilyIndexIdx )
		{
			uint32 queueFamilyIndex = queueFamilyIndices[queueFamilyIndexIdx];
			if ( queueFamilyIndex == param )
			{
				break;
			}
			else if ( queueFamilyIndex != VK_QUEUE_FAMILY_IGNORED )
			{
				continue;
			}

			queueFamilyIndices[queueFamilyIndexIdx] = param;
			++numQueueFamilyIndices;
			break;
		}
	}
}

/*
==================
CStudioAPIQueueVk::CStudioAPIQueueVk
==================
*/
CStudioAPIQueueVk::CStudioAPIQueueVk()
	: queueFamilyIndex( -1 )
	, vkQueue( VK_NULL_HANDLE )
{
	Mem_Memzero( &vkQueueFamilyProperties, sizeof( VkQueueFamilyProperties ) );
}

/*
==================
CStudioAPIQueueVk::~CStudioAPIQueueVk
==================
*/
CStudioAPIQueueVk::~CStudioAPIQueueVk()
{
	Shutdown();
}

/*
==================
CStudioAPIQueueVk::Init
==================
*/
void CStudioAPIQueueVk::Init( uint32 queueFamilyIndex, const VkQueueFamilyProperties& vkQueueFamilyProperties )
{
	// Remember us queue family index, properties and get from the logical device a queue
	CStudioAPIQueueVk::queueFamilyIndex		   = queueFamilyIndex;
	CStudioAPIQueueVk::vkQueueFamilyProperties = vkQueueFamilyProperties;
	vkGetDeviceQueue( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), queueFamilyIndex, 0, &vkQueue );
}

/*
==================
CStudioAPIQueueVk::Shutdown
==================
*/
void CStudioAPIQueueVk::Shutdown()
{
	// Clear all fields
	queueFamilyIndex = -1;
	vkQueue			 = VK_NULL_HANDLE;
	Mem_Memzero( &vkQueueFamilyProperties, sizeof( VkQueueFamilyProperties ) );
}

/*
==================
CStudioAPIQueueVk::Submit
==================
*/
void CStudioAPIQueueVk::Submit( VkSubmitInfo* pVkSubmitInfos, uint32 numVkSubmitInfos, CStudioAPIFenceVk* pFence /* = NULL */ )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	VkFence vkFence = pFence ? pFence->GetVkFence() : VK_NULL_HANDLE;
	STUDIOAPI_VK_VERIFY_RESULT( vkQueueSubmit( vkQueue, numVkSubmitInfos, pVkSubmitInfos, vkFence ) );
}