#pragma once
#include <EASTL/vector.h>
#include <EASTL/list.h>

#include "Volk/volk.h"
#include "studiorender/studioapi_vk/vk_studioapi_delegates.h"
#include "studiorender/studioapi_vk/vk_studioapi_descriptorsetslayout.h"

//-----------------------------------------------------------------------------
// Vulkan StudioAPI descriptor pool
//-----------------------------------------------------------------------------
class CStudioAPIDescriptorPoolVk
{
public:
	CStudioAPIDescriptorPoolVk( const CStudioAPIDescriptorSetsLayoutVk& descriptorSetsLayout, uint32 maxSetsAllocations );
	~CStudioAPIDescriptorPoolVk();

	void			 Reset();
	bool			 AllocateDescriptorSets( const VkDescriptorSetAllocateInfo& vkDescriptorSetAllocateInfo, VkDescriptorSet* pDestDescriptorSets );
	VkDescriptorPool GetVkDescriptorPool() const;

private:
	VkDescriptorPool vkDescriptorPool;
};

//-----------------------------------------------------------------------------
// Vulkan StudioAPI typed descriptor pool set
//-----------------------------------------------------------------------------
class CStudioAPITypedDescriptorPoolSetVk
{
public:
	CStudioAPITypedDescriptorPoolSetVk( const CStudioAPIDescriptorSetsLayoutVk& descriptorSetsLayout );
	~CStudioAPITypedDescriptorPoolSetVk();

	void									Reset();
	bool									AllocateDescriptorSets( const CStudioAPIDescriptorSetsLayoutVk& descriptorSetsLayout, VkDescriptorSet* pDestDescriptorSets );
	const CStudioAPIDescriptorSetsLayoutVk& GetDescriptorSetsLayout() const;

private:
	CStudioAPIDescriptorPoolVk* GetFreePool( bool bForceNewPool );
	CStudioAPIDescriptorPoolVk* PushNewPool();

	const CStudioAPIDescriptorSetsLayoutVk&			   descriptorSetsLayout;
	eastl::list<CStudioAPIDescriptorPoolVk*>::iterator poolCurrentIt;
	eastl::list<CStudioAPIDescriptorPoolVk*>		   poolList;
};

//-----------------------------------------------------------------------------
// Vulkan StudioAPI descriptor pool set container
//-----------------------------------------------------------------------------
class CStudioAPIDescriptorPoolSetContainerVk
{
public:
	CStudioAPIDescriptorPoolSetContainerVk();
	~CStudioAPIDescriptorPoolSetContainerVk();

	CStudioAPITypedDescriptorPoolSetVk* AcquireTypedPoolSet( const CStudioAPIDescriptorSetsLayoutVk& descriptorSetsLayout );
	void								Reset();

	void   SetUsed( bool bUsed );
	bool   IsUsed() const;
	uint64 GetLastFrameUsed() const;

private:
	bool															  bUsed;
	uint64															  lastFrameUsed;
	eastl::unordered_map<hash, CStudioAPITypedDescriptorPoolSetVk*> typedDescriptorPoolsDict;
};

//-----------------------------------------------------------------------------
// Vulkan StudioAPI descriptor pools manager
//-----------------------------------------------------------------------------
class CStudioAPIDescriptorPoolsMgrVk
{
public:
	CStudioAPIDescriptorPoolsMgrVk();
	~CStudioAPIDescriptorPoolsMgrVk();

	void Init();
	void Shutdown();

	CStudioAPIDescriptorPoolSetContainerVk* AcquirePoolSet();
	void									ReleasePoolSet( CStudioAPIDescriptorPoolSetContainerVk* pPoolSet );
	void									FreeUnusedPoolSets();

private:
	static void OnStudioAPIVkShutdown( void* pUserData );

	eastl::list<CStudioAPIDescriptorPoolSetContainerVk*> poolSets;
	COnStudioAPIVkShutdown::funcDelegate_t*				 pStudioAPIVkShutdownDelegate;
};

#include "studiorender/studioapi_vk/vk_studioapi_descriptorpoolsmgr.inl"
