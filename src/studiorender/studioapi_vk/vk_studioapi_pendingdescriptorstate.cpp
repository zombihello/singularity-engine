#include "pch_studioapi_vk.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdcontext.h"
#include "studiorender/studioapi_vk/vk_studioapi_shader.h"
#include "studiorender/studioapi_vk/vk_studioapi_pendingdescriptorstate.h"

/*
==================
CStudioAPIDescriptorStateCommonVk::CStudioAPIDescriptorStateCommonVk
==================
*/
CStudioAPIDescriptorStateCommonVk::CStudioAPIDescriptorStateCommonVk( CStudioAPICmdContextVk& cmdContext )
	: bDirtyDescriptorSets( false )
	, cmdContext( cmdContext )
	, pDescriptorSetsLayout( NULL )
{
}

/*
==================
CStudioAPIDescriptorStateCommonVk::SetConstantBuffer
==================
*/
void CStudioAPIDescriptorStateCommonVk::SetConstantBuffer( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPIBufferVk* pConstantBuffer )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Do nothing if the set isn't valid
	if ( set >= (uint32)descriptorSetWriters.size() )
	{
		return;
	}

	if ( descriptorSetWriters[set].WriteConstantBuffer( slot, pConstantBuffer, 0, 0 ) )
	{
		// Update a synchronization state of the buffer
		if ( pConstantBuffer )
		{
			pConstantBuffer->UpdateSyncState( VK_ACCESS_UNIFORM_READ_BIT,
											  VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
											  cmdContext.GetQueue().GetQueueFamilyIndex() );
		}
		bDirtyDescriptorSets = true;
	}
}

/*
==================
CStudioAPIDescriptorStateCommonVk::SetTexture
==================
*/
void CStudioAPIDescriptorStateCommonVk::SetTexture( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPITextureVk* pTexture )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Do nothing if the set isn't valid
	if ( set >= (uint32)descriptorSetWriters.size() )
	{
		return;
	}

	if ( descriptorSetWriters[set].WriteTexture( slot, pTexture ) )
	{
		// Update a synchronization state of the texture
		if ( pTexture )
		{
			pTexture->UpdateSyncState( CStudioAPIImageSubresourceRangeVk::CreateForAllSubresources(),
									   pTexture->GetSyncState().vkImageLayout,
									   VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
									   cmdContext.GetQueue().GetQueueFamilyIndex() );
		}
		bDirtyDescriptorSets = true;
	}
}

/*
==================
CStudioAPIDescriptorStateCommonVk::SetSampler
==================
*/
void CStudioAPIDescriptorStateCommonVk::SetSampler( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPISamplerVk* pSampler )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Do nothing if the set isn't valid
	if ( set >= (uint32)descriptorSetWriters.size() )
	{
		return;
	}

	if ( descriptorSetWriters[set].WriteSampler( slot, pSampler ) )
	{
		bDirtyDescriptorSets = true;
	}
}

/*
==================
CStudioAPIDescriptorStateCommonVk::InitDescriptorWriteInfos
==================
*/
void CStudioAPIDescriptorStateCommonVk::InitDescriptorWriteInfos()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Initialize descriptor set write container
	Assert( pDescriptorSetsLayout );
	const eastl::vector<studioAPIDescriptorSetInfoVk_t>& descriptorSetInfos = pDescriptorSetsLayout->GetDescriptorSetInfos();
	uint32											   numDescriptorSets  = (uint32)descriptorSetInfos.size();
	for ( uint32 set = 0; set < numDescriptorSets; ++set )
	{
		const studioAPIDescriptorSetInfoVk_t& descriptorSetInfo = descriptorSetInfos[set];
		descriptorSetWriteContainer.vkWriteDescriptorSets.resize( descriptorSetWriteContainer.vkWriteDescriptorSets.size() + descriptorSetInfo.vkDescriptorTypes.size() );
		descriptorSetWriteContainer.vkDescriptorBufferInfos.resize( descriptorSetWriteContainer.vkDescriptorBufferInfos.size() + descriptorSetInfo.numBufferInfos );
		descriptorSetWriteContainer.vkDescriptorImageInfos.resize( descriptorSetWriteContainer.vkDescriptorImageInfos.size() + descriptorSetInfo.numImageInfos );
		descriptorSetWriteContainer.studioAPIBuffers.resize( descriptorSetWriteContainer.studioAPIBuffers.size() + descriptorSetInfo.numBufferInfos );
		descriptorSetWriteContainer.studioAPITexturesSamplers.resize( descriptorSetWriteContainer.studioAPITexturesSamplers.size() + descriptorSetInfo.numImageInfos );
		descriptorSetWriteContainer.slotToWriteDescriptorSetMap.resize( descriptorSetWriteContainer.slotToWriteDescriptorSetMap.size() + descriptorSetInfo.slotToDescriptorTypeIndexMap.size() );
	}
	Mem_Memset( descriptorSetWriteContainer.slotToWriteDescriptorSetMap.data(), (uint8)-1, (uint32)descriptorSetWriteContainer.slotToWriteDescriptorSetMap.size() * sizeof( uint8 ) );

	// Reserve mamory for Vulkan descriptor sets
	vkDescriptorSets.resize( pDescriptorSetsLayout->GetNumDescriptorSets() );

	// Setup descriptor set writers
	descriptorSetWriters.resize( numDescriptorSets );
	VkWriteDescriptorSet*			 pCurrentVkWriteDescriptorSet		 = descriptorSetWriteContainer.vkWriteDescriptorSets.data();
	VkDescriptorBufferInfo*			 pCurrentVkDescriptorBufferInfo		 = descriptorSetWriteContainer.vkDescriptorBufferInfos.data();
	VkDescriptorImageInfo*			 pCurrentVkDescriptorImageInfo		 = descriptorSetWriteContainer.vkDescriptorImageInfos.data();
	TRefPtr<CStudioAPIBufferVk>*	 pCurrentStudioAPIBuffer			 = descriptorSetWriteContainer.studioAPIBuffers.data();
	studioAPITextureSamplerPairVk_t* pCurrentStudioAPITexturesSamplers	 = descriptorSetWriteContainer.studioAPITexturesSamplers.data();
	uint8*							 pCurrentSlotToWriteDescriptorSetMap = descriptorSetWriteContainer.slotToWriteDescriptorSetMap.data();
	for ( uint32 set = 0; set < numDescriptorSets; ++set )
	{
		// Skip empty descriptor set info
		const studioAPIDescriptorSetInfoVk_t& descriptorSetInfo = descriptorSetInfos[set];
		if ( descriptorSetInfo.vkDescriptorTypes.empty() )
		{
			continue;
		}

		// Setup descriptor set writers
		descriptorSetWriters[set].SetupDescriptorSetWrites( descriptorSetInfo,
															pCurrentVkWriteDescriptorSet,
															pCurrentVkDescriptorBufferInfo,
															pCurrentVkDescriptorImageInfo,
															pCurrentStudioAPIBuffer,
															pCurrentStudioAPITexturesSamplers,
															pCurrentSlotToWriteDescriptorSetMap );
		pCurrentVkWriteDescriptorSet += descriptorSetInfo.vkDescriptorTypes.size();
		pCurrentVkDescriptorBufferInfo += descriptorSetInfo.numBufferInfos;
		pCurrentVkDescriptorImageInfo += descriptorSetInfo.numImageInfos;
		pCurrentStudioAPIBuffer += descriptorSetInfo.numBufferInfos;
		pCurrentStudioAPITexturesSamplers += descriptorSetInfo.numImageInfos;
		pCurrentSlotToWriteDescriptorSetMap += descriptorSetInfo.slotToDescriptorTypeIndexMap.size();
	}
}

/*
==================
CStudioAPIDescriptorStateComputeVk::CStudioAPIDescriptorStateComputeVk
==================
*/
CStudioAPIDescriptorStateComputeVk::CStudioAPIDescriptorStateComputeVk( CStudioAPICmdContextVk& cmdContext )
	: CStudioAPIDescriptorStateCommonVk( cmdContext )
{
	InitDescriptorWriteInfos();
}

/*
==================
CStudioAPIDescriptorStateRenderVk::CStudioAPIDescriptorStateRenderVk
==================
*/
CStudioAPIDescriptorStateRenderVk::CStudioAPIDescriptorStateRenderVk( CStudioAPICmdContextVk& cmdContext, CStudioAPIRenderPipelineVk* pRenderPipeline )
	: CStudioAPIDescriptorStateCommonVk( cmdContext )
	, pRenderPipeline( pRenderPipeline )
{
	// Initialize descriptor write infos
	CStudioAPIBoundShaderStateVk* pBoundShaderState = pRenderPipeline->GetBoundShaderState();
	pDescriptorSetsLayout							= &pBoundShaderState->GetDescriptorSetsLayout();
	InitDescriptorWriteInfos();
}

/*
==================
CStudioAPIDescriptorStateRenderVk::UpdateDescriptorSets
==================
*/
bool CStudioAPIDescriptorStateRenderVk::UpdateDescriptorSets( CStudioAPICmdListVk* pCmdList )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Allocated a new descriptor sets
	CStudioAPIBoundShaderStateVk* pBoundShaderState = pRenderPipeline->GetBoundShaderState();
	if ( !pBoundShaderState )
	{
		return false;
	}

	// Update descriptor sets
	if ( pCmdList->GetCmdBuffer()->AcquirePoolSetAndDescriptorsIfNeed( *pDescriptorSetsLayout, bDirtyDescriptorSets, vkDescriptorSets.data() ) )
	{
		for ( uint32 index = 0, numSets = (uint32)vkDescriptorSets.size(); index < numSets; ++index )
		{
			CStudioAPIDescriptorSetWriterVk& descriptorSetWriter = descriptorSetWriters[index];
			if ( !descriptorSetWriter.IsEmpty() )
			{
				descriptorSetWriters[index].SetVkDescriptorSet( vkDescriptorSets[index] );
			}
		}

		vkUpdateDescriptorSets( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), (uint32)descriptorSetWriteContainer.vkWriteDescriptorSets.size(), descriptorSetWriteContainer.vkWriteDescriptorSets.data(), 0, NULL );
		return true;
	}
	return false;
}