#pragma once
#include "Volk/volk.h"

#include "studiorender/studioapi/vk/vk_studioapi_descriptorsetslayout.h"
#include "studiorender/studioapi/vk/vk_studioapi_buffer.h"
#include "studiorender/studioapi/vk/vk_studioapi_texture.h"

//-----------------------------------------------------------------------------
// Descriptor set containers
//-----------------------------------------------------------------------------
struct studioAPITextureSamplerPairVk_t
{
	TRefPtr<CStudioAPITextureVk> pStudioAPITexture;
	TRefPtr<CStudioAPISamplerVk> pStudioAPISampler;
};

struct studioAPIDescriptorSetWriteContainerVk_t
{
	eastl::vector<VkWriteDescriptorSet>			 vkWriteDescriptorSets;
	eastl::vector<VkDescriptorBufferInfo>			 vkDescriptorBufferInfos;
	eastl::vector<VkDescriptorImageInfo>			 vkDescriptorImageInfos;
	eastl::vector<TRefPtr<CStudioAPIBufferVk>>	 studioAPIBuffers;
	eastl::vector<studioAPITextureSamplerPairVk_t> studioAPITexturesSamplers;
	eastl::vector<uint8>							 slotToWriteDescriptorSetMap;
};

//-----------------------------------------------------------------------------
// This class encapsulates updating VkWriteDescriptorSet structures but doesn't own them.
// It is intended to be used to access a sub-region of a long array of VkWriteDescriptorSet (ie studioAPIDescriptorSetWriteContainerVk_t)
//-----------------------------------------------------------------------------
class CStudioAPIDescriptorSetWriterVk
{
public:
	CStudioAPIDescriptorSetWriterVk();

	void SetupDescriptorSetWrites( const studioAPIDescriptorSetInfoVk_t& descriptorSetInfo, VkWriteDescriptorSet* pVkWriteDescriptorSets, VkDescriptorBufferInfo* pVkDescriptorBufferInfos, VkDescriptorImageInfo* pVkDescriptorImageInfos, TRefPtr<CStudioAPIBufferVk>* pStudioAPIBuffers, studioAPITextureSamplerPairVk_t* pStudioAPITexturesSamplers, uint8* pSlotToWriteDescriptorSetMap );

	bool WriteConstantBuffer( uint32 slot, CStudioAPIBufferVk* pConstantBuffer, uint64 offset, uint64 size );
	bool WriteTexture( uint32 slot, CStudioAPITextureVk* pTexture );
	bool WriteSampler( uint32 slot, CStudioAPISamplerVk* pSampler );

	void SetVkDescriptorSet( VkDescriptorSet vkDescriptorSet );
	bool IsEmpty() const;

private:
	template<VkDescriptorType vkDescriptorType>
	bool WriteDescriptorBufferInfo( uint32 slot, CStudioAPIBufferVk* pBuffer, uint64 offset, uint64 size );

	template<VkDescriptorType vkDescriptorType>
	bool WriteDescriptorImageInfo( uint32 slot, CStudioAPITextureVk* pTexture );

	template<VkDescriptorType vkDescriptorType>
	bool WriteDescriptorImageInfo( uint32 slot, CStudioAPISamplerVk* pSampler );

	uint32							 numWriteDescriptorSets;
	uint32							 numSlots;	// Slots in pSlotToWriteDescriptorSetMap number
	uint32							 numBufferInfos;
	uint32							 numImageInfos;
	VkWriteDescriptorSet*			 pVkWriteDescriptorSets;		// A view into someone else's Vulkan descriptor set writes
	VkDescriptorBufferInfo*			 pVkDescriptorBufferInfos;		// A view into someone else's Vulkan descriptor buffer infos
	VkDescriptorImageInfo*			 pVkDescriptorImageInfos;		// A view into someone else's Vulkan descriptor image infos
	TRefPtr<CStudioAPIBufferVk>*	 pStudioAPIBuffers;				// A view into someone else's StudioAPI buffers
	studioAPITextureSamplerPairVk_t* pStudioAPITexturesSamplers;	// A view into someone else's StudioAPI textures and samplers
	uint8*							 pSlotToWriteDescriptorSetMap;	// A view into someone else's slot to write descriptor set map
};

#include "studiorender/studioapi/vk/vk_studioapi_descriptorsetwriter.inl"