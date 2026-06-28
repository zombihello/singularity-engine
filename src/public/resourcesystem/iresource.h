#pragma once
#include "tier1/refcount.h"
#include "tier1/event.h"
#include "resourcesystem/iresourcetype.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IResource;

//-----------------------------------------------------------------------------
// Resource flags
//-----------------------------------------------------------------------------
enum resourceFlags_t
{
	RESOURCE_FLAG_NONE		= 0,
	RESOURCE_FLAG_DEFAULT	= BIT( 0 ),
	RESOURCE_FLAG_CACHED	= BIT( 1 ),
	RESOURCE_FLAG_PERMANENT = BIT( 2 ),	 // Will never be uncached
	RESOURCE_FLAG_ANONYMOUS = BIT( 3 )	 // Will never be in a resource type manager
};

//-----------------------------------------------------------------------------
// A resource data interface
//-----------------------------------------------------------------------------
class IResourceData
{
public:
	virtual ~IResourceData() {}

	// Marks all dependent resources as used
	virtual void MarkUsedDependencies() = 0;

	// Set/clear permanent flag in all dependent resources
	virtual void MakePermanentDependencies()  = 0;
	virtual void ClearPermanentDependencies() = 0;

	// Get the resource where the data is
	virtual IResource* GetResource() const = 0;
};

//-----------------------------------------------------------------------------
// A resource interface
//-----------------------------------------------------------------------------
class IResource : public IRefCounted
{
public:
	DECLARE_EVENT_INTERFACE( IOnCached, IResource* /* pResource */ );
	DECLARE_EVENT_INTERFACE( IOnUncached, IResource* /* pResource */ );

	virtual ~IResource() {}

	// To bring a data in or out of memory
	virtual bool Cache()   = 0;
	virtual void Uncache() = 0;

	// Mark the resource as used
	virtual void MarkUsed() = 0;

	// Set/clear permanent flag
	virtual void MakePermanent()  = 0;
	virtual void ClearPermanent() = 0;

	virtual bool		   HasAllFlags( uint8 flags ) const = 0;
	virtual bool		   HasAnyFlags( uint8 flags ) const = 0;
	virtual IResourceData* GetData() const					= 0;
	virtual resourceType_t GetType() const					= 0;
	virtual const char*	   GetName() const					= 0;
	virtual const char*	   GetPath() const					= 0;
	virtual IOnCached*	   OnCached() const					= 0;
	virtual IOnUncached*   OnUncached() const				= 0;
};

//-----------------------------------------------------------------------------
// Base class to implement a resource data
//-----------------------------------------------------------------------------
template<class TBaseClass>
class CResourceData : public TBaseClass
{
public:
	// IResourceData interface
	// Marks all dependent resources as used
	virtual void MarkUsedDependencies() override;

	// Set/clear permanent flag in all dependent resources
	virtual void MakePermanentDependencies() override;
	virtual void ClearPermanentDependencies() override;

	// Get the resource where the data is
	virtual IResource* GetResource() const override;

	CResourceData( IResource* pResource );

private:
	IResource* pResource;
};

#include "resourcesystem/iresource.inl"
