#include "pch_studioapi_vk.h"
#include "studiorender/studioapi_vk/vk_studioapi_descriptorsetwriter.h"

/*
==================
CStudioAPIDescriptorSetWriterVk::CStudioAPIDescriptorSetWriterVk
==================
*/
CStudioAPIDescriptorSetWriterVk::CStudioAPIDescriptorSetWriterVk()
	: numWriteDescriptorSets( 0 )
	, numSlots( 0 )
	, numBufferInfos( 0 )
	, numImageInfos( 0 )
	, pVkWriteDescriptorSets( NULL )
	, pVkDescriptorBufferInfos( NULL )
	, pVkDescriptorImageInfos( NULL )
	, pStudioAPIBuffers( NULL )
	, pStudioAPITexturesSamplers( NULL )
	, pSlotToWriteDescriptorSetMap( NULL )
{
}

/*
==================
CStudioAPIDescriptorSetWriterVk::SetupDescriptorSetWrites
==================
*/
void CStudioAPIDescriptorSetWriterVk::SetupDescriptorSetWrites( const studioAPIDescriptorSetInfoVk_t& descriptorSetInfo, VkWriteDescriptorSet* pVkWriteDescriptorSets, VkDescriptorBufferInfo* pVkDescriptorBufferInfos, VkDescriptorImageInfo* pVkDescriptorImageInfos, CRefPtr<CStudioAPIBufferVk>* pStudioAPIBuffers, studioAPITextureSamplerPairVk_t* pStudioAPITexturesSamplers, uint8* pSlotToWriteDescriptorSetMap )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Remember some data
	CStudioAPIDescriptorSetWriterVk::pVkWriteDescriptorSets		  = pVkWriteDescriptorSets;
	CStudioAPIDescriptorSetWriterVk::pVkDescriptorBufferInfos	  = pVkDescriptorBufferInfos;
	CStudioAPIDescriptorSetWriterVk::pVkDescriptorImageInfos	  = pVkDescriptorImageInfos;
	CStudioAPIDescriptorSetWriterVk::pStudioAPIBuffers			  = pStudioAPIBuffers;
	CStudioAPIDescriptorSetWriterVk::pStudioAPITexturesSamplers	  = pStudioAPITexturesSamplers;
	CStudioAPIDescriptorSetWriterVk::pSlotToWriteDescriptorSetMap = pSlotToWriteDescriptorSetMap;
	numWriteDescriptorSets										  = (uint32)descriptorSetInfo.vkDescriptorTypes.size();
	numSlots													  = (uint32)descriptorSetInfo.slotToDescriptorTypeIndexMap.size();
	numBufferInfos												  = descriptorSetInfo.numBufferInfos;
	numImageInfos												  = descriptorSetInfo.numImageInfos;
	Assert( numSlots >= numWriteDescriptorSets );

	// Initialize Vulkan write descriptor sets
	for ( uint32 slot = 0; slot < numSlots; ++slot, ++pSlotToWriteDescriptorSetMap )
	{
		uint8 writeDescriptorTypeIndex = descriptorSetInfo.slotToDescriptorTypeIndexMap[slot];
		if ( writeDescriptorTypeIndex != (uint8)-1 )
		{
			pVkWriteDescriptorSets->sType			= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			pVkWriteDescriptorSets->dstBinding		= slot;
			pVkWriteDescriptorSets->descriptorCount = 1;
			pVkWriteDescriptorSets->descriptorType	= descriptorSetInfo.vkDescriptorTypes[writeDescriptorTypeIndex];
			*pSlotToWriteDescriptorSetMap			= writeDescriptorTypeIndex;

			switch ( pVkWriteDescriptorSets->descriptorType )
			{
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
				pVkWriteDescriptorSets->pBufferInfo = pVkDescriptorBufferInfos++;
				break;

			case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
			case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
			case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
			case VK_DESCRIPTOR_TYPE_SAMPLER:
				pVkWriteDescriptorSets->pImageInfo = pVkDescriptorImageInfos++;
				break;

			default:
				Sys_Error( "Unknown Vulkan descriptor type 0x%X", pVkWriteDescriptorSets->descriptorType );
				break;
			}
			++pVkWriteDescriptorSets;
		}
	}
}