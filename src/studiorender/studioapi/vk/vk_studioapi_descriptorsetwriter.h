/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_DESCRIPTORSETWRITER_H
#define VK_STUDIOAPI_DESCRIPTORSETWRITER_H

#include "Volk/volk.h"

#include "studiorender/studioapi/vk/vk_studioapi_descriptorsetslayout.h"
#include "studiorender/studioapi/vk/vk_studioapi_buffer.h"
#include "studiorender/studioapi/vk/vk_studioapi_texture.h"

/**
 * @ingroup studioapi_vk
 * @brief This container holds texture and sampler
 */
struct studioAPITextureSamplerPairVk_t
{
	TRefPtr<CStudioAPITextureVk>		pStudioAPITexture;	/**< StudioAPI texture */
	TRefPtr<CStudioAPISamplerVk>		pStudioAPISampler;	/**< StudioAPI sampler */
};

/**
 * @ingroup studioapi_vk
 * @brief This container holds the actual VkWriteDescriptorSet structures
 */
struct studioAPIDescriptorSetWriteContainerVk_t
{
	std::vector<VkWriteDescriptorSet>				vkWriteDescriptorSets;			/**< Vulkan write descriptor set */
	std::vector<VkDescriptorBufferInfo>				vkDescriptorBufferInfos;		/**< Vulkan descriptor buffer infos */
	std::vector<VkDescriptorImageInfo>				vkDescriptorImageInfos;			/**< Vulkan descriptor image infos */
	std::vector<TRefPtr<CStudioAPIBufferVk>>		studioAPIBuffers;				/**< StudioAPI buffers */
	std::vector<studioAPITextureSamplerPairVk_t>	studioAPITexturesSamplers;		/**< StudioAPI textures and samplers */
	std::vector<uint8>								slotToWriteDescriptorSetMap;	/**< Map to convert slot index to write descriptor set index */
};

/**
 * @ingroup studioapi_vk
 * @brief This class encapsulates updating VkWriteDescriptorSet structures but doesn't own them.
 * It is intended to be used to access a sub-region of a long array of VkWriteDescriptorSet (ie studioAPIDescriptorSetWriteContainerVk_t)
 */
class CStudioAPIDescriptorSetWriterVk
{
public:
	/**
	 * @brief Constructor
	 */
	CStudioAPIDescriptorSetWriterVk();

	/**
	 * @brief Setup descriptor set writes
	 * @param descriptorSetInfo				Descriptor set info
	 * @param pVkWriteDescriptorSets		Vulkan write descriptor sets
	 * @param pVkDescriptorBufferInfos		Vulkan descriptor buffer infos
	 * @param pVkDescriptorImageInfos		Vulkan descriptor image infos
	 * @param pStudioAPIBuffers				StudioAPI buffers
	 * @param pStudioAPITexturesSamplers	StudioAPI textures and samplers
	 * @param pSlotToWriteDescriptorSetMap	Slot to write descriptor set map
	 */
	void SetupDescriptorSetWrites( const studioAPIDescriptorSetInfoVk_t& descriptorSetInfo, VkWriteDescriptorSet* pVkWriteDescriptorSets, VkDescriptorBufferInfo* pVkDescriptorBufferInfos, VkDescriptorImageInfo* pVkDescriptorImageInfos, TRefPtr<CStudioAPIBufferVk>* pStudioAPIBuffers, studioAPITextureSamplerPairVk_t* pStudioAPITexturesSamplers, uint8* pSlotToWriteDescriptorSetMap );

	/**
	 * @brief Write a constant buffer
	 * @param slot				Slot index
	 * @param pConstantBuffer	StudioAPI constant buffer
	 * @param offset			Offset in the buffer
	 * @param size				Buffer size
	 * @return Return TRUE if the descriptor slot has been changed, otherwise FALSE
	 */
	FORCEINLINE bool WriteConstantBuffer( uint32 slot, CStudioAPIBufferVk* pConstantBuffer, uint64 offset, uint64 size )
	{
		PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
		return WriteDescriptorBufferInfo<VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER>( slot, pConstantBuffer, 0, 0 );
	}

	/**
	 * @brief Write a texture
	 * @param slot				Slot index
	 * @param pTexture			StudioAPI texture
	 * @return Return TRUE if the descriptor slot has been changed, otherwise FALSE
	 */
	FORCEINLINE bool WriteTexture( uint32 slot, CStudioAPITextureVk* pTexture )
	{
		PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
		return WriteDescriptorImageInfo<VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE>( slot, pTexture );
	}

	/**
	 * @brief Write a sampler
	 * @param slot				Slot index
	 * @param pSampler			StudioAPI sampler
	 * @return Return TRUE if the descriptor slot has been changed, otherwise FALSE
	 */
	FORCEINLINE bool WriteSampler( uint32 slot, CStudioAPISamplerVk* pSampler )
	{
		PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
		return WriteDescriptorImageInfo<VK_DESCRIPTOR_TYPE_SAMPLER>( slot, pSampler );
	}

	/**
	 * @brief Set Vulkan descriptor set
	 * @param vkDescriptorSet	Vulkan descriptor set
	 */
	FORCEINLINE void SetVkDescriptorSet( VkDescriptorSet vkDescriptorSet )
	{
		PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
		for ( uint32 index = 0; index < numWriteDescriptorSets; ++index )
		{
			pVkWriteDescriptorSets[index].dstSet = vkDescriptorSet;
		}
	}

	/**
	 * @brief Is the set empty
	 * @return Return TRUE if the set is empty, otherwise FALSE
	 */
	FORCEINLINE bool IsEmpty() const
	{
		return numWriteDescriptorSets == 0;
	}

private:
	/**
	 * @brief Write descriptor buffer info
	 * @param slot				Slot index
	 * @param pBuffer			StudioAPI buffer
	 * @param offset			Offset in the buffer
	 * @param size				Buffer size
	 * @return Return TRUE if the descriptor buffer info has been changed, otherwise FALSE
	 */
	template<VkDescriptorType vkDescriptorType>
	FORCEINLINE bool WriteDescriptorBufferInfo( uint32 slot, CStudioAPIBufferVk* pBuffer, uint64 offset, uint64 size )
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
		const VkWriteDescriptorSet*		pVkWriteDescriptorSet	= &pVkWriteDescriptorSets[writeDescriptorIndex];
		VkDescriptorBufferInfo*			pVkDescriptorBufferInfo = ( VkDescriptorBufferInfo* )pVkWriteDescriptorSet->pBufferInfo;
		uint32							studioAPIBufferIndex	= ( uint32 )( pVkDescriptorBufferInfo - pVkDescriptorBufferInfos );
		AssertMsg( pVkWriteDescriptorSet->descriptorType == vkDescriptorType, "vkDescriptorType mismatch at index %i", writeDescriptorIndex );
		Assert( pVkDescriptorBufferInfo );
		Assert( studioAPIBufferIndex < numBufferInfos );

		bool	bChanged = VK_CopyAndReturnNotEqual( pVkDescriptorBufferInfo->buffer, pBuffer ? pBuffer->GetVkBuffer() : VK_NULL_HANDLE );
		bChanged |= VK_CopyAndReturnNotEqual( pVkDescriptorBufferInfo->offset, pBuffer ? ( VkDeviceSize )pBuffer->GetOffset() + offset : 0 );
		bChanged |= VK_CopyAndReturnNotEqual( pVkDescriptorBufferInfo->range, pBuffer ? ( VkDeviceSize )pBuffer->GetSize() + size : 0 );
		pStudioAPIBuffers[studioAPIBufferIndex] = pBuffer;
		return bChanged;
	}

	/**
	 * @brief Write descriptor image info
	 * @param slot			Slot index
	 * @param pTexture		StudioAPI texture
	 * @return Return TRUE if the descriptor image info has been changed, otherwise FALSE
	 */
	template<VkDescriptorType vkDescriptorType>
	FORCEINLINE bool WriteDescriptorImageInfo( uint32 slot, CStudioAPITextureVk* pTexture )
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
		const VkWriteDescriptorSet*		pVkWriteDescriptorSet			= &pVkWriteDescriptorSets[writeDescriptorIndex];
		VkDescriptorImageInfo*			pVkDescriptorImageInfo			= ( VkDescriptorImageInfo* )pVkWriteDescriptorSet->pImageInfo;
		uint32							studioAPITextureSamplerIndex	= ( uint32 )( pVkDescriptorImageInfo - pVkDescriptorImageInfos );
		AssertMsg( pVkWriteDescriptorSet->descriptorType == vkDescriptorType, "vkDescriptorType mismatch at index %i", writeDescriptorIndex );
		Assert( pVkDescriptorImageInfo );
		Assert( studioAPITextureSamplerIndex < numImageInfos );

		bool	bChanged = VK_CopyAndReturnNotEqual<VkSampler>( pVkDescriptorImageInfo->sampler, VK_NULL_HANDLE );
		bChanged |= VK_CopyAndReturnNotEqual( pVkDescriptorImageInfo->imageView, pTexture ? pTexture->GetVkImageView() : VK_NULL_HANDLE );
		bChanged |= VK_CopyAndReturnNotEqual( pVkDescriptorImageInfo->imageLayout, pTexture ? pTexture->GetSyncState().vkImageLayout : VK_IMAGE_LAYOUT_UNDEFINED );

		// Remember pointer to the texture
		studioAPITextureSamplerPairVk_t&		textureSamplerPair = pStudioAPITexturesSamplers[studioAPITextureSamplerIndex];
		textureSamplerPair.pStudioAPITexture	= pTexture;
		textureSamplerPair.pStudioAPISampler	= NULL;
		return bChanged;
	}

	/**
	 * @brief Write descriptor image info
	 * @param slot			Slot index
	 * @param pSampler		StudioAPI sampler
	 * @return Return TRUE if the descriptor image info has been changed, otherwise FALSE
	 */
	template<VkDescriptorType vkDescriptorType>
	FORCEINLINE bool WriteDescriptorImageInfo( uint32 slot, CStudioAPISamplerVk* pSampler )
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
		const VkWriteDescriptorSet*		pVkWriteDescriptorSet			= &pVkWriteDescriptorSets[writeDescriptorIndex];
		VkDescriptorImageInfo*			pVkDescriptorImageInfo			= ( VkDescriptorImageInfo* )pVkWriteDescriptorSet->pImageInfo;
		uint32							studioAPITextureSamplerIndex	= ( uint32 )( pVkDescriptorImageInfo - pVkDescriptorImageInfos );
		AssertMsg( pVkWriteDescriptorSet->descriptorType == vkDescriptorType, "vkDescriptorType mismatch at index %i", writeDescriptorIndex );
		Assert( pVkDescriptorImageInfo );
		Assert( studioAPITextureSamplerIndex < numImageInfos );

		bool	bChanged = VK_CopyAndReturnNotEqual( pVkDescriptorImageInfo->sampler, pSampler ? pSampler->GetVkSampler() : VK_NULL_HANDLE );
		bChanged |= VK_CopyAndReturnNotEqual<VkImageView>( pVkDescriptorImageInfo->imageView, VK_NULL_HANDLE );
		bChanged |= VK_CopyAndReturnNotEqual( pVkDescriptorImageInfo->imageLayout, VK_IMAGE_LAYOUT_UNDEFINED );
		
		// Remember pointer to the sampler
		studioAPITextureSamplerPairVk_t&		textureSamplerPair = pStudioAPITexturesSamplers[studioAPITextureSamplerIndex];
		textureSamplerPair.pStudioAPITexture	= NULL;
		textureSamplerPair.pStudioAPISampler	= pSampler;
		return bChanged;
	}

	uint32								numWriteDescriptorSets;			/**< Write descriptor set number */
	uint32								numSlots;						/**< Slots in pSlotToWriteDescriptorSetMap number */
	uint32								numBufferInfos;					/**< Vulkan descriptor buffer info number */
	uint32								numImageInfos;					/**< Vulkan descriptor image info number */
	VkWriteDescriptorSet*				pVkWriteDescriptorSets;			/**< A view into someone else's Vulkan descriptor set writes */
	VkDescriptorBufferInfo*				pVkDescriptorBufferInfos;		/**< A view into someone else's Vulkan descriptor buffer infos */
	VkDescriptorImageInfo*				pVkDescriptorImageInfos;		/**< A view into someone else's Vulkan descriptor image infos */
	TRefPtr<CStudioAPIBufferVk>*		pStudioAPIBuffers;				/**< A view into someone else's StudioAPI buffers */
	studioAPITextureSamplerPairVk_t*	pStudioAPITexturesSamplers;		/**< A view into someone else's StudioAPI textures and samplers */
	uint8*								pSlotToWriteDescriptorSetMap;	/**< A view into someone else's slot to write descriptor set map */
};

#endif // !VK_STUDIOAPI_DESCRIPTORSETWRITER_H