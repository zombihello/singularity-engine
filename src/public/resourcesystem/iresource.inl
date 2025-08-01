#pragma once

/*
==================
TResourcePtr::operator=
==================
*/
template<class TResourceClass>
FORCEINLINE TResourcePtr<TResourceClass>& TResourcePtr<TResourceClass>::operator=( IResource* pPtr )
{
	TResourcePtr::pPtr = pPtr;
	Assert( !pPtr || ResourceSystem_IsValidClassForType<TResourceClass>( pPtr->GetType() ) );
	return *this;
}

/*
==================
TResourcePtr::operator=
==================
*/
template<class TResourceClass>
FORCEINLINE TResourcePtr<TResourceClass>& TResourcePtr<TResourceClass>::operator=( const TResourcePtr& copy )
{
	pPtr = copy.pPtr;
	Assert( !pPtr || ResourceSystem_IsValidClassForType<TResourceClass>( pPtr->GetType() ) );
	return *this;
}

/*
==================
TResourcePtr::operator==
==================
*/
template<class TResourceClass>
FORCEINLINE bool TResourcePtr<TResourceClass>::operator==( const TResourcePtr& right ) const
{
	return pPtr == right.pPtr;
}

/*
==================
TResourcePtr::operator==
==================
*/
template<class TResourceClass>
FORCEINLINE bool TResourcePtr<TResourceClass>::operator==( IResource* pRight ) const
{
	return pPtr == pRight;
}

/*
==================
TResourcePtr::operator!=
==================
*/
template<class TResourceClass>
FORCEINLINE bool TResourcePtr<TResourceClass>::operator!=( const TResourcePtr& right ) const
{
	return pPtr != right.pPtr;
}

/*
==================
TResourcePtr::operator!=
==================
*/
template<class TResourceClass>
FORCEINLINE bool TResourcePtr<TResourceClass>::operator!=( IResource* pRight ) const
{
	return pPtr != pRight;
}

/*
==================
TResourcePtr::operator bool
==================
*/
template<class TResourceClass>
FORCEINLINE TResourcePtr<TResourceClass>::operator bool() const
{
	return !!pPtr;
}

/*
==================
TResourcePtr::operator ptrint
==================
*/
template<class TResourceClass>
FORCEINLINE TResourcePtr<TResourceClass>::operator ptrint() const
{
	return ( ptrint )pPtr;
}

/*
==================
TResourcePtr::operator uptrint
==================
*/
template<class TResourceClass>
FORCEINLINE TResourcePtr<TResourceClass>::operator uptrint() const
{
	return ( uptrint )pPtr;
}

/*
==================
TResourcePtr::operator IResource*
==================
*/
template<class TResourceClass>
FORCEINLINE TResourcePtr<TResourceClass>::operator IResource*() const
{
	return ( IResource* )pPtr;
}

/*
==================
TResourcePtr::operator->
==================
*/
template<class TResourceClass>
FORCEINLINE TResourceClass* TResourcePtr<TResourceClass>::operator->() const
{
	Assert( pPtr );
	return ( TResourceClass* )pPtr->GetData();
}

/*
==================
TResourcePtr::operator*
==================
*/
template<class TResourceClass>
FORCEINLINE TResourceClass* TResourcePtr<TResourceClass>::operator*() const
{
	Assert( pPtr );
	return ( TResourceClass* )pPtr->GetData();
}

/*
==================
TResourcePtr::GetPtr
==================
*/
template<class TResourceClass>
FORCEINLINE IResource* TResourcePtr<TResourceClass>::GetPtr() const
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

/*
==================
ResourceSystem_IsValidClassForType
==================
*/
template<>
FORCEINLINE bool ResourceSystem_IsValidClassForType<IMap>( resourceType_t type )
{
	return type == RESOURCE_TYPE_MAP;
}