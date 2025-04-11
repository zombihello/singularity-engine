/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_DESCRIPTORSETSLAYOUT_H
#define VK_STUDIOAPI_DESCRIPTORSETSLAYOUT_H

#include <vector>
#include <unordered_map>

#include "stdlib/hashing/fasthash.h"
#include "studiorender/studioapi/vk/vk_studioapi_device.h"
#include "studiorender/studioapi/vk/vk_studioapi_delegates.h"

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI descriptor set info
 */
struct studioAPIDescriptorSetInfoVk_t
{
	/**
	 * @brief Constructor
	 */
	studioAPIDescriptorSetInfoVk_t()
		: numBufferInfos( 0 )
		, numImageInfos( 0 )
	{}

	uint16								numBufferInfos;					/**< Descriptor buffer infos number */
	uint16								numImageInfos;					/**< Descriptor image infos number */
	std::vector<VkDescriptorType>		vkDescriptorTypes;				/**< Vulkan descriptor types */
	std::vector<uint8>					slotToDescriptorTypeIndexMap;	/**< Map to convert slot index to descriptor type index */
};

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI descriptor set layout
 */
struct studioAPIDescriptorSetLayoutVk_t
{
	std::vector<VkDescriptorSetLayoutBinding>	vkLayoutBindings;					/**< Vulkan layout bindings */
	studioAPIDescriptorSetInfoVk_t				descriptorSetInfo;					/**< Descriptor set info */
#if !RETAIL
	std::unordered_map<uint8, hash_t>			descriptorSlotValidationHashDict;	/**< Descriptor slot validation hash dictionary */
#endif // !RETAIL
};

/**
 * @ingroup studioapi_vk
 * @brief Vulkan studio api descriptor set layout dictionary
 */
typedef std::unordered_map<uint32, studioAPIDescriptorSetLayoutVk_t>	studioAPIDescriptorSetLayoutVkDict_t;

/**
 * @ingroup studioapi_vk
 * @brief Vulkan studio api descriptor set layouts
 */
class CStudioAPIDescriptorSetsLayoutVk
{
public:
	/**
	 * @brief Constructor
	 */
	CStudioAPIDescriptorSetsLayoutVk();

	/**
	 * @brief Destructor
	 */
	~CStudioAPIDescriptorSetsLayoutVk();

	/**
	 * @brief Initialize the descriptor set layouts
	 * @param descriptorSetLayoutDict		Descriptor set layout dictionary
	 * @param vkPushConstantRanges			Vulkan push constant ranges
	 */
	void Init( const studioAPIDescriptorSetLayoutVkDict_t& descriptorSetLayoutDict, const std::vector<VkPushConstantRange>& vkPushConstantRanges );

	/**
	 * @brief Destroy the descriptor set layouts
	 */
	void Destroy();

	/**
	 * @brief Has input attachments
	 * @return Return TRUE if the descriptor set layouts has input attachments, otherwise FALSE 
	 */
	FORCEINLINE bool HasInputAttachments() const
	{
		return GetNumUsedDescriptorTypes( VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT ) > 0;
	}

	/**
	 * @brief Get the Vulkan descriptor set layouts
	 * @return Return the Vulkan descriptor set layouts. Return empty array if the set wasn't initialized
	 */
	FORCEINLINE const std::vector<VkDescriptorSetLayout>& GetVkDescriptorSetLayouts() const
	{
		return vkDescriptorSetLayouts;
	}

	/**
	 * @brief Get number of used descriptor types
	 * @param vkDescriptorType		Vulkan descriptor type
	 * @return Return number of used descriptor types
	 */
	FORCEINLINE uint32 GetNumUsedDescriptorTypes( VkDescriptorType vkDescriptorType ) const
	{
		PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
		auto	it = numUsedDescriptorTypesDict.find( vkDescriptorType );
		if ( it != numUsedDescriptorTypesDict.end() )
		{
			return it->second;
		}
		return 0;
	}

	/**
	 * @brief Get number of descriptor sets
	 * @return Return number of descriptor sets
	 */
	FORCEINLINE uint32 GetNumDescriptorSets() const
	{
		return numDescriptorSets;
	}

	/**
	 * @brief Get the descriptor types usage hash
	 * @return Return the descriptor types usage hash
	 */
	FORCEINLINE hash_t GetDescriptorTypesUsageHash() const
	{
		return descriptorTypesUsageHash;
	}

	/**
	 * @brief Get descriptor set infos
	 * @return Return descriptor set infos
	 */
	FORCEINLINE const std::vector<studioAPIDescriptorSetInfoVk_t>& GetDescriptorSetInfos() const
	{
		return descriptorSetInfos;
	}

	/**
	 * @brief Get the Vulkan pipeline layout
	 * @return Return the Vulkan pipeline layout
	 */
	FORCEINLINE VkPipelineLayout GetVkPipelineLayout() const
	{
		return vkPipelineLayout;
	}

	/**
	 * @brief Get the Vulkan descriptor set allocate information
	 * @return Return the Vulkan descriptor set allocate information
	 */
	FORCEINLINE VkDescriptorSetAllocateInfo GetVkAllocateInfo() const
	{
		return vkDescriptorSetAllocateInfo;
	}

	/**
	 * @brief Get event manager 'on the descriptor sets layout is deleted'
	 * @return Return event manager 'on the descriptor sets layout is deleted'
	 */
	FORCEINLINE COnStudioAPIDescriptorSetsLayoutDeletedVk& OnDescriptorSetsLayoutDeleted() const
	{
		return onDescriptorSetsLayoutDeleted;
	}

private:
	uint32													numDescriptorSets;				/**< Number of descriptor sets */
	hash_t													descriptorTypesUsageHash;		/**< Descriptor types usage hash */
	std::unordered_map<VkDescriptorType, uint32>			numUsedDescriptorTypesDict;		/**< Number of used descriptor types dictionary */
	std::vector<VkDescriptorSetLayout>						vkDescriptorSetLayouts;			/**< Vulkan descriptor set layouts */
	std::vector<studioAPIDescriptorSetInfoVk_t>				descriptorSetInfos;				/**< Descriptor set infos */
	VkPipelineLayout										vkPipelineLayout;				/**< Vulkan pipeline layout */
	VkDescriptorSetAllocateInfo								vkDescriptorSetAllocateInfo;	/**< Vulkan descriptor set allocate information */
	mutable COnStudioAPIDescriptorSetsLayoutDeletedVk		onDescriptorSetsLayoutDeleted;	/**< Event on the descriptor sets layout is deleted */
};

#endif // !VK_STUDIOAPI_DESCRIPTORSETSLAYOUT_H