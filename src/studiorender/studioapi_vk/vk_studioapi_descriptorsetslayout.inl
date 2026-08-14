#pragma once

/*
==================
CStudioAPIDescriptorSetsLayoutVk::HasInputAttachments
==================
*/
FORCEINLINE bool CStudioAPIDescriptorSetsLayoutVk::HasInputAttachments() const
{
	return GetNumUsedDescriptorTypes( VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT ) > 0;
}

/*
==================
CStudioAPIDescriptorSetsLayoutVk::GetVkDescriptorSetLayouts
==================
*/
FORCEINLINE const eastl::vector<VkDescriptorSetLayout>& CStudioAPIDescriptorSetsLayoutVk::GetVkDescriptorSetLayouts() const
{
	return vkDescriptorSetLayouts;
}

/*
==================
CStudioAPIDescriptorSetsLayoutVk::GetNumUsedDescriptorTypes
==================
*/
FORCEINLINE uint32 CStudioAPIDescriptorSetsLayoutVk::GetNumUsedDescriptorTypes( VkDescriptorType vkDescriptorType ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	auto it = numUsedDescriptorTypesDict.find( vkDescriptorType );
	if ( it != numUsedDescriptorTypesDict.end() )
	{
		return it->second;
	}
	return 0;
}

/*
==================
CStudioAPIDescriptorSetsLayoutVk::GetNumDescriptorSets
==================
*/
FORCEINLINE uint32 CStudioAPIDescriptorSetsLayoutVk::GetNumDescriptorSets() const
{
	return numDescriptorSets;
}

/*
==================
CStudioAPIDescriptorSetsLayoutVk::GetDescriptorTypesUsageHash
==================
*/
FORCEINLINE hash CStudioAPIDescriptorSetsLayoutVk::GetDescriptorTypesUsageHash() const
{
	return descriptorTypesUsageHash;
}

/*
==================
CStudioAPIDescriptorSetsLayoutVk::GetDescriptorSetInfos
==================
*/
FORCEINLINE const eastl::vector<studioAPIDescriptorSetInfoVk_t>& CStudioAPIDescriptorSetsLayoutVk::GetDescriptorSetInfos() const
{
	return descriptorSetInfos;
}

/*
==================
CStudioAPIDescriptorSetsLayoutVk::GetVkPipelineLayout
==================
*/
FORCEINLINE VkPipelineLayout CStudioAPIDescriptorSetsLayoutVk::GetVkPipelineLayout() const
{
	return vkPipelineLayout;
}

/*
==================
CStudioAPIDescriptorSetsLayoutVk::GetVkAllocateInfo
==================
*/
FORCEINLINE VkDescriptorSetAllocateInfo CStudioAPIDescriptorSetsLayoutVk::GetVkAllocateInfo() const
{
	return vkDescriptorSetAllocateInfo;
}

/*
==================
CStudioAPIDescriptorSetsLayoutVk::GetVkPushConstantRanges
==================
*/
FORCEINLINE const eastl::vector<VkPushConstantRange>& CStudioAPIDescriptorSetsLayoutVk::GetVkPushConstantRanges() const
{
	return vkPushConstantRanges;
}

/*
==================
CStudioAPIDescriptorSetsLayoutVk::OnDescriptorSetsLayoutDeleted
==================
*/
FORCEINLINE COnStudioAPIDescriptorSetsLayoutDeletedVk& CStudioAPIDescriptorSetsLayoutVk::OnDescriptorSetsLayoutDeleted() const
{
	return onDescriptorSetsLayoutDeleted;
}
