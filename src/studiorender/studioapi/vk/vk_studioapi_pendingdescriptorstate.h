#pragma once
#include <vector>

#include "studiorender/studioapi/vk/vk_studioapi_renderpipeline.h"
#include "studiorender/studioapi/vk/vk_studioapi_cmdbuffer.h"
#include "studiorender/studioapi/vk/vk_studioapi_shader.h"
#include "studiorender/studioapi/vk/vk_studioapi_descriptorsetwriter.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPICmdContextVk;
class CStudioAPICmdListVk;
class CStudioAPIBufferVk;
class CStudioAPICmdListVk;


//-----------------------------------------------------------------------------
// Common Vulkan descriptor state
//-----------------------------------------------------------------------------
class CStudioAPIDescriptorStateCommonVk
{
public:
	CStudioAPIDescriptorStateCommonVk( CStudioAPICmdContextVk& cmdContext );

	void SetConstantBuffer( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPIBufferVk* pConstantBuffer );
	void SetTexture( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPITextureVk* pTexture );
	void SetSampler( CStudioAPICmdListVk* pCmdList, uint32 set, uint32 slot, CStudioAPISamplerVk* pSampler );

	void Reset();

protected:
	void InitDescriptorWriteInfos();

	bool											bDirtyDescriptorSets;
	CStudioAPICmdContextVk&							cmdContext;
	const CStudioAPIDescriptorSetsLayoutVk*			pDescriptorSetsLayout;
	studioAPIDescriptorSetWriteContainerVk_t		descriptorSetWriteContainer;
	std::vector<CStudioAPIDescriptorSetWriterVk>	descriptorSetWriters;
	std::vector<VkDescriptorSet>					vkDescriptorSets;
};


//-----------------------------------------------------------------------------
// Vulkan descriptor state for compute pipeline
//-----------------------------------------------------------------------------
class CStudioAPIDescriptorStateComputeVk : public CStudioAPIDescriptorStateCommonVk
{
public:
	CStudioAPIDescriptorStateComputeVk( CStudioAPICmdContextVk& cmdContext );
};


//-----------------------------------------------------------------------------
// Vulkan descriptor state for render pipeline
//-----------------------------------------------------------------------------
class CStudioAPIDescriptorStateRenderVk : public CStudioAPIDescriptorStateCommonVk
{
public:
	CStudioAPIDescriptorStateRenderVk( CStudioAPICmdContextVk& cmdContext, CStudioAPIRenderPipelineVk* pRenderPipeline );

	void Reset();
	bool UpdateDescriptorSets( CStudioAPICmdListVk* pCmdList );
	void BindDescriptorSets( CStudioAPICmdListVk* pCmdList );

	CStudioAPIRenderPipelineVk* GetRenderPipeline() const;

private:
	CStudioAPIRenderPipelineVk*		pRenderPipeline;
};

#include "studiorender/studioapi/vk/vk_studioapi_pendingdescriptorstate.inl"