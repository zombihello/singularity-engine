#pragma once

/*
==================
CStudioAPIDescriptorPoolVk::GetVkDescriptorPool
==================
*/
FORCEINLINE VkDescriptorPool CStudioAPIDescriptorPoolVk::GetVkDescriptorPool() const
{
	return vkDescriptorPool;
}


/*
==================
CStudioAPITypedDescriptorPoolSetVk::GetDescriptorSetsLayout
==================
*/
FORCEINLINE const CStudioAPIDescriptorSetsLayoutVk& CStudioAPITypedDescriptorPoolSetVk::GetDescriptorSetsLayout() const
{
	return descriptorSetsLayout;
}


/*
==================
CStudioAPIDescriptorPoolSetContainerVk::AcquireTypedPoolSet
==================
*/
FORCEINLINE CStudioAPITypedDescriptorPoolSetVk* CStudioAPIDescriptorPoolSetContainerVk::AcquireTypedPoolSet( const CStudioAPIDescriptorSetsLayoutVk& descriptorSetsLayout )
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

/*
==================
CStudioAPIDescriptorPoolSetContainerVk::Reset
==================
*/
FORCEINLINE void CStudioAPIDescriptorPoolSetContainerVk::Reset()
{
	for ( auto it = typedDescriptorPoolsDict.begin(), itEnd = typedDescriptorPoolsDict.end(); it != itEnd; ++it )
	{
		it->second->Reset();
	}
}

/*
==================
CStudioAPIDescriptorPoolSetContainerVk::IsUsed
==================
*/
FORCEINLINE bool CStudioAPIDescriptorPoolSetContainerVk::IsUsed() const
{
	return bUsed;
}

/*
==================
CStudioAPIDescriptorPoolSetContainerVk::GetLastFrameUsed
==================
*/
FORCEINLINE uint64 CStudioAPIDescriptorPoolSetContainerVk::GetLastFrameUsed() const
{
	return lastFrameUsed;
}
