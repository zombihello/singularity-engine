#pragma once

/*
==================
CResourceData::CResourceData
==================
*/
template<class TBaseClass>
FORCEINLINE CResourceData<TBaseClass>::CResourceData( IResource* pResource )
	: pResource( pResource )
{
}

/*
==================
CResourceData::CollectDependencies
==================
*/
template<class TBaseClass>
FORCEINLINE void CResourceData<TBaseClass>::CollectDependencies( IResourceDependencyCollector* pCollector ) const
{
}

/*
==================
CResourceData::GetResource
==================
*/
template<class TBaseClass>
FORCEINLINE IResource* CResourceData<TBaseClass>::GetResource() const
{
	return pResource;
}
