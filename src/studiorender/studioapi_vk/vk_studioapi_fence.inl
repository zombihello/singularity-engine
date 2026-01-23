#pragma once

/*
==================
CStudioAPIFenceVk::IsSignaled
==================
*/
FORCEINLINE bool CStudioAPIFenceVk::IsSignaled() const
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Do nothing if the state already is FENCE_STATE_SIGNALED
	if ( state == FENCE_STATE_SIGNALED )
	{
		return true;
	}

	// Get the fence status by Vulkan
	VkResult vkResult = vkGetFenceStatus( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkFence );
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

/*
==================
CStudioAPIFenceVk::Wait
==================
*/
FORCEINLINE bool CStudioAPIFenceVk::Wait( uint64 waitTime )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_WAIT );

	// Do nothing if the state is FENCE_STATE_SIGNALED
	if ( IsSignaled() )
	{
		return true;
	}

	// Wait for fence by Vulkan
	VkResult vkResult = vkWaitForFences( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), 1, &vkFence, true, waitTime );
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

/*
==================
CStudioAPIFenceVk::Reset
==================
*/
FORCEINLINE void CStudioAPIFenceVk::Reset()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Reset fence only if the state is signaled
	if ( IsSignaled() )
	{
		STUDIOAPI_VK_VERIFY_RESULT( vkResetFences( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), 1, &vkFence ) );
		state = FENCE_STATE_NOT_READY;
	}
}

/*
==================
CStudioAPIFenceVk::WaitAndReset
==================
*/
FORCEINLINE bool CStudioAPIFenceVk::WaitAndReset( uint64 waitTime )
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

/*
==================
CStudioAPIFenceVk::GetVkFence
==================
*/
FORCEINLINE VkFence CStudioAPIFenceVk::GetVkFence() const
{
	return vkFence;
}

/*
==================
CStudioAPIFenceVk::Destroy
==================
*/
FORCEINLINE void CStudioAPIFenceVk::Destroy()
{
	// Destroy the Vulkan fence
	if ( vkFence != VK_NULL_HANDLE )
	{
		vkDestroyFence( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkFence, NULL );
		vkFence = VK_NULL_HANDLE;
	}
}
