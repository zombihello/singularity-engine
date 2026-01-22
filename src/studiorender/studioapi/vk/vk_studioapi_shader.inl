#pragma once

/*
==================
CStudioAPIVertexDeclarationVk::GetVkPipelineVertexInputStateInfo
==================
*/
FORCEINLINE const VkPipelineVertexInputStateCreateInfo& CStudioAPIVertexDeclarationVk::GetVkPipelineVertexInputStateInfo() const
{
	return vkPipelineVertexInputStateInfo;
}

/*
==================
CStudioAPIShaderVk::GetVkShaderModule
==================
*/
FORCEINLINE VkShaderModule CStudioAPIShaderVk::GetVkShaderModule() const
{
	return vkShaderModule;
}

/*
==================
CStudioAPIShaderVk::GetVkPipelineShaderStageInfo
==================
*/
FORCEINLINE const VkPipelineShaderStageCreateInfo& CStudioAPIShaderVk::GetVkPipelineShaderStageInfo() const
{
	return vkPipelineShaderStageInfo;
}

/*
==================
CStudioAPIShaderVk::GetEntryPointName
==================
*/
FORCEINLINE const eastl::string& CStudioAPIShaderVk::GetEntryPointName() const
{
	return entryPointName;
}

/*
==================
CStudioAPIShaderVk::GetDescriptorSetLayoutDict
==================
*/
FORCEINLINE const studioAPIDescriptorSetLayoutVkDict_t& CStudioAPIShaderVk::GetDescriptorSetLayoutDict() const
{
	return descriptorSetLayoutDict;
}

/*
==================
CStudioAPIShaderVk::GetVkPushConstantRanges
==================
*/
FORCEINLINE const eastl::vector<VkPushConstantRange>& CStudioAPIShaderVk::GetVkPushConstantRanges() const
{
	return vkPushConstantRanges;
}

/*
==================
CStudioAPIBoundShaderStateVk::GetKey
==================
*/
FORCEINLINE const CStudioAPIBoundShaderStateKeyVk& CStudioAPIBoundShaderStateVk::GetKey() const
{
	return key;
}

/*
==================
CStudioAPIBoundShaderStateVk::GetDescriptorSetsLayout
==================
*/
FORCEINLINE const CStudioAPIDescriptorSetsLayoutVk& CStudioAPIBoundShaderStateVk::GetDescriptorSetsLayout() const
{
	return descriptorSetsLayout;
}
