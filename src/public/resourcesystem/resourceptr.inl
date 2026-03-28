#pragma once

/*
==================
CResourcePtr::CResourcePtr
==================
*/
template<class TResourceClass>
FORCEINLINE CResourcePtr<TResourceClass>::CResourcePtr()
{
}

/*
==================
CResourcePtr::CResourcePtr
==================
*/
template<class TResourceClass>
FORCEINLINE CResourcePtr<TResourceClass>::CResourcePtr( IResource* pPtr )
	: pPtr( pPtr )
{
	Assert( !pPtr || pPtr->GetType() == resourceTypeInfo_t<TResourceClass>::RESOURCE_TYPE );
}

/*
==================
CResourcePtr::CResourcePtr
==================
*/
template<class TResourceClass>
FORCEINLINE CResourcePtr<TResourceClass>::CResourcePtr( const CResourcePtr& copy )
	: pPtr( copy.pPtr )
{
	Assert( !pPtr || pPtr->GetType() == resourceTypeInfo_t<TResourceClass>::RESOURCE_TYPE );
}

/*
==================
CResourcePtr::Cache
==================
*/
template<class TResourceClass>
FORCEINLINE bool CResourcePtr<TResourceClass>::Cache()
{
	Assert( pPtr );
	pPtr->MarkUsed();
	return pPtr->Cache();
}

/*
==================
CResourcePtr::Uncache
==================
*/
template<class TResourceClass>
FORCEINLINE void CResourcePtr<TResourceClass>::Uncache()
{
	Assert( pPtr );
	pPtr->MarkUsed();
	pPtr->Uncache();
}

/*
==================
CResourcePtr::Reload
==================
*/
template<class TResourceClass>
FORCEINLINE bool CResourcePtr<TResourceClass>::Reload()
{
	Assert( pPtr );
	pPtr->MarkUsed();
	pPtr->Uncache();
	return pPtr->Cache();
}

/*
==================
CResourcePtr::MarkUsed
==================
*/
template<class TResourceClass>
FORCEINLINE void CResourcePtr<TResourceClass>::MarkUsed()
{
	Assert( pPtr );
	pPtr->MarkUsed();
}

/*
==================
CResourcePtr::IsValid
==================
*/
template<class TResourceClass>
FORCEINLINE bool CResourcePtr<TResourceClass>::IsValid() const
{
	return !!pPtr;
}

/*
==================
CResourcePtr::IsCached
==================
*/
template<class TResourceClass>
FORCEINLINE bool CResourcePtr<TResourceClass>::IsCached() const
{
	return pPtr && pPtr->HasAnyFlags( RESOURCE_FLAG_CACHED );
}

/*
==================
CResourcePtr::GetResource
==================
*/
template<class TResourceClass>
FORCEINLINE IResource* CResourcePtr<TResourceClass>::GetResource() const
{
	if ( pPtr )
	{
		pPtr->MarkUsed();
	}
	return pPtr;
}

/*
==================
CResourcePtr::GetData
==================
*/
template<class TResourceClass>
FORCEINLINE TResourceClass* CResourcePtr<TResourceClass>::GetData() const
{
	if ( !IsCached() )
	{
		return NULL;
	}

	pPtr->MarkUsed();
	return (TResourceClass*)pPtr->GetData();
}

/*
==================
CResourcePtr::operator=
==================
*/
template<class TResourceClass>
FORCEINLINE CResourcePtr<TResourceClass>& CResourcePtr<TResourceClass>::operator=( IResource* pPtr )
{
	CResourcePtr::pPtr = pPtr;
	Assert( !pPtr || pPtr->GetType() == resourceTypeInfo_t<TResourceClass>::RESOURCE_TYPE );
	return *this;
}

/*
==================
CResourcePtr::operator=
==================
*/
template<class TResourceClass>
FORCEINLINE CResourcePtr<TResourceClass>& CResourcePtr<TResourceClass>::operator=( const CResourcePtr& copy )
{
	pPtr = copy.pPtr;
	Assert( !pPtr || pPtr->GetType() == resourceTypeInfo_t<TResourceClass>::RESOURCE_TYPE );
	return *this;
}

/*
==================
CResourcePtr::operator==
==================
*/
template<class TResourceClass>
FORCEINLINE bool CResourcePtr<TResourceClass>::operator==( const CResourcePtr& right ) const
{
	return pPtr == right.pPtr;
}

/*
==================
CResourcePtr::operator==
==================
*/
template<class TResourceClass>
FORCEINLINE bool CResourcePtr<TResourceClass>::operator==( IResource* pRight ) const
{
	return pPtr == pRight;
}

/*
==================
CResourcePtr::operator!=
==================
*/
template<class TResourceClass>
FORCEINLINE bool CResourcePtr<TResourceClass>::operator!=( const CResourcePtr& right ) const
{
	return pPtr != right.pPtr;
}

/*
==================
CResourcePtr::operator!=
==================
*/
template<class TResourceClass>
FORCEINLINE bool CResourcePtr<TResourceClass>::operator!=( IResource* pRight ) const
{
	return pPtr != pRight;
}

/*
==================
CResourcePtr::operator bool
==================
*/
template<class TResourceClass>
FORCEINLINE CResourcePtr<TResourceClass>::operator bool() const
{
	return !!pPtr;
}

/*
==================
CResourcePtr::operator ptrint
==================
*/
template<class TResourceClass>
FORCEINLINE CResourcePtr<TResourceClass>::operator ptrint() const
{
	return (ptrint)pPtr;
}

/*
==================
CResourcePtr::operator uptrint
==================
*/
template<class TResourceClass>
FORCEINLINE CResourcePtr<TResourceClass>::operator uptrint() const
{
	return (uptrint)pPtr;
}

/*
==================
CResourcePtr::operator IResource*
==================
*/
template<class TResourceClass>
FORCEINLINE CResourcePtr<TResourceClass>::operator IResource*() const
{
	return GetResource();
}

/*
==================
CResourcePtr::operator TResourceClass*
==================
*/
template<class TResourceClass>
FORCEINLINE CResourcePtr<TResourceClass>::operator TResourceClass*() const
{
	return GetData();
}

/*
==================
CResourcePtr::operator->
==================
*/
template<class TResourceClass>
FORCEINLINE TResourceClass* CResourcePtr<TResourceClass>::operator->() const
{
	Assert( IsCached() );
	return GetData();
}

/*
==================
CResourcePtr::operator*
==================
*/
template<class TResourceClass>
FORCEINLINE TResourceClass* CResourcePtr<TResourceClass>::operator*() const
{
	Assert( IsCached() );
	return GetData();
}
