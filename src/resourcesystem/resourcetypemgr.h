#pragma once
#include <EASTL/vector.h>
#include <EASTL/list.h>
#include <EASTL/unordered_map.h>

#include "tier1/stl.h"
#include "resourcesystem/iresourcetypemgr.h"
#include "resourcesystem/resource.h"

//-----------------------------------------------------------------------------
// A resource type manager
//-----------------------------------------------------------------------------
class CResourceTypeMgr : public IResourceTypeMgr
{
public:
	// Functions register/unregister a resource type factory
	// NOTE: In one time in the resource type manager can be only one a resource type factory
	virtual void RegisterResourceFactory( IResourceTypeFactory* pResourceTypeFactory ) override;
	virtual void UnregisterResourceFactory() override;

	// Functions register/unregister resource loaders
	virtual void RegisterResourceLoader( IResourceTypeLoader* pResourceTypeLoader ) override;
	virtual void UnregisterResourceLoader( IResourceTypeLoader* pResourceTypeLoader ) override;
	virtual void UnregisterAllResourceLoaders() override;

	// Functions to create/load/find a resource
	virtual CRefPtr<IResource> CreateResource( const char* pName, uint8 flags = RESOURCE_FLAG_NONE ) override;
	virtual CRefPtr<IResource> LoadResource( const char* pName, const char* pPath, uint8 flags = RESOURCE_FLAG_NONE ) override;
	virtual bool			   LoadResource( IResource* pResource, const char* pPath ) const override;
	virtual CRefPtr<IResource> FindResource( const char* pName, bool bDefaultResourceIfNotFound = true ) const override;

	// Functions add/remove into the manager a resource
	virtual void AddResource( IResource* pResource ) override;
	virtual void RemoveResource( IResource* pResource ) override;

	// Functions to uncache all resources
	virtual void UncacheAllResources() override;

	// Functions to set/get a default resource
	virtual void			   SetDefaultResource( IResource* pResource ) override;
	virtual CRefPtr<IResource> GetDefaultResource() const override;
	virtual resourceType_t	   GetResourceType() const override;

	CResourceTypeMgr( resourceType_t resourceType );
	~CResourceTypeMgr();

	// Performs per-frame resource management. Handles tasks like uncache unused resources
	// and other maintenance operations
	void FrameUpdate();

	// To bring a data in or out of memory
	bool CacheResource( CResource* pResource );
	void UncacheResource( CResource* pResource );

private:
	resourceType_t																									   resourceType;
	IResourceTypeFactory*																							   pResourceTypeFactory;
	CRefPtr<CResource>																								   pDefaultResource;
	eastl::vector<IResourceTypeLoader*>																				   resourceTypeLoaders;
	eastl::list<CResource*>																							   lruResourcesList;
	eastl::unordered_map<eastl::string, CRefPtr<CResource>, stlInsensitiveStringHash_t, stlInsensitiveCompareString_t> resourcesDict;
};
