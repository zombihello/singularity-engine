/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_DESCRIPTORPOOLSMGR_H
#define VK_STUDIOAPI_DESCRIPTORPOOLSMGR_H

#include <vector>
#include <list>

#include "Volk/volk.h"
#include "studiorender/studioapi/vk/vk_studioapi_delegates.h"
#include "studiorender/studioapi/vk/vk_studioapi_descriptorsetslayout.h"

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI descriptor pool
 */
class CStudioAPIDescriptorPoolVk
{
public:
	/**
	 * @brief Constructor
	 * @param descriptorSetsLayout	Descriptor sets layout for which will be created a pool
	 * @param maxSetsAllocations	Maximum descriptor sets in the pool
	 */
	CStudioAPIDescriptorPoolVk( const CStudioAPIDescriptorSetsLayoutVk& descriptorSetsLayout, uint32 maxSetsAllocations );

	/**
	 * @brief Destructor
	 */
	~CStudioAPIDescriptorPoolVk();

	/**
	 * @brief Reset the pool
	 */
	void Reset();

	/**
	 * @brief Allocate descriptor sets
	 * @param vkDescriptorSetAllocateInfo	Vulkan descriptor set allocate information
	 * @param pDestDescriptorSets			Destination descriptor sets
	 * @return Return TRUE if the descriptor sets has been allocated, otherwise FALSE
	 */
	bool AllocateDescriptorSets( const VkDescriptorSetAllocateInfo& vkDescriptorSetAllocateInfo, VkDescriptorSet* pDestDescriptorSets );

	/**
	 * @brief Get the Vulkan descriptor pool
	 * @return Return the Vulkan descriptor pool
	 */
	FORCEINLINE VkDescriptorPool GetVkDescriptorPool() const
	{
		return vkDescriptorPool;
	}

private:
	VkDescriptorPool	vkDescriptorPool;	/**< Vulkan descriptor pool */
};

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI typed descriptor pool set
 */
class CStudioAPITypedDescriptorPoolSetVk
{
public:
	/**
	 * @brief Constructor
	 * @param descriptorSetsLayout	Descriptor sets layout for which will be created a pool
	 */
	CStudioAPITypedDescriptorPoolSetVk( const CStudioAPIDescriptorSetsLayoutVk& descriptorSetsLayout );

	/**
	 * @brief Destructor
	 */
	~CStudioAPITypedDescriptorPoolSetVk();

	/**
	 * @brief Reset the pool set
	 */
	void Reset();

	/**
	 * @brief Allocate descriptor sets
	 * @param descriptorSetsLayout	Descriptor sets layout
	 * @param pDestDescriptorSets	Destination descriptor sets
	 * @return Return TRUE if the descriptor sets has been allocated, otherwise FALSE
	 */
	bool AllocateDescriptorSets( const CStudioAPIDescriptorSetsLayoutVk& descriptorSetsLayout, VkDescriptorSet* pDestDescriptorSets );

	/**
	 * @brief Get a descriptor set layout which correspond to the pool set
	 * @return Return a descriptor set layout which correspond to the pool set
	 */
	FORCEINLINE const CStudioAPIDescriptorSetsLayoutVk& GetDescriptorSetsLayout() const
	{
		return descriptorSetsLayout;
	}

private:
	/**
	 * @brief Get a free pool
	 * @param bForceNewPool		Is need force a new pool
	 * @return Return a free pool
	 */
	CStudioAPIDescriptorPoolVk* GetFreePool( bool bForceNewPool );

	/**
	 * @brief Push a new pool
	 * @return Return a new pool
	 */
	CStudioAPIDescriptorPoolVk* PushNewPool();

	const CStudioAPIDescriptorSetsLayoutVk&				descriptorSetsLayout;	/**< Descriptor sets layout for which has been created the pool set */
	std::list<CStudioAPIDescriptorPoolVk*>::iterator	poolCurrentIt;			/**< Current pool */
	std::list<CStudioAPIDescriptorPoolVk*>				poolList;				/**< Descriptor pools list */
};

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI descriptor pool set container
 */
class CStudioAPIDescriptorPoolSetContainerVk
{
public:
	/**
	 * @brief Constructor
	 */
	CStudioAPIDescriptorPoolSetContainerVk();

	/**
	 * @brief Destructor
	 */
	~CStudioAPIDescriptorPoolSetContainerVk();

	/**
	 * @brief Acquire a typed pool set
	 * @param descriptorSetsLayout	Descriptor sets layout for which will be created a typed pool set
	 */
	FORCEINLINE CStudioAPITypedDescriptorPoolSetVk* AcquireTypedPoolSet( const CStudioAPIDescriptorSetsLayoutVk& descriptorSetsLayout )
	{
		PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
		hash_t		hash = descriptorSetsLayout.GetDescriptorTypesUsageHash();
		auto		it = typedDescriptorPoolsDict.find( hash );
		if ( it == typedDescriptorPoolsDict.end() )
		{
			it = typedDescriptorPoolsDict.insert( std::make_pair( hash, new CStudioAPITypedDescriptorPoolSetVk( descriptorSetsLayout ) ) ).first;
		}
		return it->second;
	}

	/**
	 * @brief Reset the pool set
	 */
	FORCEINLINE void Reset()
	{
		for ( auto it = typedDescriptorPoolsDict.begin(), itEnd = typedDescriptorPoolsDict.end(); it != itEnd; ++it )
		{
			it->second->Reset();
		}
	}

	/**
	 * @brief Set used flag
	 * @param bUsed		Is the pool set used
	 */
	void SetUsed( bool bUsed );

	/**
	 * @brief Is the pool set used
	 * @return Return TRUE if the pool set is used, otherwise FALSE
	 */
	FORCEINLINE bool IsUsed() const
	{
		return bUsed;
	}

	/**
	 * @brief Get the last frame number when the container has been used
	 * @return Return the last frame number when the container has been used
	 */
	FORCEINLINE uint64 GetLastFrameUsed() const
	{
		return lastFrameUsed;
	}

private:
	bool																bUsed;						/**< Is the pool set used */
	uint64																lastFrameUsed;				/**< The last frame when the container has been used */
	std::unordered_map<hash_t, CStudioAPITypedDescriptorPoolSetVk*>		typedDescriptorPoolsDict;	/**< Typed descriptor pools dictionary */
};

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI descriptor pools manager
 */
class CStudioAPIDescriptorPoolsMgrVk
{
public:
	/**
	 * @brief Constructor
	 */
	CStudioAPIDescriptorPoolsMgrVk();

	/**
	 * @brief Destructor
	 */
	~CStudioAPIDescriptorPoolsMgrVk();

	/**
	 * @brief Initialize the manager
	 */
	void Init();

	/**
	 * @brief Shutdown the manager
	 */
	void Shutdown();

	/**
	 * @brief Acquire a pool set
	 * @return Return a pool set
	 */
	CStudioAPIDescriptorPoolSetContainerVk* AcquirePoolSet();

	/**
	 * @brief Release the pool set
	 * @param pPoolSet	The pool set to release
	 */
	void ReleasePoolSet( CStudioAPIDescriptorPoolSetContainerVk* pPoolSet );

	/**
	 * @brief Free unused pool sets
	 */
	void FreeUnusedPoolSets();

private:
	/**
	 * @brief Delegate called when StudioAPI Vulkan is shutdown
	 * @param pUserData		Pointer to CStudioAPIDescriptorPoolsMgrVk
	 */
	static void OnStudioAPIVkShutdown( void* pUserData );

	std::list<CStudioAPIDescriptorPoolSetContainerVk*>		poolSets;						/**< Pool sets */
	COnStudioAPIVkShutdown::funcDelegate_t*					pStudioAPIVkShutdownDelegate;	/**< Delegate handle 'on StudioAPI Vulkan shutdown' */
};

#endif // !VK_STUDIOAPI_DESCRIPTORPOOLSMGR_H