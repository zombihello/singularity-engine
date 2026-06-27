#pragma once

/*
==================
CResourceTypeFactory::Create
==================
*/
template<class TResourceDataClass>
FORCEINLINE IResourceData* CResourceTypeFactory<TResourceDataClass>::Create( IResource* pResource ) const
{
	return new TResourceDataClass( pResource );
}

/*
==================
CResourceTypeFactory::Delete
==================
*/
template<class TResourceDataClass>
FORCEINLINE void CResourceTypeFactory<TResourceDataClass>::Delete( IResourceData* pData ) const
{
	delete (TResourceDataClass*)pData;
}
