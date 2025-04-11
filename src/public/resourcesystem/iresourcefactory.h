/**
 * @file
 * @addtogroup resourcesystem resourcesystem
 */

#ifndef IRESOURCEFACTORY_H
#define IRESOURCEFACTORY_H

#include "stdlib/refcount.h"

// Forward declarations
class IResource;

/**
 * @ingroup resourcesystem
 * @brief Resource load flag
 */
enum resourceLoadFlag_t
{
	RESOURCE_LOAD_FLAG_NONE			= 0,		/**< None */
	RESOURCE_LOAD_FLAG_ONLY_FIND	= 1 << 0	/**< Only try to find already exists a resource */
};

/**
 * @ingroup resourcesystem
 * @brief Resource factory interface
 */
class IResourceFactory
{
public:
	/**
	 * @brief Create a procedural resource
	 * @return Return pointer to the created resource
	 */
	virtual TRefPtr<IRefCounted> CreateProceduralResource() const = 0;

	/**
	 * @brief Load a resource
	 * @param pPath				Path to the resource in the file system (can be without file extension)
	 * @param loadFlags			Load flags (see resourceLoadFlag_t)
	 * @return Return pointer to the loaded resource. In a fail case return NULL
	 */
	virtual TRefPtr<IRefCounted> LoadResource( const achar* pPath, uint32 loadFlags = RESOURCE_LOAD_FLAG_NONE ) const = 0;

	/**
	 * @brief Unload the resource
	 * @param pResoruce			Resource to unload
	 */
	virtual void UnloadResource( IRefCounted* pResoruce ) const = 0;

	/**
	 * @brief Get the default resource
	 * @return Return the default resource
	 */
	virtual TRefPtr<IResource> GetDefaultResource() const = 0;
};

#endif // !IRESOURCEFACTORY_H