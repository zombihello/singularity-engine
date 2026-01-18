#pragma once
#include <vector>
#include <list>

#include "Volk/volk.h"
#include "studiorender/studioapi/vk/vk_studioapi_delegates.h"
#include "studiorender/studioapi/vk/vk_studioapi_descriptorsetslayout.h"

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

	const CStudioAPIDescriptorSetsLayoutVk&			 descriptorSetsLayout;
	std::list<CStudioAPIDescriptorPoolVk*>::iterator poolCurrentIt;
	std::list<CStudioAPIDescriptorPoolVk*>			 poolList;
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
	bool															bUsed;
	uint64															lastFrameUsed;
	std::unordered_map<hash_t, CStudioAPITypedDescriptorPoolSetVk*> typedDescriptorPoolsDict;
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

	std::list<CStudioAPIDescriptorPoolSetContainerVk*> poolSets;
	COnStudioAPIVkShutdown::funcDelegate_t*			   pStudioAPIVkShutdownDelegate;
};

#include "studiorender/studioapi/vk/vk_studioapi_descriptorpoolsmgr.inl"