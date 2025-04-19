#ifndef REFCOUNT_INL
#define REFCOUNT_INL

/*
==================
TRefCounted::AddRef
==================
*/
template<class TBaseClass>
void TRefCounted<TBaseClass>::AddRef()
{
	Sys_InterlockedIncrement( ( int32* )&countReferences );
}

/*
==================
TRefCounted::ReleaseRef
==================
*/
template<class TBaseClass>
void TRefCounted<TBaseClass>::ReleaseRef()
{
	if ( !countReferences || !Sys_InterlockedDecrement( ( int32* )&countReferences ) )
	{
		delete this;
	}
}

/*
==================
TRefCounted::GetRefCount
==================
*/
template<class TBaseClass>
uint32 TRefCounted<TBaseClass>::GetRefCount() const
{
	return countReferences;
}


/*
==================
TRefPtr::hashFunction_t::operator()
==================
*/
template<typename TPtrType>
FORCEINLINE std::size_t TRefPtr<TPtrType>::hashFunction_t::operator()( const TRefPtr& refPtr ) const
{
	return FastHash( refPtr );
}

/*
==================
TRefPtr::operator=
==================
*/
template<typename TPtrType>
FORCEINLINE TRefPtr<TPtrType>& TRefPtr<TPtrType>::operator=( TPtrType* pPtr )
{
	if ( TRefPtr::pPtr )
	{
		TRefPtr::pPtr->ReleaseRef();
	}

	TRefPtr::pPtr = pPtr;

	if ( TRefPtr::pPtr )
	{
		TRefPtr::pPtr->AddRef();
	}
	return *this;
}

/*
==================
TRefPtr::operator=
==================
*/
template<typename TPtrType>
FORCEINLINE TRefPtr<TPtrType>& TRefPtr<TPtrType>::operator=( const TRefPtr& copy )
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
TRefPtr::operator=
==================
*/
template<typename TPtrType>
template<typename TBasePtrType>
FORCEINLINE TRefPtr<TPtrType>& TRefPtr<TPtrType>::operator=( const TRefPtr<TBasePtrType>& copy )
{
	if ( pPtr )
	{
		pPtr->ReleaseRef();
	}

	pPtr = ( TPtrType* )( copy.GetPtr() );

	if ( pPtr )
	{
		pPtr->AddRef();
	}
	return *this;
}

/*
==================
TRefPtr::operator==
==================
*/
template<typename TPtrType>
FORCEINLINE bool TRefPtr<TPtrType>::operator==( const TRefPtr& right ) const
{
	return pPtr == right.pPtr;
}

/*
==================
TRefPtr::operator==
==================
*/
template<typename TPtrType>
FORCEINLINE bool TRefPtr<TPtrType>::operator==( TPtrType* pRight ) const
{
	return pPtr == pRight;
}

/*
==================
TRefPtr::operator!=
==================
*/
template<typename TPtrType>
FORCEINLINE bool TRefPtr<TPtrType>::operator!=( const TRefPtr& right ) const
{
	return pPtr != right.pPtr;
}

/*
==================
TRefPtr::operator!=
==================
*/
template<typename TPtrType>
FORCEINLINE bool TRefPtr<TPtrType>::operator!=( TPtrType* pRight ) const
{
	return pPtr != pRight;
}

/*
==================
TRefPtr::operator bool
==================
*/
template<typename TPtrType>
FORCEINLINE TRefPtr<TPtrType>::operator bool() const
{
	return !!pPtr;
}

/*
==================
TRefPtr::operator ptrint
==================
*/
template<typename TPtrType>
FORCEINLINE TRefPtr<TPtrType>::operator ptrint() const
{
	return ( ptrint )pPtr;
}

/*
==================
TRefPtr::operator uptrint
==================
*/
template<typename TPtrType>
FORCEINLINE TRefPtr<TPtrType>::operator uptrint() const
{
	return ( uptrint )pPtr;
}

/*
==================
TRefPtr::operator TPtrType&
==================
*/
template<typename TPtrType>
FORCEINLINE TRefPtr<TPtrType>::operator TPtrType&() const
{
	return *pPtr;
}

/*
==================
TRefPtr::operator TPtrType*
==================
*/
template<typename TPtrType>
FORCEINLINE TRefPtr<TPtrType>::operator TPtrType*() const
{
	return pPtr;
}

/*
==================
TRefPtr::operator TPtrType*&
==================
*/
template<typename TPtrType>
FORCEINLINE TRefPtr<TPtrType>::operator TPtrType*&()
{
	return pPtr;
}

/*
==================
TRefPtr::operator->
==================
*/
template<typename TPtrType>
FORCEINLINE TPtrType* TRefPtr<TPtrType>::operator->() const
{
	Assert( pPtr );
	return pPtr;
}

/*
==================
TRefPtr::operator*
==================
*/
template<typename TPtrType>
FORCEINLINE TPtrType& TRefPtr<TPtrType>::operator*()
{
	Assert( pPtr );
	return *pPtr;
}

/*
==================
TRefPtr::operator&
==================
*/
template<typename TPtrType>
FORCEINLINE TPtrType** TRefPtr<TPtrType>::operator&()
{
	Assert( pPtr );
	return &pPtr;
}

/*
==================
TRefPtr<TPtrType>::SafeRelease
==================
*/
template<typename TPtrType>
FORCEINLINE void TRefPtr<TPtrType>::SafeRelease()
{
	*this = NULL;
}

/*
==================
TRefPtr::IsValid
==================
*/
template<typename TPtrType>
FORCEINLINE bool TRefPtr<TPtrType>::IsValid() const
{
	return !!pPtr;
}

/*
==================
TRefPtr::GetRefCount
==================
*/
template<typename TPtrType>
FORCEINLINE uint32 TRefPtr<TPtrType>::GetRefCount() const
{
	if ( !pPtr )
	{
		return 0;
	}

	return pPtr->GetRefCount();
}

/*
==================
TRefPtr::GetPtr
==================
*/
template<typename TPtrType>
FORCEINLINE TPtrType* TRefPtr<TPtrType>::GetPtr() const
{
	return pPtr;
}

#endif // !REFCOUNT_INL
