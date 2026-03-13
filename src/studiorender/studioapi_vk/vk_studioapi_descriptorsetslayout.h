#pragma once
#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>

#include "tier1/fasthash.h"
#include "studiorender/studioapi_vk/vk_studioapi_device.h"
#include "studiorender/studioapi_vk/vk_studioapi_delegates.h"

//-----------------------------------------------------------------------------
// Vulkan descriptor set structs
//-----------------------------------------------------------------------------
struct studioAPIDescriptorSetInfoVk_t
{
	studioAPIDescriptorSetInfoVk_t()
		: numBufferInfos( 0 )
		, numImageInfos( 0 )
	{
	}

	uint16							numBufferInfos;
	uint16							numImageInfos;
	eastl::vector<VkDescriptorType> vkDescriptorTypes;
	eastl::vector<uint8>			slotToDescriptorTypeIndexMap;
};

struct studioAPIDescriptorSetLayoutVk_t
{
	eastl::vector<VkDescriptorSetLayoutBinding> vkLayoutBindings;
	studioAPIDescriptorSetInfoVk_t				descriptorSetInfo;
#if !RETAIL
	eastl::unordered_map<uint8, hash> descriptorSlotValidationHashDict;
#endif	// !RETAIL
};

typedef eastl::unordered_map<uint32, studioAPIDescriptorSetLayoutVk_t> studioAPIDescriptorSetLayoutVkDict_t;

//-----------------------------------------------------------------------------
// Vulkan Studio API descriptor set layouts
//-----------------------------------------------------------------------------
class CStudioAPIDescriptorSetsLayoutVk
{
public:
	CStudioAPIDescriptorSetsLayoutVk();
	~CStudioAPIDescriptorSetsLayoutVk();

	void Init( const studioAPIDescriptorSetLayoutVkDict_t& descriptorSetLayoutDict, const eastl::vector<VkPushConstantRange>& vkPushConstantRanges );
	void Destroy();

	COnStudioAPIDescriptorSetsLayoutDeletedVk& OnDescriptorSetsLayoutDeleted() const;

	bool												 HasInputAttachments() const;
	const eastl::vector<VkDescriptorSetLayout>&			 GetVkDescriptorSetLayouts() const;
	uint32												 GetNumUsedDescriptorTypes( VkDescriptorType vkDescriptorType ) const;
	uint32												 GetNumDescriptorSets() const;
	hash												 GetDescriptorTypesUsageHash() const;
	const eastl::vector<studioAPIDescriptorSetInfoVk_t>& GetDescriptorSetInfos() const;
	VkPipelineLayout									 GetVkPipelineLayout() const;
	VkDescriptorSetAllocateInfo							 GetVkAllocateInfo() const;

private:
	uint32											  numDescriptorSets;
	hash											  descriptorTypesUsageHash;
	eastl::unordered_map<VkDescriptorType, uint32>	  numUsedDescriptorTypesDict;
	eastl::vector<VkDescriptorSetLayout>			  vkDescriptorSetLayouts;
	eastl::vector<studioAPIDescriptorSetInfoVk_t>	  descriptorSetInfos;
	VkPipelineLayout								  vkPipelineLayout;
	VkDescriptorSetAllocateInfo						  vkDescriptorSetAllocateInfo;
	mutable COnStudioAPIDescriptorSetsLayoutDeletedVk onDescriptorSetsLayoutDeleted;
};

#include "studiorender/studioapi_vk/vk_studioapi_descriptorsetslayout.inl"
