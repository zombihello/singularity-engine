#ifndef VK_STUDIOAPI_DESCRIPTORSETWRITER_INL
#define VK_STUDIOAPI_DESCRIPTORSETWRITER_INL

/*
==================
CStudioAPIDescriptorSetWriterVk::WriteConstantBuffer
==================
*/
FORCEINLINE bool CStudioAPIDescriptorSetWriterVk::WriteConstantBuffer( uint32 slot, CStudioAPIBufferVk* pConstantBuffer, uint64 offset, uint64 size )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	return WriteDescriptorBufferInfo<VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER>( slot, pConstantBuffer, 0, 0 );
}

/*
==================
CStudioAPIDescriptorSetWriterVk::WriteTexture
==================
*/
FORCEINLINE bool CStudioAPIDescriptorSetWriterVk::WriteTexture( uint32 slot, CStudioAPITextureVk* pTexture )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	return WriteDescriptorImageInfo<VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE>( slot, pTexture );
}

/*
==================
CStudioAPIDescriptorSetWriterVk::WriteSampler
==================
*/
FORCEINLINE bool CStudioAPIDescriptorSetWriterVk::WriteSampler( uint32 slot, CStudioAPISamplerVk* pSampler )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	return WriteDescriptorImageInfo<VK_DESCRIPTOR_TYPE_SAMPLER>( slot, pSampler );
}

/*
==================
CStudioAPIDescriptorSetWriterVk::SetVkDescriptorSet
==================
*/
FORCEINLINE void CStudioAPIDescriptorSetWriterVk::SetVkDescriptorSet( VkDescriptorSet vkDescriptorSet )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	for ( uint32 index = 0; index < numWriteDescriptorSets; ++index )
	{
		pVkWriteDescriptorSets[index].dstSet = vkDescriptorSet;
	}
}

/*
==================
CStudioAPIDescriptorSetWriterVk::IsEmpty
==================
*/
FORCEINLINE bool CStudioAPIDescriptorSetWriterVk::IsEmpty() const
{
	return numWriteDescriptorSets == 0;
}

/*
==================
CStudioAPIDescriptorSetWriterVk::WriteDescriptorBufferInfo
==================
*/
template<VkDescriptorType vkDescriptorType>
FORCEINLINE bool CStudioAPIDescriptorSetWriterVk::WriteDescriptorBufferInfo( uint32 slot, CStudioAPIBufferVk* pBuffer, uint64 offset, uint64 size )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Get Vulkan write descriptor set index
	if ( slot >= numSlots )
	{
		return false;
	}
	uint8	writeDescriptorIndex = pSlotToWriteDescriptorSetMap[slot];
	if ( writeDescriptorIndex == ( uint8 )-1 )
	{
		return false;
	}

	// Write data into Vulkan descriptor buffer info
	const VkWriteDescriptorSet* pVkWriteDescriptorSet = &pVkWriteDescriptorSets[writeDescriptorIndex];
	VkDescriptorBufferInfo* pVkDescriptorBufferInfo = ( VkDescriptorBufferInfo* )pVkWriteDescriptorSet->pBufferInfo;
	uint32							studioAPIBufferIndex = ( uint32 )( pVkDescriptorBufferInfo - pVkDescriptorBufferInfos );
	AssertMsg( pVkWriteDescriptorSet->descriptorType == vkDescriptorType, "vkDescriptorType mismatch at index %i", writeDescriptorIndex );
	Assert( pVkDescriptorBufferInfo );
	Assert( studioAPIBufferIndex < numBufferInfos );

	bool	bChanged = VK_CopyAndReturnNotEqual( pVkDescriptorBufferInfo->buffer, pBuffer ? pBuffer->GetVkBuffer() : VK_NULL_HANDLE );
	bChanged |= VK_CopyAndReturnNotEqual( pVkDescriptorBufferInfo->offset, pBuffer ? ( VkDeviceSize )pBuffer->GetOffset() + offset : 0 );
	bChanged |= VK_CopyAndReturnNotEqual( pVkDescriptorBufferInfo->range, pBuffer ? ( VkDeviceSize )pBuffer->GetSize() + size : 0 );
	pStudioAPIBuffers[studioAPIBufferIndex] = pBuffer;
	return bChanged;
}

/*
==================
CStudioAPIDescriptorSetWriterVk::WriteDescriptorImageInfo
==================
*/
template<VkDescriptorType vkDescriptorType>
FORCEINLINE bool CStudioAPIDescriptorSetWriterVk::WriteDescriptorImageInfo( uint32 slot, CStudioAPITextureVk* pTexture )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Get Vulkan write descriptor set index
	if ( slot >= numSlots )
	{
		return false;
	}
	uint8	writeDescriptorIndex = pSlotToWriteDescriptorSetMap[slot];
	if ( writeDescriptorIndex == ( uint8 )-1 )
	{
		return false;
	}

	// Write data into Vulkan descriptor image info
	const VkWriteDescriptorSet* pVkWriteDescriptorSet = &pVkWriteDescriptorSets[writeDescriptorIndex];
	VkDescriptorImageInfo* pVkDescriptorImageInfo = ( VkDescriptorImageInfo* )pVkWriteDescriptorSet->pImageInfo;
	uint32							studioAPITextureSamplerIndex = ( uint32 )( pVkDescriptorImageInfo - pVkDescriptorImageInfos );
	AssertMsg( pVkWriteDescriptorSet->descriptorType == vkDescriptorType, "vkDescriptorType mismatch at index %i", writeDescriptorIndex );
	Assert( pVkDescriptorImageInfo );
	Assert( studioAPITextureSamplerIndex < numImageInfos );

	bool	bChanged = VK_CopyAndReturnNotEqual<VkSampler>( pVkDescriptorImageInfo->sampler, VK_NULL_HANDLE );
	bChanged |= VK_CopyAndReturnNotEqual( pVkDescriptorImageInfo->imageView, pTexture ? pTexture->GetVkImageView() : VK_NULL_HANDLE );
	bChanged |= VK_CopyAndReturnNotEqual( pVkDescriptorImageInfo->imageLayout, pTexture ? pTexture->GetSyncState().vkImageLayout : VK_IMAGE_LAYOUT_UNDEFINED );

	// Remember pointer to the texture
	studioAPITextureSamplerPairVk_t& textureSamplerPair = pStudioAPITexturesSamplers[studioAPITextureSamplerIndex];
	textureSamplerPair.pStudioAPITexture = pTexture;
	textureSamplerPair.pStudioAPISampler = NULL;
	return bChanged;
}

/*
==================
CStudioAPIDescriptorSetWriterVk::WriteDescriptorImageInfo
==================
*/
template<VkDescriptorType vkDescriptorType>
FORCEINLINE bool CStudioAPIDescriptorSetWriterVk::WriteDescriptorImageInfo( uint32 slot, CStudioAPISamplerVk* pSampler )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Get Vulkan write descriptor set index
	if ( slot >= numSlots )
	{
		return false;
	}
	uint8	writeDescriptorIndex = pSlotToWriteDescriptorSetMap[slot];
	if ( writeDescriptorIndex == ( uint8 )-1 )
	{
		return false;
	}

	// Write data into Vulkan descriptor image info
	const VkWriteDescriptorSet* pVkWriteDescriptorSet = &pVkWriteDescriptorSets[writeDescriptorIndex];
	VkDescriptorImageInfo* pVkDescriptorImageInfo = ( VkDescriptorImageInfo* )pVkWriteDescriptorSet->pImageInfo;
	uint32							studioAPITextureSamplerIndex = ( uint32 )( pVkDescriptorImageInfo - pVkDescriptorImageInfos );
	AssertMsg( pVkWriteDescriptorSet->descriptorType == vkDescriptorType, "vkDescriptorType mismatch at index %i", writeDescriptorIndex );
	Assert( pVkDescriptorImageInfo );
	Assert( studioAPITextureSamplerIndex < numImageInfos );

	bool	bChanged = VK_CopyAndReturnNotEqual( pVkDescriptorImageInfo->sampler, pSampler ? pSampler->GetVkSampler() : VK_NULL_HANDLE );
	bChanged |= VK_CopyAndReturnNotEqual<VkImageView>( pVkDescriptorImageInfo->imageView, VK_NULL_HANDLE );
	bChanged |= VK_CopyAndReturnNotEqual( pVkDescriptorImageInfo->imageLayout, VK_IMAGE_LAYOUT_UNDEFINED );

	// Remember pointer to the sampler
	studioAPITextureSamplerPairVk_t& textureSamplerPair = pStudioAPITexturesSamplers[studioAPITextureSamplerIndex];
	textureSamplerPair.pStudioAPITexture = NULL;
	textureSamplerPair.pStudioAPISampler = pSampler;
	return bChanged;
}

#endif // !VK_STUDIOAPI_DESCRIPTORSETWRITER_INL
