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
CResourceData::MarkUsedDependencies
==================
*/
template<class TBaseClass>
FORCEINLINE void CResourceData<TBaseClass>::MarkUsedDependencies()
{
}

/*
==================
CResourceData::MakePermanentDependencies
==================
*/
template<class TBaseClass>
FORCEINLINE void CResourceData<TBaseClass>::MakePermanentDependencies()
{
}

/*
==================
CResourceData::ClearPermanentDependencies
==================
*/
template<class TBaseClass>
FORCEINLINE void CResourceData<TBaseClass>::ClearPermanentDependencies()
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
