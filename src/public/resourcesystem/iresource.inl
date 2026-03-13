#pragma once

/*
==================
CResourcePtr::operator=
==================
*/
template<class TResourceClass>
FORCEINLINE CResourcePtr<TResourceClass>& CResourcePtr<TResourceClass>::operator=( IResource* pPtr )
{
	CResourcePtr::pPtr = pPtr;
	Assert( !pPtr || ResourceSystem_IsValidClassForType<TResourceClass>( pPtr->GetType() ) );
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
	Assert( !pPtr || ResourceSystem_IsValidClassForType<TResourceClass>( pPtr->GetType() ) );
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
	return (IResource*)pPtr;
}

/*
==================
CResourcePtr::operator->
==================
*/
template<class TResourceClass>
FORCEINLINE TResourceClass* CResourcePtr<TResourceClass>::operator->() const
{
	Assert( pPtr );
	return (TResourceClass*)pPtr->GetData();
}

/*
==================
CResourcePtr::operator*
==================
*/
template<class TResourceClass>
FORCEINLINE TResourceClass* CResourcePtr<TResourceClass>::operator*() const
{
	Assert( pPtr );
	return (TResourceClass*)pPtr->GetData();
}

/*
==================
CResourcePtr::GetRawPtr
==================
*/
template<class TResourceClass>
FORCEINLINE IResource* CResourcePtr<TResourceClass>::GetRawPtr() const
{
	return pPtr;
}

/*
==================
ResourceSystem_IsValidClassForType
==================
*/
template<class TClassType>
FORCEINLINE bool ResourceSystem_IsValidClassForType( resourceType_t type )
{
	static_assert( false, "Invalid TResourceClass, the class isn't corresponds to any resource type" );
	return false;
}

/*
==================
ResourceSystem_IsValidClassForType
==================
*/
template<>
FORCEINLINE bool ResourceSystem_IsValidClassForType<IMaterial>( resourceType_t type )
{
	return type == RESOURCE_TYPE_MATERIAL;
}

/*
==================
ResourceSystem_IsValidClassForType
==================
*/
template<>
FORCEINLINE bool ResourceSystem_IsValidClassForType<ITexture>( resourceType_t type )
{
	return type == RESOURCE_TYPE_TEXTURE;
}

/*
==================
ResourceSystem_IsValidClassForType
==================
*/
template<>
FORCEINLINE bool ResourceSystem_IsValidClassForType<IEntityDesc>( resourceType_t type )
{
	return type == RESOURCE_TYPE_ENTITY_DESC;
}