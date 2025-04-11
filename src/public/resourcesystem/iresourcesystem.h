/**
 * @file
 * @addtogroup resourcesystem resourcesystem
 */

#ifndef IRESOURCESYSTEM_H
#define IRESOURCESYSTEM_H

#include "stdlib/refcount.h"
#include "appframework/iappsystem.h"
#include "resourcesystem/iresource.h"
#include "resourcesystem/iresourcefactory.h"

/**
 * @ingroup resourcesystem
 * @brief Resource system interface version
 */
#define RESOURCESYSTEM_INTERFACE_VERSION "SResourceSystem001"

/**
 * @ingroup resourcesystem
 * @brief Resource system interface
 */
class IResourceSystem : public IAppSystem
{
public:
	/**
	 * @brief Register a resource factory
	 * The function register a new resource factory and reset a default resource for the resource type
	 * 
	 * @param type		Resource type
	 * @param pFactory	Resource factory
	 */
	virtual void RegisterResourceFactory( resourceType_t type, IResourceFactory* pFactory ) = 0;

	/**
	 * @brief Unregister a resource factory
	 * The function unregister a resource factory and reset a default resource for the resource type
	 * 
	 * @param type		Resource type
	 */
	virtual void UnRegisterResourceFactory( resourceType_t type ) = 0;

	/**
	 * @brief Create a procedural resource
	 * @param pName		Resource name
	 * @param type		Resource type
	 * @return Return pointer to the created resource. If resource type isn't registered return NULL
	 */
	virtual TRefPtr<IResource> CreateProceduralResource( const achar* pName, resourceType_t type ) = 0;

	/**
	 * @brief Find or load a resource
	 * @param pPath		The path to the resource in the file system (can be without file extension), or its name if it is a procedural resource
	 * @param type		Resource type
	 * @param loadFlags	Load flags (see resourceLoadFlag_t)
	 * @return Return pointer to the loaded resource. In a fail case return the default resource if exist, otherwise NULL
	 */
	virtual TRefPtr<IResource> FindOrLoadResource( const achar* pPath, resourceType_t type, uint32 loadFlags = RESOURCE_LOAD_FLAG_NONE ) = 0;

	/**
	 * @brief Remove unused resources
	 * This function delete any resource that has a refcount <= 1 (one reference in the resource system)
	 */
	virtual void RemoveUnusedResources() = 0;

	/**
	 * @brief Get a default resource by its type
	 * @param type		Resource type
	 * @return Return a default resource by its type. If the type isn't registered or not have a default resource return NULL
	 */
	virtual TRefPtr<IResource> GetDefaultResource( resourceType_t type ) const = 0;
};

#endif // !IRESOURCESYSTEM_H