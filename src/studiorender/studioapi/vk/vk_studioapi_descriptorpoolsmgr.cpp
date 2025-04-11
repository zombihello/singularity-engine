#include "pch_studioapi.h"
#include "studiorender/studioapi/vk/vk_studioapi_descriptorpoolsmgr.h"

/*
==================
CStudioAPIDescriptorPoolVk::CStudioAPIDescriptorPoolVk
==================
*/
CStudioAPIDescriptorPoolVk::CStudioAPIDescriptorPoolVk( const CStudioAPIDescriptorSetsLayoutVk& descriptorSetsLayout, uint32 maxSetsAllocations )
	: vkDescriptorPool( VK_NULL_HANDLE )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// When we're hashing pools with types usage hash the descriptor pool can be used for different layouts so the initial layout does not make much sense
	std::vector<VkDescriptorPoolSize>		vkDescriptorPoolSizes;
	for ( uint32 index = STUDIOAPI_VK_DESCRIPTOR_TYPE_BEGIN_RANGE; index <= STUDIOAPI_VK_DESCRIPTOR_TYPE_END_RANGE; ++index )
	{
		VkDescriptorType		vkDescriptorType		= ( VkDescriptorType )index;
		uint32					numUsedDescriptorTypes	= descriptorSetsLayout.GetNumUsedDescriptorTypes( vkDescriptorType );
		if ( numUsedDescriptorTypes > 0 )
		{
			VkDescriptorPoolSize&					vkDescriptorPoolSize = vkDescriptorPoolSizes.emplace_back();
			Mem_Memzero( &vkDescriptorPoolSize, sizeof( VkDescriptorPoolSize ) );
			vkDescriptorPoolSize.type				= vkDescriptorType;
			vkDescriptorPoolSize.descriptorCount	= numUsedDescriptorTypes * maxSetsAllocations;
		}
	}

	// Create a descriptor pool
	VkDescriptorPoolCreateInfo					vkDescriptorPoolCreateInfo = {};
	vkDescriptorPoolCreateInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	vkDescriptorPoolCreateInfo.poolSizeCount	= ( uint32 )vkDescriptorPoolSizes.size();
	vkDescriptorPoolCreateInfo.pPoolSizes		= vkDescriptorPoolSizes.data();
	vkDescriptorPoolCreateInfo.maxSets			= maxSetsAllocations;
	STUDIOAPI_VK_VERIFY_RESULT( vkCreateDescriptorPool( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &vkDescriptorPoolCreateInfo, NULL, &vkDescriptorPool ) );
}

/*
==================
CStudioAPIDescriptorPoolVk::~CStudioAPIDescriptorPoolVk
==================
*/
CStudioAPIDescriptorPoolVk::~CStudioAPIDescriptorPoolVk()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	if ( vkDescriptorPool != VK_NULL_HANDLE )
	{
		vkDestroyDescriptorPool( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkDescriptorPool, NULL );
		vkDescriptorPool = VK_NULL_HANDLE;
	}
}

/*
==================
CStudioAPIDescriptorPoolVk::Reset
==================
*/
void CStudioAPIDescriptorPoolVk::Reset()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	if ( vkDescriptorPool != VK_NULL_HANDLE )
	{
		STUDIOAPI_VK_VERIFY_RESULT( vkResetDescriptorPool( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkDescriptorPool, 0 ) );
	}
}

/*
==================
CStudioAPIDescriptorPoolVk::AllocateDescriptorSets
==================
*/
bool CStudioAPIDescriptorPoolVk::AllocateDescriptorSets( const VkDescriptorSetAllocateInfo& vkDescriptorSetAllocateInfo, VkDescriptorSet* pDestDescriptorSets )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	if ( vkDescriptorPool != VK_NULL_HANDLE )
	{
		VkDescriptorSetAllocateInfo		tmpVkDescriptorSetAllocateInfo = vkDescriptorSetAllocateInfo;
		tmpVkDescriptorSetAllocateInfo.descriptorPool = vkDescriptorPool;
		return vkAllocateDescriptorSets( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), &tmpVkDescriptorSetAllocateInfo, pDestDescriptorSets ) == VK_SUCCESS;
	}
	return false;
}


/*
==================
CStudioAPITypedDescriptorPoolSetVk::CStudioAPITypedDescriptorPoolSetVk
==================
*/
CStudioAPITypedDescriptorPoolSetVk::CStudioAPITypedDescriptorPoolSetVk( const CStudioAPIDescriptorSetsLayoutVk& descriptorSetsLayout )
	: descriptorSetsLayout( descriptorSetsLayout )
{
	PushNewPool();
}

/*
==================
CStudioAPITypedDescriptorPoolSetVk::~CStudioAPITypedDescriptorPoolSetVk
==================
*/
CStudioAPITypedDescriptorPoolSetVk::~CStudioAPITypedDescriptorPoolSetVk()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	for ( auto it = poolList.begin(), itEnd = poolList.end(); it != itEnd; ++it )
	{
		delete *it;
	}
}

/*
==================
CStudioAPITypedDescriptorPoolSetVk::Reset
==================
*/
void CStudioAPITypedDescriptorPoolSetVk::Reset()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	for ( auto it = poolList.begin(), itEnd = poolList.end(); it != itEnd; ++it )
	{
		( *it )->Reset();
	}
	poolCurrentIt = poolList.begin();
}

/*
==================
CStudioAPITypedDescriptorPoolSetVk::AllocateDescriptorSets
==================
*/
bool CStudioAPITypedDescriptorPoolSetVk::AllocateDescriptorSets( const CStudioAPIDescriptorSetsLayoutVk& descriptorSetsLayout, VkDescriptorSet* pDestDescriptorSets )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	if ( !descriptorSetsLayout.GetVkDescriptorSetLayouts().empty() )
	{
		CStudioAPIDescriptorPoolVk*		pPool = *poolCurrentIt;
		while ( !pPool->AllocateDescriptorSets( descriptorSetsLayout.GetVkAllocateInfo(), pDestDescriptorSets ) )
		{
			pPool = GetFreePool( true );
		}

		return true;
	}
	return false;
}

/*
==================
CStudioAPITypedDescriptorPoolSetVk::GetFreePool
==================
*/
CStudioAPIDescriptorPoolVk* CStudioAPITypedDescriptorPoolSetVk::GetFreePool( bool bForceNewPool )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	if ( !bForceNewPool )
	{
		return *poolCurrentIt;
	}

	std::list<CStudioAPIDescriptorPoolVk*>::iterator	nextPoolIt = std::next( poolCurrentIt, 1 );
	if ( nextPoolIt != poolList.end() )
	{
		poolCurrentIt = nextPoolIt;
		return *poolCurrentIt;
	}

	return PushNewPool();
}

/*
==================
CStudioAPITypedDescriptorPoolSetVk::PushNewPool
==================
*/
CStudioAPIDescriptorPoolVk* CStudioAPITypedDescriptorPoolSetVk::PushNewPool()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Max number of descriptor sets layout allocations
	const uint32	maxSetsAllocationsBase = 32;

	// Allow max 128 sets per pool (32 << 2)
	const uint32	maxSetsAllocations = maxSetsAllocationsBase << Min<uint32>( ( uint32 )poolList.size(), 2 );

	// Allocate a new descriptor pool
	CStudioAPIDescriptorPoolVk*		pNewPool = poolList.emplace_back( new CStudioAPIDescriptorPoolVk( descriptorSetsLayout, maxSetsAllocations ) );
	poolCurrentIt = --poolList.end();
	return pNewPool;
}


/*
==================
CStudioAPIDescriptorPoolSetContainerVk::CStudioAPIDescriptorPoolSetContainerVk
==================
*/
CStudioAPIDescriptorPoolSetContainerVk::CStudioAPIDescriptorPoolSetContainerVk()
	: bUsed( true )
	, lastFrameUsed( g_StudioAPIVk.GetFrameNumber() )
{}

/*
==================
CStudioAPIDescriptorPoolSetContainerVk::~CStudioAPIDescriptorPoolSetContainerVk
==================
*/
CStudioAPIDescriptorPoolSetContainerVk::~CStudioAPIDescriptorPoolSetContainerVk()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	for ( auto it = typedDescriptorPoolsDict.begin(), itEnd = typedDescriptorPoolsDict.end(); it != itEnd; ++it )
	{
		delete it->second;
	}
}

/*
==================
CStudioAPIDescriptorPoolSetContainerVk::SetUsed
==================
*/
void CStudioAPIDescriptorPoolSetContainerVk::SetUsed( bool bUsed )
{
	CStudioAPIDescriptorPoolSetContainerVk::bUsed = bUsed;
	lastFrameUsed = g_StudioAPIVk.GetFrameNumber();
}


/*
==================
CStudioAPIDescriptorPoolsMgrVk::CStudioAPIDescriptorPoolsMgrVk
==================
*/
CStudioAPIDescriptorPoolsMgrVk::CStudioAPIDescriptorPoolsMgrVk()
	: pStudioAPIVkShutdownDelegate( NULL )
{}

/*
==================
CStudioAPIDescriptorPoolsMgrVk::~CStudioAPIDescriptorPoolsMgrVk
==================
*/
CStudioAPIDescriptorPoolsMgrVk::~CStudioAPIDescriptorPoolsMgrVk()
{
	Shutdown();
}

/*
==================
CStudioAPIDescriptorPoolsMgrVk::Init
==================
*/
void CStudioAPIDescriptorPoolsMgrVk::Init()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Register in 'onStudioAPIVkShutodwn' for destroy Vulkan objects when the one is shutdown
	pStudioAPIVkShutdownDelegate = g_StudioAPIVk.OnStudioAPIVkShutdown().AddFunc( &CStudioAPIDescriptorPoolsMgrVk::OnStudioAPIVkShutdown, this );
}

/*
==================
CStudioAPIDescriptorPoolsMgrVk::Shutdown
==================
*/
void CStudioAPIDescriptorPoolsMgrVk::Shutdown()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );

	// Delete all pool sets
	for ( auto it = poolSets.begin(), itEnd = poolSets.end(); it != itEnd; ++it )
	{
		delete *it;
	}
	poolSets.clear();

	// Remove CStudioAPIDescriptorPoolsMgrVk::OnStudioAPIVkShutdown from event 'onStudioAPIVkShutodwn'
	if ( pStudioAPIVkShutdownDelegate )
	{
		g_StudioAPIVk.OnStudioAPIVkShutdown().RemoveFunc( pStudioAPIVkShutdownDelegate );
		pStudioAPIVkShutdownDelegate = NULL;
	}
}

/*
==================
CStudioAPIDescriptorPoolsMgrVk::AcquirePoolSet
==================
*/
CStudioAPIDescriptorPoolSetContainerVk* CStudioAPIDescriptorPoolsMgrVk::AcquirePoolSet()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	for ( auto it = poolSets.begin(), itEnd = poolSets.end(); it != itEnd; ++it )
	{
		CStudioAPIDescriptorPoolSetContainerVk*		pPoolSet = *it;
		if ( !pPoolSet->IsUsed() )
		{
			pPoolSet->SetUsed( true );
			pPoolSet->Reset();
			return pPoolSet;
		}
	}
	return poolSets.emplace_back( new CStudioAPIDescriptorPoolSetContainerVk() );
}

/*
==================
CStudioAPIDescriptorPoolsMgrVk::ReleasePoolSet
==================
*/
void CStudioAPIDescriptorPoolsMgrVk::ReleasePoolSet( CStudioAPIDescriptorPoolSetContainerVk* pPoolSet )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	if ( !poolSets.empty() )
	{
		Assert( pPoolSet );
		pPoolSet->SetUsed( false );
	}
}

/*
==================
CStudioAPIDescriptorPoolsMgrVk::FreeUnusedPoolSets
==================
*/
void CStudioAPIDescriptorPoolsMgrVk::FreeUnusedPoolSets()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	for ( auto it = poolSets.rbegin(), itEnd = poolSets.rend(); it != itEnd; ++it )
	{
		CStudioAPIDescriptorPoolSetContainerVk*		pPoolSet = *it;
		if ( !pPoolSet->IsUsed() && g_StudioAPIVk.GetFrameNumber() - pPoolSet->GetLastFrameUsed() > STUDIOAPI_VK_NUM_FRAMES_TO_WAIT_BEFORE_RELEASING_TO_OS )
		{
			delete pPoolSet;
			poolSets.erase( std::next( it ).base() );
			break;
		}
	}
}

/*
==================
CStudioAPIDescriptorPoolsMgrVk::OnStudioAPIVkShutdown
==================
*/
void CStudioAPIDescriptorPoolsMgrVk::OnStudioAPIVkShutdown( void* pUserData )
{
	Assert( pUserData );
	CStudioAPIDescriptorPoolsMgrVk*		pStudioAPIDescriptorPoolsMgr	= ( CStudioAPIDescriptorPoolsMgrVk* )pUserData;
	pStudioAPIDescriptorPoolsMgr->pStudioAPIVkShutdownDelegate			= NULL;
	pStudioAPIDescriptorPoolsMgr->Shutdown();
}