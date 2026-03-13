#pragma once
#include <EASTL/unordered_map.h>

#include "resourcesystem/iresourcesystem.h"
#include "resourcesystem/resource.h"

//-----------------------------------------------------------------------------
// Resource system
//-----------------------------------------------------------------------------
class CResourceSystem : public CBaseAppSystem<IResourceSystem>
{
public:
	// IAppSystem interface
	// Here's where the app systems get to learn about each other
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// Initialize and shutdown
	virtual bool Init() override;
	virtual void Shutdown() override;

	// IResourceSystem interface
	// Functions register/unregister a resource factory
	// NOTE: You can't override a resource factory if the one has RESOURCE_FACTORY_FLAG_STATIC (except for the case if the factory has RESOURCE_FACTORY_FLAG_NOT_USED)
	virtual bool RegisterResourceFactory( resourceType_t type, IResourceFactory* pFactory ) override;
	virtual bool UnRegisterResourceFactory( resourceType_t type ) override;

	// NOTE: The path to the resource in the file system can be without file extension, or its name if it is a procedural resource
	virtual CRefPtr<IResource> FindOrLoadResource( const char* pPath, resourceType_t type, uint32 loadFlags = RESOURCE_LOAD_FLAG_NONE ) override;
	virtual CRefPtr<IResource> CreateProceduralResource( const char* pName, resourceType_t type ) override;

	// This function delete any resource that has a refcount <= 1 (one reference in the resource system)
	virtual void RemoveUnusedResources() override;

	// Return a default resource by its type. If the type isn't registered or not have a default resource return NULL
	virtual CRefPtr<IResource> GetDefaultResource( resourceType_t type ) const override;

	virtual bool			  HasResourceFactory( resourceType_t type ) const override;
	virtual IResourceFactory* GetResourceFactory( resourceType_t type ) const override;

private:
	IResourceFactory*										pResourceFactories[RESOURCE_NUM_TYPES];
	eastl::unordered_map<eastl::string, CRefPtr<CResource>> resourcesDicts[RESOURCE_NUM_TYPES];
};
extern CResourceSystem g_ResourceSystem;
