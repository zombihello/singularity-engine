#pragma once
#include "tier1/refcount.h"
#include "resourcesystem/iresourcetype.h"
#include "resourcesystem/iresource.h"

//-----------------------------------------------------------------------------
// A resource type manager
//-----------------------------------------------------------------------------
class IResourceTypeMgr
{
public:
	// Functions register/unregister a resource type factory
	// NOTE: In one time in the resource type manager can be only one a resource type factory
	virtual void RegisterResourceFactory( IResourceTypeFactory* pResourceTypeFactory ) = 0;
	virtual void UnregisterResourceFactory()										   = 0;

	// Functions register/unregister resource loaders
	virtual void RegisterResourceLoader( IResourceTypeLoader* pResourceTypeLoader )	  = 0;
	virtual void UnregisterResourceLoader( IResourceTypeLoader* pResourceTypeLoader ) = 0;
	virtual void UnregisterAllResourceLoaders()										  = 0;

	// Functions to create/load/find a resource
	virtual CRefPtr<IResource> CreateResource( const char* pName, uint8 flags = RESOURCE_FLAG_NONE )				  = 0;
	virtual CRefPtr<IResource> LoadResource( const char* pName, const char* pPath, uint8 flags = RESOURCE_FLAG_NONE ) = 0;
	CRefPtr<IResource>		   LoadResource( const char* pPath, uint8 flags = RESOURCE_FLAG_NONE );
	virtual bool			   LoadResource( IResource* pResource, const char* pPath ) const				   = 0;
	virtual CRefPtr<IResource> FindResource( const char* pName, bool bDefaultResourceIfNotFound = true ) const = 0;

	// Functions add/remove into the manager a resource
	virtual void AddResource( IResource* pResource )	= 0;
	virtual void RemoveResource( IResource* pResource ) = 0;

	// Functions to uncache all resources
	virtual void UncacheAllResources() = 0;

	// Functions to set/get a default resource
	virtual void			   SetDefaultResource( IResource* pResource ) = 0;
	virtual CRefPtr<IResource> GetDefaultResource() const				  = 0;
	virtual resourceType_t	   GetResourceType() const					  = 0;
};

#include "resourcesystem/iresourcetypemgr.inl"
