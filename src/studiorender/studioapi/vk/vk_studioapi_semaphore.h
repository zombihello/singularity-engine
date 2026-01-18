#pragma once
#include "studiorender/studioapi/vk/vk_studioapi.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPISyncMgrVk;

//-----------------------------------------------------------------------------
// Vulkan StudioAPI semaphore
//-----------------------------------------------------------------------------
class CStudioAPISemaphoreVk
{
public:
	friend CStudioAPISyncMgrVk;

	CStudioAPISemaphoreVk()
		: bSignaled( false )
		, vkSemaphore( VK_NULL_HANDLE )
	{
		// Create semaphore
		VkSemaphoreCreateInfo vkSemaphoreCreateInfo = {};
		vkSemaphoreCreateInfo.sType					= VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		STUDIOAPI_VK_VERIFY_RESULT( vkCreateSemaphore( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkSemaphoreCreateInfo, NULL, &vkSemaphore ) );
	}
	~CStudioAPISemaphoreVk()
	{
		// TODO yehor.pohuliaka - Implement deferred deletion
		Destroy();
	}

	void Signal();
	void Unsignal();

	bool		IsSignaled() const;
	VkSemaphore GetVkSemaphore() const;

private:
	void Destroy();

	bool		bSignaled;
	VkSemaphore vkSemaphore;
};

//-----------------------------------------------------------------------------
// Vulkan StudioAPI named semaphore
//-----------------------------------------------------------------------------
class CStudioAPINamedSemaphoreVk
{
public:
	friend CStudioAPISyncMgrVk;

	CStudioAPISemaphoreVk* GetCurrentSemaphore() const;
	const std::string&	   GetName() const;

						   operator CStudioAPISemaphoreVk*() const;
	CStudioAPISemaphoreVk* operator->() const;

private:
	CStudioAPINamedSemaphoreVk()
		: countReferences( 1 )
	{
	}
	~CStudioAPINamedSemaphoreVk()
	{
	}

	uint32				   countReferences;
	CStudioAPISemaphoreVk* pSemaphores[STUDIOAPI_VK_NUM_FRAMES_IN_FLIGHT];
	std::string			   name;
};

#include "studiorender/studioapi/vk/vk_studioapi_semaphore.inl"