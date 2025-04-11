/**
 * @file
 * @addtogroup resourcesystem resourcesystem
 */

#ifndef RESOURCESYSTEM_H
#define RESOURCESYSTEM_H

#include <unordered_map>

#include "resourcesystem/iresourcesystem.h"
#include "resourcesystem/resource.h"

/**
 * @ingroup resourcesystem
 * @brief Resource system
 */
class CResourceSystem : public CBaseAppSystem<IResourceSystem>
{
public:
	/**
	 * @brief Connect application system
	 *
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes application system is connected, otherwise return FALSE
	 */
	virtual bool Connect( createInterfaceFn_t pFactory ) override;

	/**
	 * @brief Disconnect application system
	 */
	virtual void Disconnect() override;

	/**
	 * @brief Init application system
	 * @return Return TRUE if application system is inited
	 */
	virtual bool Init() override;

	/**
	 * @brief Shutdown application system
	 */
	virtual void Shutdown() override;

	/**
	 * @brief Register a resource factory
	 * The function register a new resource factory and reset a default resource for the resource type
	 *
	 * @param type		Resource type
	 * @param pFactory	Resource factory
	 */
	virtual void RegisterResourceFactory( resourceType_t type, IResourceFactory* pFactory ) override;

	/**
	 * @brief Unregister a resource factory
	 * The function unregister a resource factory and reset a default resource for the resource type
	 *
	 * @param type		Resource type
	 */
	virtual void UnRegisterResourceFactory( resourceType_t type ) override;

	/**
	 * @brief Create a procedural resource
	 * @param pName		Resource name
	 * @param type		Resource type
	 * @return Return pointer to the created resource. If resource type isn't registered return NULL
	 */
	virtual TRefPtr<IResource> CreateProceduralResource( const achar* pName, resourceType_t type ) override;

	/**
	 * @brief Find or load a resource
	 * @param pPath		The path to the resource in the file system (can be without file extension), or its name if it is a procedural resource
	 * @param type		Resource type
	 * @param loadFlags	Load flags (see resourceLoadFlag_t)
	 * @return Return pointer to the loaded resource. In a fail case return the default resource if exist, otherwise NULL
	 */
	virtual TRefPtr<IResource> FindOrLoadResource( const achar* pPath, resourceType_t type, uint32 loadFlags = RESOURCE_LOAD_FLAG_NONE ) override;

	/**
	 * @brief Remove unused resources
	 * This function delete any resource that has a refcount <= 1 (one reference in the resource system)
	 */
	virtual void RemoveUnusedResources() override;

	/**
	 * @brief Get a default resource by its type
	 * @param type		Resource type
	 * @return Return a default resource by its type. If the type isn't registered or not have a default resource return NULL
	 */
	virtual TRefPtr<IResource> GetDefaultResource( resourceType_t type ) const override;

private:
	IResourceFactory*										pResourceFactories[RESOURCE_NUM_TYPES];	/**< Resource factory for each type */
	std::unordered_map<std::string, TRefPtr<CResource>>		resourcesDicts[RESOURCE_NUM_TYPES];		/**< Dictionary of resources for each type */
};

/**
 * @ingroup resourcesystem
 * @brief Resource system
 */
extern CResourceSystem		g_ResourceSystem;

#endif // !RESOURCESYSTEM_H