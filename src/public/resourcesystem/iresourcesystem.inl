#pragma once

/*
==================
IResourceSystem::InstallResourceManagerForType
==================
*/
template<class TResourceClass>
FORCEINLINE IResourceTypeMgr* IResourceSystem::InstallResourceManagerForType()
{
	return InstallResourceManager( resourceTypeInfo_t<TResourceClass>::RESOURCE_TYPE );
}

/*
==================
IResourceSystem::RemoveResourceManagerForType
==================
*/
template<class TResourceClass>
FORCEINLINE void IResourceSystem::RemoveResourceManagerForType()
{
	return RemoveResourceManager( resourceTypeInfo_t<TResourceClass>::RESOURCE_TYPE );
}

/*
==================
IResourceSystem::HasResourceManagerForType
==================
*/
template<class TResourceClass>
FORCEINLINE bool IResourceSystem::HasResourceManagerForType() const
{
	return HasResourceManager( resourceTypeInfo_t<TResourceClass>::RESOURCE_TYPE );
}

/*
==================
IResourceSystem::GetResourceManagerForType
==================
*/
template<class TResourceClass>
FORCEINLINE IResourceTypeMgr* IResourceSystem::GetResourceManagerForType() const
{
	Assert( HasResourceManagerForType<TResourceClass>() );
	return GetResourceManager( resourceTypeInfo_t<TResourceClass>::RESOURCE_TYPE );
}
