#pragma once

/*
==================
shaderReflectionDescriptorSet_t::IsEmpty
==================
*/
FORCEINLINE bool shaderReflectionDescriptorSet_t::IsEmpty() const
{
	return	constantBuffersDict.empty() && storageBuffersDict.empty() &&
			sampledImagesDict.empty() && storageImagesDict.empty() &&
			separateTexturesDict.empty() && separateSamplersDict.empty();
}


/*
==================
CShaderReflection::Clear
==================
*/
FORCEINLINE void CShaderReflection::Clear()
{
	descriptorSets.clear();
	pushConstantBuffers.clear();
	pushConstantRanges.clear();
}

/*
==================
CShaderReflection::IsFreeBindingIndex
==================
*/
FORCEINLINE bool CShaderReflection::IsFreeBindingIndex( uint32 bindingIndex, uint32 descriptorSetIndex ) const
{
	// If we haven't the set or it is empty then the binding index is free
	if ( descriptorSetIndex >= ( uint32 )descriptorSets.size() )
	{
		return true;
	}

	const shaderReflectionDescriptorSet_t&	shaderDesciptorSet = descriptorSets[descriptorSetIndex];
	if ( shaderDesciptorSet.IsEmpty() )
	{
		return true;
	}

	// If in the set isn't used the binding index then it is free
	return	shaderDesciptorSet.constantBuffersDict.find( bindingIndex ) == shaderDesciptorSet.constantBuffersDict.end() &&
			shaderDesciptorSet.storageBuffersDict.find( bindingIndex ) == shaderDesciptorSet.storageBuffersDict.end() &&
			shaderDesciptorSet.sampledImagesDict.find( bindingIndex ) == shaderDesciptorSet.sampledImagesDict.end() &&
			shaderDesciptorSet.storageImagesDict.find( bindingIndex ) == shaderDesciptorSet.storageImagesDict.end() &&
			shaderDesciptorSet.separateTexturesDict.find( bindingIndex ) == shaderDesciptorSet.separateTexturesDict.end() &&
			shaderDesciptorSet.separateSamplersDict.find( bindingIndex ) == shaderDesciptorSet.separateSamplersDict.end();
}

/*
==================
CShaderReflection::GetDescriptorSets
==================
*/
FORCEINLINE const std::vector<shaderReflectionDescriptorSet_t>& CShaderReflection::GetDescriptorSets() const
{
	return descriptorSets;
}

/*
==================
CShaderReflection::GetPushConstantBuffers
==================
*/
FORCEINLINE const std::vector<shaderReflectionPushConstantBuffer_t>& CShaderReflection::GetPushConstantBuffers() const
{
	return pushConstantBuffers;
}

/*
==================
CShaderReflection::GetPushConstantRanges
==================
*/
FORCEINLINE const std::vector<shaderReflectionPushConstantRange_t>& CShaderReflection::GetPushConstantRanges() const
{
	return pushConstantRanges;
}
