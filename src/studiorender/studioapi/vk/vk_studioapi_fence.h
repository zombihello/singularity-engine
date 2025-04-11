/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_FENCE_H
#define VK_STUDIOAPI_FENCE_H

#include "studiorender/studioapi/vk/vk_studioapi.h"

// Forward declarations
class CStudioAPISyncMgrVk;

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI fence
 */
class CStudioAPIFenceVk
{
public:
	friend CStudioAPISyncMgrVk;

	/**
	 * @brief Constructor
	 * @param bCreateSignaled		Is need create the fence in signaled state
	 */
	CStudioAPIFenceVk( bool bCreateSignaled = false )
		: state( bCreateSignaled ? FENCE_STATE_SIGNALED : FENCE_STATE_NOT_READY )
		, vkFence( VK_NULL_HANDLE )
	{
		// Create fence
		VkFenceCreateInfo				vkFenceCreateInfo = {};
		vkFenceCreateInfo.sType			= VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		vkFenceCreateInfo.flags			= bCreateSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
		STUDIOAPI_VK_VERIFY_RESULT( vkCreateFence( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkFenceCreateInfo, NULL, &vkFence ) );
	}

	/**
	 * @brief Destructor
	 */
	~CStudioAPIFenceVk()
	{
		// TODO yehor.pohuliaka - Implement deferred deletion
		Destroy();
	}

	/**
	 * @brief Is signaled
	 * @return Return TRUE if the fence is signaled, otherwise FALSE
	 */
	FORCEINLINE bool IsSignaled() const
	{
		PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

		// Do nothing if the state already is FENCE_STATE_SIGNALED
		if ( state == FENCE_STATE_SIGNALED )
		{
			return true;
		}

		// Get the fence status by Vulkan
		VkResult	vkResult = vkGetFenceStatus( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkFence );
		switch ( vkResult )
		{
		case VK_SUCCESS:
			state = FENCE_STATE_SIGNALED;
			return true;

		case VK_NOT_READY:
			break;

		default:
			STUDIOAPI_VK_VERIFY_RESULT( vkResult );
			break;
		}

		return false;
	}

	/**
	 * @brief Wait
	 * @param waitTime		Wait time in nanoseconds
	 * @return Return TRUE if the fence has been signaled, otherwise FALSE
	 */
	FORCEINLINE bool Wait( uint64 waitTime )
	{
		PROFILE_SCOPE( PROFILE_SCOPE_GROUP_WAIT );

		// Do nothing if the state is FENCE_STATE_SIGNALED
		if ( IsSignaled() )
		{
			return true;
		}

		// Wait for fence by Vulkan
		VkResult	vkResult = vkWaitForFences( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), 1, &vkFence, true, waitTime );
		switch ( vkResult )
		{
		case VK_SUCCESS:
			state = FENCE_STATE_SIGNALED;
			return true;

		case VK_TIMEOUT:
			return false;
			break;

		default:
			STUDIOAPI_VK_VERIFY_RESULT( vkResult );
			break;
		}

		return true;
	}

	/**
	 * @brief Reset
	 */
	FORCEINLINE void Reset()
	{
		PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

		// Reset fence only if the state is signaled
		if ( IsSignaled() )
		{
			STUDIOAPI_VK_VERIFY_RESULT( vkResetFences( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), 1, &vkFence ) );
			state = FENCE_STATE_NOT_READY;
		}
	}

	/**
	 * @brief Wait and reset
	 * @param waitTime		Wait time in nanoseconds
	 * @return Return TRUE if the fence has been signaled, otherwise FALSE
	 */
	FORCEINLINE bool WaitAndReset( uint64 waitTime )
	{
		PROFILE_SCOPE( PROFILE_SCOPE_GROUP_WAIT );

		// Wait for the fence
		if ( !IsSignaled() && !Wait( waitTime ) )
		{
			return false;
		}

		// Reset the fence
		Reset();
	}

	/**
	 * @brief Get Vulkan fence
	 * @return Return Vulkan fence
	 */
	FORCEINLINE VkFence GetVkFence() const
	{
		return vkFence;
	}

private:
	/**
	 * @brief Fence state
	 */
	enum fenceState_t
	{
		FENCE_STATE_NOT_READY,		/**< Fence isn't ready */
		FENCE_STATE_SIGNALED		/**< Fence is signaled */
	};

	/**
	 * @brief Destroy
	 */
	FORCEINLINE void Destroy()
	{
		// Destroy the Vulkan fence
		if ( vkFence != VK_NULL_HANDLE )
		{
			vkDestroyFence( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkFence, NULL );
			vkFence = VK_NULL_HANDLE;
		}
	}

	mutable fenceState_t	state;				/**< Fence state */
	VkFence					vkFence;			/**< Vulkan fence */
};

#endif // !VK_STUDIOAPI_FENCE_H