/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_PENDINGDESCRIPTORSTATE_H
#define VK_STUDIOAPI_PENDINGDESCRIPTORSTATE_H

#include <vector>

#include "studiorender/studioapi/vk/vk_studioapi_renderpipeline.h"
#include "studiorender/studioapi/vk/vk_studioapi_cmdbuffer.h"
#include "studiorender/studioapi/vk/vk_studioapi_shader.h"
#include "studiorender/studioapi/vk/vk_studioapi_descriptorsetwriter.h"

// Forward declarations
class CStudioAPICmdContextVk;
class CStudioAPICmdListVk;
class CStudioAPIBufferVk;
class CStudioAPICmdListVk;

/**
 * @ingroup studioapi_vk
 * @brief Common Vulkan descriptor state
 */
class CStudioAPIDescriptorStateCommonVk
{
public:
	/**
	 * @brief Constructor
	 * @param cmdContext	Command context
	 */
	CStudioAPIDescriptorStateCommonVk( CStudioAPICmdContextVk& cmdContext );

	/**
	 * @brief Set a constant buffer
	 * @param pCmdList				StudioAPI command list
	 * @param set					Set index
	 * @param slot					Slot index in the set
	 * @param pConstantBuffer		StudioAPI constant buffer
	 */
	void SetConstantBuffer( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPIBufferVk* pConstantBuffer );

	/**
	 * @brief Set a texture
	 * @param pCmdList		StudioAPI command list
	 * @param set			Set index
	 * @param slot			Slot index in the set
	 * @param pTexture		Texture
	 */
	void SetTexture( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPITextureVk* pTexture );

	/**
	 * @brief Set a sampler
	 * @param pCmdList		StudioAPI command list
	 * @param set			Set index
	 * @param slot			Slot index in the set
	 * @param pSampler		Sampler
	 */
	void SetSampler( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPISamplerVk* pSampler );

	/**
	 * @brief Reset the descriptor state
	 */
	FORCEINLINE void Reset()
	{
		PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
		Mem_Memzero( descriptorSetWriteContainer.vkDescriptorBufferInfos.data(), descriptorSetWriteContainer.vkDescriptorBufferInfos.size() * sizeof( VkDescriptorBufferInfo ) );
		Mem_Memzero( descriptorSetWriteContainer.vkDescriptorImageInfos.data(), descriptorSetWriteContainer.vkDescriptorImageInfos.size() * sizeof( VkDescriptorImageInfo ) );
		for ( uint32 index = 0, count = ( uint32 )descriptorSetWriteContainer.studioAPIBuffers.size(); index < count; ++index )
		{
			descriptorSetWriteContainer.studioAPIBuffers[index] = NULL;
		}
		bDirtyDescriptorSets = false;
	}

protected:
	/**
	 * @brief Initialize descriptor write infos
	 * @note Before use pDescriptorSetsLayout must be valid
	 */
	void InitDescriptorWriteInfos();

	bool											bDirtyDescriptorSets;			/**< Is dirty descriptor sets */
	CStudioAPICmdContextVk&							cmdContext;						/**< Command context */
	const CStudioAPIDescriptorSetsLayoutVk*			pDescriptorSetsLayout;			/**< Descriptor sets layout */
	studioAPIDescriptorSetWriteContainerVk_t		descriptorSetWriteContainer;	/**< Descriptor set write container */
	std::vector<CStudioAPIDescriptorSetWriterVk>	descriptorSetWriters;			/**< Descriptor set writers */
	std::vector<VkDescriptorSet>					vkDescriptorSets;				/**< Vulkan descriptor sets */
};

/**
 * @ingroup studioapi_vk
 * @brief Vulkan descriptor state for compute pipeline
 */
class CStudioAPIDescriptorStateComputeVk : public CStudioAPIDescriptorStateCommonVk
{
public:
	/**
	 * @brief Constructor
	 * @param cmdContext	Command context
	 */
	CStudioAPIDescriptorStateComputeVk( CStudioAPICmdContextVk& cmdContext );
};

/**
 * @ingroup studioapi_vk
 * @brief Vulkan descriptor state for render pipeline
 */
class CStudioAPIDescriptorStateRenderVk : public CStudioAPIDescriptorStateCommonVk
{
public:
	/**
	 * @brief Constructor
	 * @param cmdContext			Command context
	 * @param pRenderPipeline		Render pipeline for which the state will be created
	 */
	CStudioAPIDescriptorStateRenderVk( CStudioAPICmdContextVk& cmdContext, CStudioAPIRenderPipelineVk* pRenderPipeline );

	/**
	 * @brief Reset the descriptor state
	 */
	FORCEINLINE void Reset()
	{
		CStudioAPIDescriptorStateCommonVk::Reset();
	}

	/**
	 * @brief Update descriptor sets
	 * @param pCmdList		Command list
	 * @return Return TRUE if the state has descriptor sets, otherwise FALSE
	 */
	bool UpdateDescriptorSets( CStudioAPICmdListVk* pCmdList );

	/**
	 * @brief Bind descriptor sets
	 * @param pCmdList		Command list
	 */
	FORCEINLINE void BindDescriptorSets( CStudioAPICmdListVk* pCmdList )
	{
		PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
		if ( !vkDescriptorSets.empty() )
		{
			CStudioAPIBoundShaderStateVk*		pBoundShaderState = pRenderPipeline->GetBoundShaderState();
			vkCmdBindDescriptorSets( pCmdList->GetCmdBuffer()->GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pBoundShaderState->GetDescriptorSetsLayout().GetVkPipelineLayout(), 0, ( uint32 )vkDescriptorSets.size(), vkDescriptorSets.data(), 0, NULL );
		}
	}

	/**
	 * @brief Get the render pipeline
	 * @return Return the render pipeline for which the state has been created
	 */
	FORCEINLINE CStudioAPIRenderPipelineVk* GetRenderPipeline() const
	{
		return pRenderPipeline;
	}

private:
	CStudioAPIRenderPipelineVk*		pRenderPipeline;	/**< Render pipeline for which the state has been created */
};

#endif // !VK_STUDIOAPI_PENDINGDESCRIPTORSTATE_H