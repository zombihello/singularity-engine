#include "pch_studioapi_vk.h"
#include "tier1/hashing/fasthash.h"
#include "tier1/streamdata_memory.h"
#include "utils/shadercache/shaderreflection.h"
#include "studiorender/studioapi_vk/vk_studioapi_shader.h"

/*
==================
VK_TranslateVertexInputRate
==================
*/
static FORCEINLINE VkVertexInputRate VK_TranslateVertexInputRate( studioAPIVertexInputRate_t vertexInputRate )
{
	switch ( vertexInputRate )
	{
	case STUDIOAPI_VERTEX_INPUT_RATE_VERTEX: return VK_VERTEX_INPUT_RATE_VERTEX;
	case STUDIOAPI_VERTEX_INPUT_RATE_INSTANCE: return VK_VERTEX_INPUT_RATE_INSTANCE;
	default:
		AssertMsg( false, "Unknown StudioAPI vertex input rate (0x%X)", vertexInputRate );
		return VK_VERTEX_INPUT_RATE_VERTEX;
	};
}

/*
==================
VK_TranslateVertexAttributeType
==================
*/
static FORCEINLINE VkFormat VK_TranslateVertexAttributeType( studioAPIVertexAttributeType_t vertexAttributeType )
{
	switch ( vertexAttributeType )
	{
	case STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT1: return VK_FORMAT_R32_SFLOAT;
	case STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT2: return VK_FORMAT_R32G32_SFLOAT;
	case STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT3: return VK_FORMAT_R32G32B32_SFLOAT;
	case STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4: return VK_FORMAT_R32G32B32A32_SFLOAT;
	case STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_BYTE4U: return VK_FORMAT_R8G8B8A8_UINT;
	case STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_BYTE4UN: return VK_FORMAT_R8G8B8A8_UNORM;
	case STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_NONE: return VK_FORMAT_UNDEFINED;
	default:
		AssertMsg( false, "Unknown StudioAPI vertex attribute type (0x%X)", vertexAttributeType );
		return VK_FORMAT_UNDEFINED;
	};
}

/*
==================
VK_ShaderReflectionCalcHash
==================
*/
static FORCEINLINE hash VK_ShaderReflectionCalcHash( const shaderReflectionConstantBuffer_t& shaderReflectionConstnatBuffer )
{
	hash hash = FastHash( shaderReflectionConstnatBuffer.name.c_str(), (uint64)shaderReflectionConstnatBuffer.name.size() * sizeof( char ) );
	return FastHash( shaderReflectionConstnatBuffer.size, hash );
}

/*
==================
VK_ShaderReflectionCalcHash
==================
*/
static FORCEINLINE hash VK_ShaderReflectionCalcHash( const shaderReflectionStorageBuffer_t& shaderReflectionStorageBuffer )
{
	hash hash = FastHash( shaderReflectionStorageBuffer.name.c_str(), (uint64)shaderReflectionStorageBuffer.name.size() * sizeof( char ) );
	return FastHash( shaderReflectionStorageBuffer.size, hash );
}

/*
==================
VK_ShaderReflectionCalcHash
==================
*/
static FORCEINLINE hash VK_ShaderReflectionCalcHash( const shaderReflectionImageSampler_t& shaderReflectionImageSampler )
{
	hash hash = FastHash( shaderReflectionImageSampler.name.c_str(), (uint64)shaderReflectionImageSampler.name.size() * sizeof( char ) );
	hash		= FastHash( shaderReflectionImageSampler.dimensionType, hash );
	return FastHash( shaderReflectionImageSampler.arraySize, hash );
}

/*
==================
VK_InitDescriptorSlotInfo
==================
*/
static void VK_InitDescriptorSlotInfo( VkDescriptorType vkDescriptorType, uint8 slot, studioAPIDescriptorSetInfoVk_t& descriptorSetInfo )
{
	// Add Vulkan descriptor type into an array
	AssertMsg( (uint32)descriptorSetInfo.vkDescriptorTypes.size() < 255, "Need more bits for studioAPIDescriptorSetInfoVk_t::slotToDescriptorTypeIndexMap (currently 8)! Requires %i descriptor slots in a set!", (uint32)descriptorSetInfo.vkDescriptorTypes.size() );
	uint8 descriptorTypeIndex = (uint8)descriptorSetInfo.vkDescriptorTypes.size();
	descriptorSetInfo.vkDescriptorTypes.emplace_back( vkDescriptorType );
	switch ( vkDescriptorType )
	{
	case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
	case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
		++descriptorSetInfo.numBufferInfos;
		break;

	case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
	case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
	case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
	case VK_DESCRIPTOR_TYPE_SAMPLER:
		++descriptorSetInfo.numImageInfos;
		break;

	default:
		Sys_Error( "Unknown Vulkan descriptor type 0x%X", vkDescriptorType );
		break;
	}

	// Initialize map to convert slot index to descriptor type index in studioAPIDescriptorSetInfoVk_t::vkDescriptorTypes
	if ( slot >= (uint32)descriptorSetInfo.slotToDescriptorTypeIndexMap.size() )
	{
		uint32 oldNumSlots = (uint32)descriptorSetInfo.slotToDescriptorTypeIndexMap.size();
		uint32 newNumSlots = slot + 1;
		descriptorSetInfo.slotToDescriptorTypeIndexMap.resize( newNumSlots );
		for ( uint32 index = oldNumSlots; index < newNumSlots; ++index )
		{
			descriptorSetInfo.slotToDescriptorTypeIndexMap[index] = -1;
		}
	}
	descriptorSetInfo.slotToDescriptorTypeIndexMap[slot] = descriptorTypeIndex;
}

/*
==================
CStudioAPIVertexDeclarationVk::CStudioAPIVertexDeclarationVk
==================
*/
CStudioAPIVertexDeclarationVk::CStudioAPIVertexDeclarationVk( const studioAPIVertexInputBuffer_t* pVertexInputBuffers, uint32 numVertexInputBuffers, const studioAPIVertexInputAttribute_t* pVertexInputAttributes, uint32 numVertexInputAttributes )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Check if we fit within the restrictions
	Assert( numVertexInputBuffers <= STUDIOAPI_VK_MAX_VERTEX_ELEMENT_COUNT );
	Assert( numVertexInputAttributes <= STUDIOAPI_VK_MAX_VERTEX_ELEMENT_COUNT );

	// Initialize vertex input bindings
	vkVertexInputBindingDescriptions.resize( numVertexInputBuffers );
	for ( uint32 index = 0; index < numVertexInputBuffers; ++index )
	{
		const studioAPIVertexInputBuffer_t& studioAPIVertexInputBuffer		= pVertexInputBuffers[index];
		VkVertexInputBindingDescription&	vkVertexInputBindingDescription = vkVertexInputBindingDescriptions[index];
		vkVertexInputBindingDescription.binding								= studioAPIVertexInputBuffer.slot;
		vkVertexInputBindingDescription.stride								= studioAPIVertexInputBuffer.stride;
		vkVertexInputBindingDescription.inputRate							= VK_TranslateVertexInputRate( studioAPIVertexInputBuffer.inputRate );
	}

	// Initialize vertex input attributes
	vkVertexInputAttributeDescriptions.resize( numVertexInputAttributes );
	for ( uint32 index = 0; index < numVertexInputAttributes; ++index )
	{
		const studioAPIVertexInputAttribute_t& studioAPIVertexInputAttribute	 = pVertexInputAttributes[index];
		VkVertexInputAttributeDescription&	   vkVertexInputAttributeDescription = vkVertexInputAttributeDescriptions[index];
		vkVertexInputAttributeDescription.location								 = studioAPIVertexInputAttribute.attributeIndex;
		vkVertexInputAttributeDescription.binding								 = studioAPIVertexInputAttribute.bufferSlot;
		vkVertexInputAttributeDescription.format								 = VK_TranslateVertexAttributeType( (studioAPIVertexAttributeType_t)studioAPIVertexInputAttribute.type );
		vkVertexInputAttributeDescription.offset								 = studioAPIVertexInputAttribute.offset;
		Assert( vkVertexInputAttributeDescription.location < 255 );
	}

	// Initialize pipeline vertex input state info
	Mem_Memzero( &vkPipelineVertexInputStateInfo, sizeof( VkPipelineVertexInputStateCreateInfo ) );
	vkPipelineVertexInputStateInfo.sType						   = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vkPipelineVertexInputStateInfo.vertexBindingDescriptionCount   = (uint32)vkVertexInputBindingDescriptions.size();
	vkPipelineVertexInputStateInfo.pVertexBindingDescriptions	   = vkVertexInputBindingDescriptions.data();
	vkPipelineVertexInputStateInfo.vertexAttributeDescriptionCount = (uint32)vkVertexInputAttributeDescriptions.size();
	vkPipelineVertexInputStateInfo.pVertexAttributeDescriptions	   = vkVertexInputAttributeDescriptions.data();
}

/*
==================
CStudioAPIShaderVk::CStudioAPIShaderVk
==================
*/
CStudioAPIShaderVk::CStudioAPIShaderVk( studioAPIShaderType_t type, const char* pEntryPointName, const byte* pBytecode, uint64 bytecodeSize, const byte* pReflectionData, uint64 reflectionDataSize, const char* pDebugName /* = "" */ )
	: type( type )
	, vkShaderModule( VK_NULL_HANDLE )
	, pStudioAPIVkShutdownDelegate( NULL )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Check on align to 4 bytes
	if ( bytecodeSize % 4 != 0 || ( (uptrint)pBytecode & 3 ) != 0 )
	{
		Sys_Error( "Shader bytecode size (%u) or pointer to bytecode (0x%p) is not aligned to 4 bytes as required by SPIRV/Vulkan spec", bytecodeSize, pBytecode );
	}

	// Create Vulkan shader module
	VkShaderModuleCreateInfo vkShaderModuleCreateInfo = {};
	vkShaderModuleCreateInfo.sType					  = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vkShaderModuleCreateInfo.pCode					  = (uint32*)pBytecode;
	vkShaderModuleCreateInfo.codeSize				  = bytecodeSize;
	STUDIOAPI_VK_VERIFY_RESULT( vkCreateShaderModule( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkShaderModuleCreateInfo, NULL, &vkShaderModule ) );

	// Initialize pipline shader stage info
	Mem_Memzero( &vkPipelineShaderStageInfo, sizeof( VkPipelineShaderStageCreateInfo ) );
	entryPointName					 = pEntryPointName;
	vkPipelineShaderStageInfo.sType	 = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vkPipelineShaderStageInfo.module = vkShaderModule;
	vkPipelineShaderStageInfo.pName	 = entryPointName.c_str();
	switch ( type )
	{
	case STUDIOAPI_SHADER_TYPE_VERTEX: vkPipelineShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; break;
	case STUDIOAPI_SHADER_TYPE_HULL: vkPipelineShaderStageInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT; break;
	case STUDIOAPI_SHADER_TYPE_DOMAIN: vkPipelineShaderStageInfo.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT; break;
	case STUDIOAPI_SHADER_TYPE_GEOMETRY: vkPipelineShaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT; break;
	case STUDIOAPI_SHADER_TYPE_PIXEL: vkPipelineShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
	case STUDIOAPI_SHADER_TYPE_COMPUTE: vkPipelineShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT; break;
	default:
		Sys_Error( "StudioAPIVk: Unknown shader type 0x%X in CStudioAPIShaderVk::Create", type );
		return;
	}

	// Deserialize shader reflection
	CStreamDataMemoryReader memoryReader( pReflectionData, reflectionDataSize );
	CShaderReflection		reflectionData;
	reflectionData.Deserialize( &memoryReader );

	// Parse descriptor set layouts
	const eastl::vector<shaderReflectionDescriptorSet_t>& reflectionDescriptorSets = reflectionData.GetDescriptorSets();
	for ( uint32 index = 0, numDescriptorSets = (uint32)reflectionDescriptorSets.size(); index < numDescriptorSets; ++index )
	{
		const shaderReflectionDescriptorSet_t& reflectionDescriptorSet = reflectionDescriptorSets[index];
		if ( reflectionDescriptorSet.IsEmpty() )
		{
			continue;
		}
		studioAPIDescriptorSetLayoutVk_t& descriptorSetLayout = descriptorSetLayoutDict[index];

		// Constant buffers
		for ( auto it = reflectionDescriptorSet.constantBuffersDict.begin(), itEnd = reflectionDescriptorSet.constantBuffersDict.end(); it != itEnd; ++it )
		{
			// Initialize descriptor set layout binding
			VkDescriptorSetLayoutBinding& vkDescriptorSetLayoutBinding = descriptorSetLayout.vkLayoutBindings.emplace_back();
			vkDescriptorSetLayoutBinding							   = {};
			vkDescriptorSetLayoutBinding.descriptorType				   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			vkDescriptorSetLayoutBinding.descriptorCount			   = 1;
			vkDescriptorSetLayoutBinding.stageFlags					   = VK_SHADER_STAGE_ALL;
			vkDescriptorSetLayoutBinding.pImmutableSamplers			   = NULL;
			vkDescriptorSetLayoutBinding.binding					   = it->first;

			// Initialize descriptor slot info and validation hash
			VK_InitDescriptorSlotInfo( vkDescriptorSetLayoutBinding.descriptorType, vkDescriptorSetLayoutBinding.binding, descriptorSetLayout.descriptorSetInfo );
#if !RETAIL
			descriptorSetLayout.descriptorSlotValidationHashDict.insert( eastl::make_pair( vkDescriptorSetLayoutBinding.binding, VK_ShaderReflectionCalcHash( it->second ) ) );
#endif	// !RETAIL
		}

		// Storage buffers
		for ( auto it = reflectionDescriptorSet.storageBuffersDict.begin(), itEnd = reflectionDescriptorSet.storageBuffersDict.end(); it != itEnd; ++it )
		{
			// Initialize descriptor set layout binding
			VkDescriptorSetLayoutBinding& vkDescriptorSetLayoutBinding = descriptorSetLayout.vkLayoutBindings.emplace_back();
			vkDescriptorSetLayoutBinding							   = {};
			vkDescriptorSetLayoutBinding.descriptorType				   = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			vkDescriptorSetLayoutBinding.descriptorCount			   = 1;
			vkDescriptorSetLayoutBinding.stageFlags					   = VK_SHADER_STAGE_ALL;
			vkDescriptorSetLayoutBinding.pImmutableSamplers			   = NULL;
			vkDescriptorSetLayoutBinding.binding					   = it->first;

			// Initialize descriptor slot info and validation hash
			VK_InitDescriptorSlotInfo( vkDescriptorSetLayoutBinding.descriptorType, vkDescriptorSetLayoutBinding.binding, descriptorSetLayout.descriptorSetInfo );
#if !RETAIL
			descriptorSetLayout.descriptorSlotValidationHashDict.insert( eastl::make_pair( vkDescriptorSetLayoutBinding.binding, VK_ShaderReflectionCalcHash( it->second ) ) );
#endif	// !RETAIL
		}

		// Sampled images
		for ( auto it = reflectionDescriptorSet.sampledImagesDict.begin(), itEnd = reflectionDescriptorSet.sampledImagesDict.end(); it != itEnd; ++it )
		{
			// Initialize descriptor set layout binding
			VkDescriptorSetLayoutBinding& vkDescriptorSetLayoutBinding = descriptorSetLayout.vkLayoutBindings.emplace_back();
			vkDescriptorSetLayoutBinding							   = {};
			vkDescriptorSetLayoutBinding.descriptorType				   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			vkDescriptorSetLayoutBinding.descriptorCount			   = it->second.arraySize;
			vkDescriptorSetLayoutBinding.stageFlags					   = VK_SHADER_STAGE_ALL;
			vkDescriptorSetLayoutBinding.pImmutableSamplers			   = NULL;
			vkDescriptorSetLayoutBinding.binding					   = it->first;

			// Initialize descriptor slot info and validation hash
			VK_InitDescriptorSlotInfo( vkDescriptorSetLayoutBinding.descriptorType, vkDescriptorSetLayoutBinding.binding, descriptorSetLayout.descriptorSetInfo );
#if !RETAIL
			descriptorSetLayout.descriptorSlotValidationHashDict.insert( eastl::make_pair( vkDescriptorSetLayoutBinding.binding, VK_ShaderReflectionCalcHash( it->second ) ) );
#endif	// !RETAIL
		}

		// Storage images
		for ( auto it = reflectionDescriptorSet.storageImagesDict.begin(), itEnd = reflectionDescriptorSet.storageImagesDict.end(); it != itEnd; ++it )
		{
			// Initialize descriptor set layout binding
			VkDescriptorSetLayoutBinding& vkDescriptorSetLayoutBinding = descriptorSetLayout.vkLayoutBindings.emplace_back();
			vkDescriptorSetLayoutBinding							   = {};
			vkDescriptorSetLayoutBinding.descriptorType				   = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			vkDescriptorSetLayoutBinding.descriptorCount			   = it->second.arraySize;
			vkDescriptorSetLayoutBinding.stageFlags					   = VK_SHADER_STAGE_ALL;
			vkDescriptorSetLayoutBinding.pImmutableSamplers			   = NULL;
			vkDescriptorSetLayoutBinding.binding					   = it->first;

			// Initialize descriptor slot info and validation hash
			VK_InitDescriptorSlotInfo( vkDescriptorSetLayoutBinding.descriptorType, vkDescriptorSetLayoutBinding.binding, descriptorSetLayout.descriptorSetInfo );
#if !RETAIL
			descriptorSetLayout.descriptorSlotValidationHashDict.insert( eastl::make_pair( vkDescriptorSetLayoutBinding.binding, VK_ShaderReflectionCalcHash( it->second ) ) );
#endif	// !RETAIL
		}

		// Separate textures
		for ( auto it = reflectionDescriptorSet.separateTexturesDict.begin(), itEnd = reflectionDescriptorSet.separateTexturesDict.end(); it != itEnd; ++it )
		{
			// Initialize descriptor set layout binding
			VkDescriptorSetLayoutBinding& vkDescriptorSetLayoutBinding = descriptorSetLayout.vkLayoutBindings.emplace_back();
			vkDescriptorSetLayoutBinding							   = {};
			vkDescriptorSetLayoutBinding.descriptorType				   = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			vkDescriptorSetLayoutBinding.descriptorCount			   = it->second.arraySize;
			vkDescriptorSetLayoutBinding.stageFlags					   = VK_SHADER_STAGE_ALL;
			vkDescriptorSetLayoutBinding.pImmutableSamplers			   = NULL;
			vkDescriptorSetLayoutBinding.binding					   = it->first;

			// Initialize descriptor slot info and validation hash
			VK_InitDescriptorSlotInfo( vkDescriptorSetLayoutBinding.descriptorType, vkDescriptorSetLayoutBinding.binding, descriptorSetLayout.descriptorSetInfo );
#if !RETAIL
			descriptorSetLayout.descriptorSlotValidationHashDict.insert( eastl::make_pair( vkDescriptorSetLayoutBinding.binding, VK_ShaderReflectionCalcHash( it->second ) ) );
#endif	// !RETAIL
		}

		// Separate samplers
		for ( auto it = reflectionDescriptorSet.separateSamplersDict.begin(), itEnd = reflectionDescriptorSet.separateSamplersDict.end(); it != itEnd; ++it )
		{
			// Initialize descriptor set layout binding
			VkDescriptorSetLayoutBinding& vkDescriptorSetLayoutBinding = descriptorSetLayout.vkLayoutBindings.emplace_back();
			vkDescriptorSetLayoutBinding							   = {};
			vkDescriptorSetLayoutBinding.descriptorType				   = VK_DESCRIPTOR_TYPE_SAMPLER;
			vkDescriptorSetLayoutBinding.descriptorCount			   = it->second.arraySize;
			vkDescriptorSetLayoutBinding.stageFlags					   = VK_SHADER_STAGE_ALL;
			vkDescriptorSetLayoutBinding.pImmutableSamplers			   = NULL;
			vkDescriptorSetLayoutBinding.binding					   = it->first;

			// Initialize descriptor slot info and validation hash
			VK_InitDescriptorSlotInfo( vkDescriptorSetLayoutBinding.descriptorType, vkDescriptorSetLayoutBinding.binding, descriptorSetLayout.descriptorSetInfo );
#if !RETAIL
			descriptorSetLayout.descriptorSlotValidationHashDict.insert( eastl::make_pair( vkDescriptorSetLayoutBinding.binding, VK_ShaderReflectionCalcHash( it->second ) ) );
#endif	// !RETAIL
		}
	}

	// Parse push constant ranges
	const eastl::vector<shaderReflectionPushConstantRange_t>& reflectionPushConstantRanges = reflectionData.GetPushConstantRanges();
	vkPushConstantRanges.resize( reflectionPushConstantRanges.size() );
	for ( uint32 index = 0, count = (uint32)reflectionPushConstantRanges.size(); index < count; ++index )
	{
		const shaderReflectionPushConstantRange_t& reflectionPushConstantRange = reflectionPushConstantRanges[index];
		VkPushConstantRange&					   vkPushConstantRange		   = vkPushConstantRanges[index];
		vkPushConstantRange.stageFlags										   = vkPipelineShaderStageInfo.stage;
		vkPushConstantRange.size											   = (uint32)reflectionPushConstantRange.size;
		vkPushConstantRange.offset											   = (uint32)reflectionPushConstantRange.offset;
	}

	// Register in 'onStudioAPIVkShutodwn' for destroy Vulkan objects when the one is shutdown
	pStudioAPIVkShutdownDelegate = g_StudioAPIVk.OnStudioAPIVkShutdown().AddFunc( &CStudioAPIShaderVk::OnStudioAPIVkShutdown, this );
}

/*
==================
CStudioAPIShaderVk::~CStudioAPIShaderVk
==================
*/
CStudioAPIShaderVk::~CStudioAPIShaderVk()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Destroy the shader module
	if ( vkShaderModule != VK_NULL_HANDLE )
	{
		g_StudioAPIVk.GetMemoryMgr().FreeResource( [vkShaderModule = vkShaderModule]()
												   { vkDestroyShaderModule( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkShaderModule, NULL ); } );
		vkShaderModule = VK_NULL_HANDLE;
	}

	// Clear fields
	descriptorSetLayoutDict.clear();
	vkPushConstantRanges.clear();

	// Remove CStudioAPIShaderVk::OnStudioAPIVkShutdown from event 'onStudioAPIVkShutodwn'
	if ( pStudioAPIVkShutdownDelegate )
	{
		g_StudioAPIVk.OnStudioAPIVkShutdown().RemoveFunc( pStudioAPIVkShutdownDelegate );
		pStudioAPIVkShutdownDelegate = NULL;
	}
}

/*
==================
CStudioAPIShaderVk::GetType
==================
*/
studioAPIShaderType_t CStudioAPIShaderVk::GetType() const
{
	return type;
}

/*
==================
CStudioAPIShaderVk::OnStudioAPIVkShutdown
==================
*/
void CStudioAPIShaderVk::OnStudioAPIVkShutdown( void* pUserData )
{
	Assert( pUserData );
	CStudioAPIShaderVk* pStudioAPIShader		   = (CStudioAPIShaderVk*)pUserData;
	pStudioAPIShader->pStudioAPIVkShutdownDelegate = NULL;
	pStudioAPIShader->~CStudioAPIShaderVk();
}

/*
==================
CStudioAPIBoundShaderStateVk::CStudioAPIBoundShaderStateVk
==================
*/
CStudioAPIBoundShaderStateVk::CStudioAPIBoundShaderStateVk( const CStudioAPIBoundShaderStateKeyVk& key, CStudioAPIVertexDeclarationVk* pVertexDeclaration, CStudioAPIVertexShaderVk* pVertexShader, CStudioAPIPixelShaderVk* pPixelShader, CStudioAPIHullShaderVk* pHullShader /* = NULL */, CStudioAPIDomainShaderVk* pDomainShader /* = NULL */, CStudioAPIGeometryShaderVk* pGeometryShader /* = NULL */ )
	: key( key )
	, pVertexDeclaration( pVertexDeclaration )
	, pVertexShader( pVertexShader )
	, pPixelShader( pPixelShader )
	, pHullShader( pHullShader )
	, pDomainShader( pDomainShader )
	, pGeometryShader( pGeometryShader )
	, pStudioAPIVkShutdownDelegate( NULL )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Grab descriptor set layouts and push constant ranges
	studioAPIDescriptorSetLayoutVkDict_t descriptorSetLayoutDict;
	eastl::vector<VkPushConstantRange>	 vkPushConstantRanges;
	CStudioAPIShaderVk*					 pShaders[] = { pVertexShader, pHullShader, pDomainShader, pGeometryShader, pPixelShader };
	for ( uint32 shaderIdx = 0; shaderIdx < ARRAYSIZE( pShaders ); ++shaderIdx )
	{
		CStudioAPIShaderVk* pShader = pShaders[shaderIdx];
		if ( !pShader )
		{
			continue;
		}

		// Grab descriptor set layouts
		const studioAPIDescriptorSetLayoutVkDict_t& shaderDescriptorSetLayoutDict = pShaders[shaderIdx]->GetDescriptorSetLayoutDict();
		for ( auto it = shaderDescriptorSetLayoutDict.begin(), itEnd = shaderDescriptorSetLayoutDict.end(); it != itEnd; ++it )
		{
			const studioAPIDescriptorSetLayoutVk_t& srcDescriptorSetLayout	   = it->second;
			studioAPIDescriptorSetLayoutVk_t&		destDescriptorSetLayout	   = descriptorSetLayoutDict[it->first];
			uint32									srcNumVkLayoutBindings	   = (uint32)srcDescriptorSetLayout.vkLayoutBindings.size();
			uint32									oldDestNumVkLayoutBindings = (uint32)destDescriptorSetLayout.vkLayoutBindings.size();

			// Copy Vulkan layout bindings
			destDescriptorSetLayout.vkLayoutBindings.resize( oldDestNumVkLayoutBindings + srcNumVkLayoutBindings );
			Mem_Memcpy( destDescriptorSetLayout.vkLayoutBindings.data() + oldDestNumVkLayoutBindings, srcDescriptorSetLayout.vkLayoutBindings.data(), srcNumVkLayoutBindings * sizeof( VkDescriptorSetLayoutBinding ) );

			// Copy descriptor set info
			for ( uint32 index = 0, count = (uint32)it->second.descriptorSetInfo.slotToDescriptorTypeIndexMap.size(); index < count; ++index )
			{
				uint8 descriptorTypeIndex = it->second.descriptorSetInfo.slotToDescriptorTypeIndexMap[index];
				if ( descriptorTypeIndex != (uint8)-1 )
				{
#if !RETAIL
					// Validate descriptor slot
					auto itSrcFind	= it->second.descriptorSlotValidationHashDict.find( index );
					auto itDestFind = destDescriptorSetLayout.descriptorSlotValidationHashDict.find( index );
					Assert( itSrcFind != it->second.descriptorSlotValidationHashDict.end() );

					// Remember a hash if we haven't it
					if ( itDestFind == destDescriptorSetLayout.descriptorSlotValidationHashDict.end() )
					{
						destDescriptorSetLayout.descriptorSlotValidationHashDict[index] = itSrcFind->second;
					}
					// Otherwise if we already have the slot and they hash aren't same it is error
					else if ( itDestFind->second != itSrcFind->second )
					{
						Sys_Error( "StudioAPIVk: Descriptor slot index (%i) is already present, CStudioAPIBoundShaderStateVk won't be created", index );
					}
#endif	// !RETAIL

					// Copy descriptor slot info
					VK_InitDescriptorSlotInfo( it->second.descriptorSetInfo.vkDescriptorTypes[descriptorTypeIndex], index, destDescriptorSetLayout.descriptorSetInfo );
				}
			}
		}

		// Grab push constant ranges
		const eastl::vector<VkPushConstantRange>& shaderVkPushConstantRanges = pShaders[shaderIdx]->GetVkPushConstantRanges();
		if ( !shaderVkPushConstantRanges.empty() )
		{
			uint32 srcNumVkPushConstantRanges	  = (uint32)shaderVkPushConstantRanges.size();
			uint32 oldDestNumVkPushConstantRanges = (uint32)vkPushConstantRanges.size();
			vkPushConstantRanges.resize( oldDestNumVkPushConstantRanges + srcNumVkPushConstantRanges );
			Mem_Memcpy( vkPushConstantRanges.data() + oldDestNumVkPushConstantRanges, shaderVkPushConstantRanges.data(), srcNumVkPushConstantRanges * sizeof( VkPushConstantRange ) );
		}
	}

	// Initialize the descriptor sets layout
	descriptorSetsLayout.Init( descriptorSetLayoutDict, vkPushConstantRanges );

	// Register in 'onStudioAPIVkShutodwn' for destroy Vulkan objects when the one is shutdown
	pStudioAPIVkShutdownDelegate = g_StudioAPIVk.OnStudioAPIVkShutdown().AddFunc( &CStudioAPIBoundShaderStateVk::OnStudioAPIVkShutdown, this );
}

/*
==================
CStudioAPIBoundShaderStateVk::~CStudioAPIBoundShaderStateVk
==================
*/
CStudioAPIBoundShaderStateVk::~CStudioAPIBoundShaderStateVk()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Destroy the descriptor sets layout
	descriptorSetsLayout.Destroy();

	// Remove the state from the cache
	g_StudioAPIVk.GetBoundShaderStateCache().Remove( key );

	// Remove CStudioAPIBoundShaderStateVk::OnStudioAPIVkShutdown from event 'onStudioAPIVkShutodwn'
	if ( pStudioAPIVkShutdownDelegate )
	{
		g_StudioAPIVk.OnStudioAPIVkShutdown().RemoveFunc( pStudioAPIVkShutdownDelegate );
		pStudioAPIVkShutdownDelegate = NULL;
	}
}

/*
==================
CStudioAPIBoundShaderStateVk::GetVertexDeclaration
==================
*/
IStudioAPIVertexDeclaration* CStudioAPIBoundShaderStateVk::GetVertexDeclaration() const
{
	return pVertexDeclaration;
}

/*
==================
CStudioAPIBoundShaderStateVk::GetVertexShader
==================
*/
IStudioAPIShader* CStudioAPIBoundShaderStateVk::GetVertexShader() const
{
	return pVertexShader;
}

/*
==================
CStudioAPIBoundShaderStateVk::GetPixelShader
==================
*/
IStudioAPIShader* CStudioAPIBoundShaderStateVk::GetPixelShader() const
{
	return pPixelShader;
}

/*
==================
CStudioAPIBoundShaderStateVk::GetHullShader
==================
*/
IStudioAPIShader* CStudioAPIBoundShaderStateVk::GetHullShader() const
{
	return pHullShader;
}

/*
==================
CStudioAPIBoundShaderStateVk::GetDomainShader
==================
*/
IStudioAPIShader* CStudioAPIBoundShaderStateVk::GetDomainShader() const
{
	return pDomainShader;
}

/*
==================
CStudioAPIBoundShaderStateVk::GetGeometryShader
==================
*/
IStudioAPIShader* CStudioAPIBoundShaderStateVk::GetGeometryShader() const
{
	return pGeometryShader;
}

/*
==================
CStudioAPIBoundShaderStateVk::OnStudioAPIVkShutdown
==================
*/
void CStudioAPIBoundShaderStateVk::OnStudioAPIVkShutdown( void* pUserData )
{
	Assert( pUserData );
	CStudioAPIBoundShaderStateVk* pStudioAPIBoundShaderState = (CStudioAPIBoundShaderStateVk*)pUserData;
	pStudioAPIBoundShaderState->pStudioAPIVkShutdownDelegate = NULL;
	pStudioAPIBoundShaderState->~CStudioAPIBoundShaderStateVk();
}
