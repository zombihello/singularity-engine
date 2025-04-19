#ifndef VK_STUDIOAPI_PENDINGDESCRIPTORSTATE_INL
#define VK_STUDIOAPI_PENDINGDESCRIPTORSTATE_INL

/*
==================
CStudioAPIDescriptorStateCommonVk::Reset
==================
*/
FORCEINLINE void CStudioAPIDescriptorStateCommonVk::Reset()
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


/*
==================
CStudioAPIDescriptorStateRenderVk::Reset
==================
*/
FORCEINLINE void CStudioAPIDescriptorStateRenderVk::Reset()
{
	CStudioAPIDescriptorStateCommonVk::Reset();
}

/*
==================
CStudioAPIDescriptorStateRenderVk::BindDescriptorSets
==================
*/
FORCEINLINE void CStudioAPIDescriptorStateRenderVk::BindDescriptorSets( CStudioAPICmdListVk* pCmdList )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	if ( !vkDescriptorSets.empty() )
	{
		CStudioAPIBoundShaderStateVk* pBoundShaderState = pRenderPipeline->GetBoundShaderState();
		vkCmdBindDescriptorSets( pCmdList->GetCmdBuffer()->GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pBoundShaderState->GetDescriptorSetsLayout().GetVkPipelineLayout(), 0, ( uint32 )vkDescriptorSets.size(), vkDescriptorSets.data(), 0, NULL );
	}
}

/*
==================
CStudioAPIDescriptorStateRenderVk::GetRenderPipeline
==================
*/
FORCEINLINE CStudioAPIRenderPipelineVk* CStudioAPIDescriptorStateRenderVk::GetRenderPipeline() const
{
	return pRenderPipeline;
}

#endif // !VK_STUDIOAPI_PENDINGDESCRIPTORSTATE_INL
