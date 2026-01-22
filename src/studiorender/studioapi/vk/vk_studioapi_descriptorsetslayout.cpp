#include "pch_studioapi.h"
#include "studiorender/studioapi/vk/vk_studioapi_descriptorsetslayout.h"

/*
==================
CStudioAPIDescriptorSetsLayoutVk::CStudioAPIDescriptorSetsLayoutVk
==================
*/
CStudioAPIDescriptorSetsLayoutVk::CStudioAPIDescriptorSetsLayoutVk()
	: numDescriptorSets( 0 )
	, descriptorTypesUsageHash( 0 )
	, vkPipelineLayout( VK_NULL_HANDLE )
{
	// Add expected descriptor types
	for ( uint32 index = STUDIOAPI_VK_DESCRIPTOR_TYPE_BEGIN_RANGE; index <= STUDIOAPI_VK_DESCRIPTOR_TYPE_END_RANGE; ++index )
	{
		numUsedDescriptorTypesDict[(VkDescriptorType)index] = 0;
	}

	// Clear some fields
	Mem_Memzero( &vkDescriptorSetAllocateInfo, sizeof( VkDescriptorSetAllocateInfo ) );
}

/*
==================
CStudioAPIDescriptorSetsLayoutVk::~CStudioAPIDescriptorSetsLayoutVk
==================
*/
CStudioAPIDescriptorSetsLayoutVk::~CStudioAPIDescriptorSetsLayoutVk()
{
	Destroy();
}

/*
==================
CStudioAPIDescriptorSetsLayoutVk::Init
==================
*/
void CStudioAPIDescriptorSetsLayoutVk::Init( const studioAPIDescriptorSetLayoutVkDict_t& descriptorSetLayoutDict, const eastl::vector<VkPushConstantRange>& vkPushConstantRanges )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Check if we obey limits
	for ( auto it = descriptorSetLayoutDict.begin(), itEnd = descriptorSetLayoutDict.end(); it != itEnd; ++it )
	{
		const studioAPIDescriptorSetLayoutVk_t& descriptorSetLayout = it->second;
		for ( uint32 index = 0, numDescriptorSetLayouts = (uint32)descriptorSetLayout.vkLayoutBindings.size(); index < numDescriptorSetLayouts; ++index )
		{
			++numUsedDescriptorTypesDict[descriptorSetLayout.vkLayoutBindings[index].descriptorType];
		}
	}

	// Check for maxDescriptorSetSamplers
	const VkPhysicalDeviceLimits& vkPhysicalDeviceLimits = g_StudioAPIVk.GetDevice().GetVkDeviceLimits();
	Assert( numUsedDescriptorTypesDict[VK_DESCRIPTOR_TYPE_SAMPLER]
				+ numUsedDescriptorTypesDict[VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER]
			<= vkPhysicalDeviceLimits.maxDescriptorSetSamplers );

	// Check for maxDescriptorSetUniformBuffers
	Assert( numUsedDescriptorTypesDict[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER]
				+ numUsedDescriptorTypesDict[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC]
			<= vkPhysicalDeviceLimits.maxDescriptorSetUniformBuffers );

	// Check for maxDescriptorSetUniformBuffersDynamic
	Assert( g_StudioAPIVk.GetInfo().gpuVendorId == STUDIOAPI_GPU_VENDOR_ID_AMD || numUsedDescriptorTypesDict[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC] <= vkPhysicalDeviceLimits.maxDescriptorSetUniformBuffersDynamic );

	// Check for maxDescriptorSetStorageBuffers
	Assert( numUsedDescriptorTypesDict[VK_DESCRIPTOR_TYPE_STORAGE_BUFFER]
				+ numUsedDescriptorTypesDict[VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC]
			<= vkPhysicalDeviceLimits.maxDescriptorSetStorageBuffers );

	// Check for maxDescriptorSetStorageBuffersDynamic
	Assert( numUsedDescriptorTypesDict[VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC]
			<= vkPhysicalDeviceLimits.maxDescriptorSetStorageBuffersDynamic );

	// Check for maxDescriptorSetSampledImages
	Assert( numUsedDescriptorTypesDict[VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER]
				+ numUsedDescriptorTypesDict[VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE]
				+ numUsedDescriptorTypesDict[VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER]
			<= vkPhysicalDeviceLimits.maxDescriptorSetSampledImages );

	// Check for maxDescriptorSetStorageImages
	Assert( numUsedDescriptorTypesDict[VK_DESCRIPTOR_TYPE_STORAGE_IMAGE]
				+ numUsedDescriptorTypesDict[VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER]
			<= vkPhysicalDeviceLimits.maxDescriptorSetStorageImages );

	// Check for maxDescriptorSetInputAttachments
	Assert( numUsedDescriptorTypesDict[VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT]
			<= vkPhysicalDeviceLimits.maxDescriptorSetInputAttachments );

	// Create descriptor set layouts
	for ( auto it = descriptorSetLayoutDict.begin(), itEnd = descriptorSetLayoutDict.end(); it != itEnd; ++it )
	{
		// Create a descriptor set layout
		VkDescriptorSetLayout*			pVkDescriptorSetLayout			= &vkDescriptorSetLayouts.emplace_back();
		VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo = {};
		vkDescriptorSetLayoutCreateInfo.sType							= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		vkDescriptorSetLayoutCreateInfo.bindingCount					= (uint32)it->second.vkLayoutBindings.size();
		vkDescriptorSetLayoutCreateInfo.pBindings						= it->second.vkLayoutBindings.data();
		STUDIOAPI_VK_VERIFY_RESULT( vkCreateDescriptorSetLayout( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkDescriptorSetLayoutCreateInfo, NULL, pVkDescriptorSetLayout ) );

		// Copy descriptor set info
		if ( it->first >= (uint32)descriptorSetInfos.size() )
		{
			descriptorSetInfos.resize( it->first + 1 );
		}
		descriptorSetInfos[it->first] = it->second.descriptorSetInfo;
	}

	// Create a pipeline layout that is used to generate rendering pipelines that are based on this descriptor set layout
	VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo = {};
	vkPipelineLayoutCreateInfo.sType					  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	vkPipelineLayoutCreateInfo.setLayoutCount			  = (uint32)vkDescriptorSetLayouts.size();
	vkPipelineLayoutCreateInfo.pSetLayouts				  = vkDescriptorSetLayouts.data();
	vkPipelineLayoutCreateInfo.pushConstantRangeCount	  = (uint32)vkPushConstantRanges.size();
	vkPipelineLayoutCreateInfo.pPushConstantRanges		  = vkPushConstantRanges.data();
	STUDIOAPI_VK_VERIFY_RESULT( vkCreatePipelineLayout( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkPipelineLayoutCreateInfo, NULL, &vkPipelineLayout ) );

	// Initialize the descriptor set allocate information
	Mem_Memzero( &vkDescriptorSetAllocateInfo, sizeof( VkDescriptorSetAllocateInfo ) );
	vkDescriptorSetAllocateInfo.sType			   = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	vkDescriptorSetAllocateInfo.descriptorSetCount = (uint32)vkDescriptorSetLayouts.size();
	vkDescriptorSetAllocateInfo.pSetLayouts		   = vkDescriptorSetLayouts.data();

	// Calculate a descriptor types usage ID and save number of descriptor sets
	for ( auto it = numUsedDescriptorTypesDict.begin(), itEnd = numUsedDescriptorTypesDict.end(); it != itEnd; ++it )
	{
		descriptorTypesUsageHash = FastHash( it->second, descriptorTypesUsageHash );
	}
	numDescriptorSets = (uint32)descriptorSetLayoutDict.size();
}

/*
==================
CStudioAPIDescriptorSetsLayoutVk::Destroy
==================
*/
void CStudioAPIDescriptorSetsLayoutVk::Destroy()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Broadcast about delete the layout
	onDescriptorSetsLayoutDeleted.Broadcast( this );

	// Free Vulkan resources
	g_StudioAPIVk.GetMemoryMgr().FreeResource( [vkPipelineLayout = vkPipelineLayout, vkDescriptorSetLayouts = vkDescriptorSetLayouts]() {
		// Destroy the pipeline layout
		if ( vkPipelineLayout != VK_NULL_HANDLE )
		{
			vkDestroyPipelineLayout( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkPipelineLayout, NULL );
		}

		// Destroy descriptor set layouts
		for ( uint32 index = 0, count = (uint32)vkDescriptorSetLayouts.size(); index < count; ++index )
		{
			vkDestroyDescriptorSetLayout( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkDescriptorSetLayouts[index], NULL );
		}
	} );

	// Reset expected descriptor types
	for ( uint32 index = STUDIOAPI_VK_DESCRIPTOR_TYPE_BEGIN_RANGE; index <= STUDIOAPI_VK_DESCRIPTOR_TYPE_END_RANGE; ++index )
	{
		numUsedDescriptorTypesDict[(VkDescriptorType)index] = 0;
	}

	// Clear all descriptor set infos and layouts
	descriptorSetInfos.clear();
	vkDescriptorSetLayouts.clear();

	// Clear fields
	vkPipelineLayout		 = VK_NULL_HANDLE;
	numDescriptorSets		 = 0;
	descriptorTypesUsageHash = 0;
	Mem_Memzero( &vkDescriptorSetAllocateInfo, sizeof( VkDescriptorSetAllocateInfo ) );
}