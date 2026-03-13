#pragma once
#include "tier1/refcount.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IMaterial;
class ITexture;
class IEntityDesc;

//-----------------------------------------------------------------------------
// Resource types
//-----------------------------------------------------------------------------
enum resourceType_t
{
	RESOURCE_TYPE_MATERIAL,
	RESOURCE_TYPE_TEXTURE,
	RESOURCE_TYPE_MODEL,
	RESOURCE_TYPE_ENTITY_DESC,
	RESOURCE_NUM_TYPES
};

//-----------------------------------------------------------------------------
// Resource interface
//-----------------------------------------------------------------------------
class IResource : public IRefCounted
{
public:
	virtual ~IResource() {}

	// If the data isn't valid return the default resource or NULL if it isn't exist in the system
	virtual IRefCounted*   GetData() const = 0;
	virtual resourceType_t GetType() const = 0;
};

//-----------------------------------------------------------------------------
// Resource pointer
//-----------------------------------------------------------------------------
template<class TClassType>
bool ResourceSystem_IsValidClassForType( resourceType_t type );

template<class TResourceClass>
class CResourcePtr
{
public:
	CResourcePtr()
	{
	}
	CResourcePtr( IResource* pPtr )
		: pPtr( pPtr )
	{
		Assert( !pPtr || ResourceSystem_IsValidClassForType<TResourceClass>( pPtr->GetType() ) );
	}
	CResourcePtr( const CResourcePtr& copy )
		: pPtr( copy.pPtr )
	{
		Assert( !pPtr || ResourceSystem_IsValidClassForType<TResourceClass>( pPtr->GetType() ) );
	}

	IResource* GetRawPtr() const;

	CResourcePtr&	operator=( IResource* pPtr );
	CResourcePtr&	operator=( const CResourcePtr& copy );
	bool			operator==( const CResourcePtr& right ) const;
	bool			operator==( IResource* pRight ) const;
	bool			operator!=( const CResourcePtr& right ) const;
	bool			operator!=( IResource* pRight ) const;
					operator bool() const;
					operator ptrint() const;
					operator uptrint() const;
					operator IResource*() const;
	TResourceClass* operator->() const;
	TResourceClass* operator*() const;

private:
	CRefPtr<IResource> pPtr;
};

#include "resourcesystem/iresource.inl"
