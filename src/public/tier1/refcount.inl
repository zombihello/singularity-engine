#pragma once

/*
==================
CRefCounted::CRefCounted
==================
*/
template<class TBaseClass>
FORCEINLINE CRefCounted<TBaseClass>::CRefCounted()
{
	countReferences.store( 0, eastl::memory_order_release );
}

/*
==================
CRefCounted::~CRefCounted
==================
*/
template<class TBaseClass>
FORCEINLINE CRefCounted<TBaseClass>::~CRefCounted()
{
	Assert( GetRefCount() == 0 );
}

/*
==================
CRefCounted::AddRef
==================
*/
template<class TBaseClass>
FORCEINLINE void CRefCounted<TBaseClass>::AddRef()
{
	// Reference increment does not require synchronization with other data, relaxed is enough
	countReferences.fetch_add( 1, eastl::memory_order_relaxed );
}

/*
==================
CRefCounted::ReleaseRef
==================
*/
template<class TBaseClass>
FORCEINLINE void CRefCounted<TBaseClass>::ReleaseRef()
{
	// Release on decrement publishes all records made by ref holders.
	// If we are the last owner, before delete do acquire-fence
	uint32 prevCountReferences = countReferences.fetch_sub( 1, eastl::memory_order_release );
	if ( prevCountReferences <= 1 )
	{
		eastl::atomic_thread_fence( eastl::memory_order_acquire );
		delete this;
	}
}

/*
==================
CRefCounted::GetRefCount
==================
*/
template<class TBaseClass>
FORCEINLINE uint32 CRefCounted<TBaseClass>::GetRefCount() const
{
	return countReferences.load( eastl::memory_order_relaxed );
}

/*
==================
CRefPtr::CRefPtr
==================
*/
template<typename TPtrType>
FORCEINLINE CRefPtr<TPtrType>::CRefPtr()
	: pPtr( NULL )
{
}

/*
==================
CRefPtr::CRefPtr
==================
*/
template<typename TPtrType>
FORCEINLINE CRefPtr<TPtrType>::CRefPtr( TPtrType* pPtr )
	: pPtr( pPtr )
{
	if ( pPtr )
	{
		pPtr->AddRef();
	}
}

/*
==================
CRefPtr::CRefPtr
==================
*/
template<typename TPtrType>
FORCEINLINE CRefPtr<TPtrType>::CRefPtr( const CRefPtr& other )
	: pPtr( other.pPtr )
{
	if ( pPtr )
	{
		pPtr->AddRef();
	}
}

/*
==================
CRefPtr::CRefPtr
==================
*/
template<typename TPtrType>
template<typename TBasePtrType>
FORCEINLINE CRefPtr<TPtrType>::CRefPtr( const CRefPtr<TBasePtrType>& other )
	: pPtr( (TPtrType*)other.GetRawPtr() )
{
	if ( pPtr )
	{
		pPtr->AddRef();
	}
}

/*
==================
CRefPtr::~CRefPtr
==================
*/
template<typename TPtrType>
FORCEINLINE CRefPtr<TPtrType>::~CRefPtr()
{
	if ( pPtr )
	{
		pPtr->ReleaseRef();
	}
}

/*
==================
CRefPtr::hashFunction_t::operator()
==================
*/
template<typename TPtrType>
FORCEINLINE size CRefPtr<TPtrType>::hashFunction_t::operator()( const CRefPtr& refPtr ) const
{
	return FastHash( refPtr );
}

/*
==================
CRefPtr::operator=
==================
*/
template<typename TPtrType>
FORCEINLINE CRefPtr<TPtrType>& CRefPtr<TPtrType>::operator=( TPtrType* pPtr )
{
	if ( CRefPtr::pPtr )
	{
		CRefPtr::pPtr->ReleaseRef();
	}

	CRefPtr::pPtr = pPtr;

	if ( CRefPtr::pPtr )
	{
		CRefPtr::pPtr->AddRef();
	}
	return *this;
}

/*
==================
CRefPtr::operator=
==================
*/
template<typename TPtrType>
FORCEINLINE CRefPtr<TPtrType>& CRefPtr<TPtrType>::operator=( const CRefPtr& copy )
{
	if ( pPtr )
	{
		pPtr->ReleaseRef();
	}

	pPtr = copy.pPtr;

	if ( pPtr )
	{
		pPtr->AddRef();
	}
	return *this;
}

/*
==================
CRefPtr::operator=
==================
*/
template<typename TPtrType>
template<typename TBasePtrType>
FORCEINLINE CRefPtr<TPtrType>& CRefPtr<TPtrType>::operator=( const CRefPtr<TBasePtrType>& copy )
{
	if ( pPtr )
	{
		pPtr->ReleaseRef();
	}

	pPtr = (TPtrType*)( copy.GetRawPtr() );

	if ( pPtr )
	{
		pPtr->AddRef();
	}
	return *this;
}

/*
==================
CRefPtr::operator==
==================
*/
template<typename TPtrType>
FORCEINLINE bool CRefPtr<TPtrType>::operator==( const CRefPtr& right ) const
{
	return pPtr == right.pPtr;
}

/*
==================
CRefPtr::operator==
==================
*/
template<typename TPtrType>
FORCEINLINE bool CRefPtr<TPtrType>::operator==( TPtrType* pRight ) const
{
	return pPtr == pRight;
}

/*
==================
CRefPtr::operator!=
==================
*/
template<typename TPtrType>
FORCEINLINE bool CRefPtr<TPtrType>::operator!=( const CRefPtr& right ) const
{
	return pPtr != right.pPtr;
}

/*
==================
CRefPtr::operator!=
==================
*/
template<typename TPtrType>
FORCEINLINE bool CRefPtr<TPtrType>::operator!=( TPtrType* pRight ) const
{
	return pPtr != pRight;
}

/*
==================
CRefPtr::operator bool
==================
*/
template<typename TPtrType>
FORCEINLINE CRefPtr<TPtrType>::operator bool() const
{
	return !!pPtr;
}

/*
==================
CRefPtr::operator ptrint
==================
*/
template<typename TPtrType>
FORCEINLINE CRefPtr<TPtrType>::operator ptrint() const
{
	return (ptrint)pPtr;
}

/*
==================
CRefPtr::operator uptrint
==================
*/
template<typename TPtrType>
FORCEINLINE CRefPtr<TPtrType>::operator uptrint() const
{
	return (uptrint)pPtr;
}

/*
==================
CRefPtr::operator TPtrType&
==================
*/
template<typename TPtrType>
FORCEINLINE CRefPtr<TPtrType>::operator TPtrType&() const
{
	return *pPtr;
}

/*
==================
CRefPtr::operator TPtrType*
==================
*/
template<typename TPtrType>
FORCEINLINE CRefPtr<TPtrType>::operator TPtrType*() const
{
	return pPtr;
}

/*
==================
CRefPtr::operator TPtrType*&
==================
*/
template<typename TPtrType>
FORCEINLINE CRefPtr<TPtrType>::operator TPtrType*&()
{
	return pPtr;
}

/*
==================
CRefPtr::operator->
==================
*/
template<typename TPtrType>
FORCEINLINE TPtrType* CRefPtr<TPtrType>::operator->() const
{
	Assert( pPtr );
	return pPtr;
}

/*
==================
CRefPtr::operator*
==================
*/
template<typename TPtrType>
FORCEINLINE TPtrType& CRefPtr<TPtrType>::operator*()
{
	Assert( pPtr );
	return *pPtr;
}

/*
==================
CRefPtr::operator&
==================
*/
template<typename TPtrType>
FORCEINLINE TPtrType** CRefPtr<TPtrType>::operator&()
{
	Assert( pPtr );
	return &pPtr;
}

/*
==================
CRefPtr::IsValid
==================
*/
template<typename TPtrType>
FORCEINLINE bool CRefPtr<TPtrType>::IsValid() const
{
	return !!pPtr;
}

/*
==================
CRefPtr::GetRefCount
==================
*/
template<typename TPtrType>
FORCEINLINE uint32 CRefPtr<TPtrType>::GetRefCount() const
{
	if ( !pPtr )
	{
		return 0;
	}

	return pPtr->GetRefCount();
}

/*
==================
CRefPtr::GetRawPtr
==================
*/
template<typename TPtrType>
FORCEINLINE TPtrType* CRefPtr<TPtrType>::GetRawPtr() const
{
	return pPtr;
}
