#ifndef RESOURCESYSTEM_H
#define RESOURCESYSTEM_H

#include <unordered_map>

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
	// Functions register/unregister a resource factory and reset a default resource for the resource type
	virtual void RegisterResourceFactory( resourceType_t type, IResourceFactory* pFactory ) override;
	virtual void UnRegisterResourceFactory( resourceType_t type ) override;

	virtual TRefPtr<IResource> CreateProceduralResource( const achar* pName, resourceType_t type ) override;

	// NOTE: The path to the resource in the file system can be without file extension, or its name if it is a procedural resource
	virtual TRefPtr<IResource> FindOrLoadResource( const achar* pPath, resourceType_t type, uint32 loadFlags = RESOURCE_LOAD_FLAG_NONE ) override;

	// This function delete any resource that has a refcount <= 1 (one reference in the resource system)
	virtual void RemoveUnusedResources() override;

	// Return a default resource by its type. If the type isn't registered or not have a default resource return NULL
	virtual TRefPtr<IResource> GetDefaultResource( resourceType_t type ) const override;

	virtual bool HasResourceFactory( resourceType_t type ) const override;
	virtual IResourceFactory* GetResourceFactory( resourceType_t type ) const override;

private:
	IResourceFactory*										pResourceFactories[RESOURCE_NUM_TYPES];
	std::unordered_map<std::string, TRefPtr<CResource>>		resourcesDicts[RESOURCE_NUM_TYPES];
};
extern CResourceSystem		g_ResourceSystem;

#endif // !RESOURCESYSTEM_H