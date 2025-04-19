#ifndef VK_STUDIOAPI_DESCRIPTORSETSLAYOUT_H
#define VK_STUDIOAPI_DESCRIPTORSETSLAYOUT_H

#include <vector>
#include <unordered_map>

#include "stdlib/hashing/fasthash.h"
#include "studiorender/studioapi/vk/vk_studioapi_device.h"
#include "studiorender/studioapi/vk/vk_studioapi_delegates.h"

//-----------------------------------------------------------------------------
// Vulkan descriptor set structs
//-----------------------------------------------------------------------------
struct studioAPIDescriptorSetInfoVk_t
{
	studioAPIDescriptorSetInfoVk_t()
		: numBufferInfos( 0 )
		, numImageInfos( 0 )
	{}

	uint16								numBufferInfos;
	uint16								numImageInfos;
	std::vector<VkDescriptorType>		vkDescriptorTypes;
	std::vector<uint8>					slotToDescriptorTypeIndexMap;
};


struct studioAPIDescriptorSetLayoutVk_t
{
	std::vector<VkDescriptorSetLayoutBinding>	vkLayoutBindings;
	studioAPIDescriptorSetInfoVk_t				descriptorSetInfo;
#if !RETAIL
	std::unordered_map<uint8, hash_t>			descriptorSlotValidationHashDict;
#endif // !RETAIL
};

typedef std::unordered_map<uint32, studioAPIDescriptorSetLayoutVk_t>	studioAPIDescriptorSetLayoutVkDict_t;


//-----------------------------------------------------------------------------
// Vulkan Studio API descriptor set layouts
//-----------------------------------------------------------------------------
class CStudioAPIDescriptorSetsLayoutVk
{
public:
	CStudioAPIDescriptorSetsLayoutVk();
	~CStudioAPIDescriptorSetsLayoutVk();

	void Init( const studioAPIDescriptorSetLayoutVkDict_t& descriptorSetLayoutDict, const std::vector<VkPushConstantRange>& vkPushConstantRanges );
	void Destroy();

	COnStudioAPIDescriptorSetsLayoutDeletedVk& OnDescriptorSetsLayoutDeleted() const;

	bool HasInputAttachments() const;
	const std::vector<VkDescriptorSetLayout>& GetVkDescriptorSetLayouts() const;
	uint32 GetNumUsedDescriptorTypes( VkDescriptorType vkDescriptorType ) const;
	uint32 GetNumDescriptorSets() const;
	hash_t GetDescriptorTypesUsageHash() const;
	const std::vector<studioAPIDescriptorSetInfoVk_t>& GetDescriptorSetInfos() const;
	VkPipelineLayout GetVkPipelineLayout() const;
	VkDescriptorSetAllocateInfo GetVkAllocateInfo() const;

private:
	uint32													numDescriptorSets;
	hash_t													descriptorTypesUsageHash;
	std::unordered_map<VkDescriptorType, uint32>			numUsedDescriptorTypesDict;
	std::vector<VkDescriptorSetLayout>						vkDescriptorSetLayouts;
	std::vector<studioAPIDescriptorSetInfoVk_t>				descriptorSetInfos;
	VkPipelineLayout										vkPipelineLayout;
	VkDescriptorSetAllocateInfo								vkDescriptorSetAllocateInfo;
	mutable COnStudioAPIDescriptorSetsLayoutDeletedVk		onDescriptorSetsLayoutDeleted;
};

#include "studiorender/studioapi/vk/vk_studioapi_descriptorsetslayout.inl"

#endif // !VK_STUDIOAPI_DESCRIPTORSETSLAYOUT_H