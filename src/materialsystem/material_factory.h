/**
 * @file
 * @addtogroup materialsystem materialsystem
 */

#ifndef MATERIAL_FACTORY_H
#define MATERIAL_FACTORY_H

#include "resourcesystem/iresourcefactory.h"

// Forward declarations
class CMaterial;

/**
 * @ingroup resourcesystem
 * @brief Material factory
 */
class CMaterialFactory : public IResourceFactory
{
public:
	/**
	 * @brief Initialize the factory
	 */
	void Init();

	/**
	 * @brief Shutdown the factory
	 */
	void Shutdown();

	/**
	 * @brief Create a procedural resource
	 * @return Return pointer to the created resource
	 */
	virtual TRefPtr<IRefCounted> CreateProceduralResource() const override;

	/**
	 * @brief Load a resource
	 * @param pPath				Path to the resource in the file system (can be without file extension)
	 * @param loadFlags			Load flags (see resourceLoadFlag_t)
	 * @return Return pointer to the loaded resource. In a fail case return NULL
	 */
	virtual TRefPtr<IRefCounted> LoadResource( const achar* pPath, uint32 loadFlags = RESOURCE_LOAD_FLAG_NONE ) const override;

	/**
	 * @brief Unload the resource
	 * @param pResoruce			Resource to unload
	 */
	virtual void UnloadResource( IRefCounted* pResoruce ) const override;

	/**
	 * @brief Get the default resource
	 * @return Return the default resource
	 */
	virtual TRefPtr<IResource> GetDefaultResource() const override;

private:
	TRefPtr<IResource>		pDefaultMaterial;		/**< Default material */
};

#endif // !MATERIAL_FACTORY_H