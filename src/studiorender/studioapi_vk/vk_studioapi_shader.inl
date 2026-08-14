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
CStudioAPIVertexDeclarationVk::GetVkEmptyPipelineVertexInputStateInfo
==================
*/
FORCEINLINE const VkPipelineVertexInputStateCreateInfo& CStudioAPIVertexDeclarationVk::GetVkEmptyPipelineVertexInputStateInfo()
{
	static VkPipelineVertexInputStateCreateInfo s_vkEmptyPipelineVertexInputStateCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, NULL, 0, 0, NULL, 0, NULL };
	return s_vkEmptyPipelineVertexInputStateCreateInfo;
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
