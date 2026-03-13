#pragma once
#include "tier1/refcount.h"
#include "appframework/iappsystem.h"
#include "resourcesystem/iresource.h"
#include "resourcesystem/iresourcefactory.h"

//-----------------------------------------------------------------------------
// Resource system interface
//-----------------------------------------------------------------------------
#define RESOURCESYSTEM_INTERFACE_VERSION "SResourceSystem001"
class IResourceSystem : public IAppSystem
{
public:
	// Functions register/unregister a resource factory
	// NOTE: You can't override a resource factory if the one has RESOURCE_FACTORY_FLAG_STATIC (except for the case if the factory has RESOURCE_FACTORY_FLAG_NOT_USED)
	virtual bool RegisterResourceFactory( resourceType_t type, IResourceFactory* pFactory ) = 0;
	virtual bool UnRegisterResourceFactory( resourceType_t type )							= 0;

	// NOTE: The path to the resource in the file system can be without file extension, or its name if it is a procedural resource
	virtual CRefPtr<IResource> FindOrLoadResource( const char* pPath, resourceType_t type, uint32 loadFlags = RESOURCE_LOAD_FLAG_NONE ) = 0;
	virtual CRefPtr<IResource> CreateProceduralResource( const char* pName, resourceType_t type )										 = 0;

	// This function delete any resource that has a refcount <= 1 (one reference in the resource system)
	virtual void RemoveUnusedResources() = 0;

	// Return a default resource by its type. If the type isn't registered or not have a default resource return NULL
	virtual CRefPtr<IResource> GetDefaultResource( resourceType_t type ) const = 0;

	virtual bool			  HasResourceFactory( resourceType_t type ) const = 0;
	virtual IResourceFactory* GetResourceFactory( resourceType_t type ) const = 0;
};