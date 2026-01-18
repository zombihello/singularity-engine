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
FORCEINLINE const std::vector<VkDescriptorSetLayout>& CStudioAPIDescriptorSetsLayoutVk::GetVkDescriptorSetLayouts() const
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
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
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
FORCEINLINE hash_t CStudioAPIDescriptorSetsLayoutVk::GetDescriptorTypesUsageHash() const
{
	return descriptorTypesUsageHash;
}

/*
==================
CStudioAPIDescriptorSetsLayoutVk::GetDescriptorSetInfos
==================
*/
FORCEINLINE const std::vector<studioAPIDescriptorSetInfoVk_t>& CStudioAPIDescriptorSetsLayoutVk::GetDescriptorSetInfos() const
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
CStudioAPIDescriptorSetsLayoutVk::OnDescriptorSetsLayoutDeleted
==================
*/
FORCEINLINE COnStudioAPIDescriptorSetsLayoutDeletedVk& CStudioAPIDescriptorSetsLayoutVk::OnDescriptorSetsLayoutDeleted() const
{
	return onDescriptorSetsLayoutDeleted;
}
