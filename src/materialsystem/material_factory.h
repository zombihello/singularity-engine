#ifndef MATERIAL_FACTORY_H
#define MATERIAL_FACTORY_H

#include "resourcesystem/iresourcefactory.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CMaterial;


//-----------------------------------------------------------------------------
// Material factory
// Path to the resource in the file system can be without file extension
//-----------------------------------------------------------------------------
class CMaterialFactory : public IResourceFactory
{
public:
	void Init();
	void Shutdown();

	// IResourceFactory interface
	virtual TRefPtr<IRefCounted> CreateProceduralResource() const override;
	virtual TRefPtr<IRefCounted> LoadResource( const achar* pPath, uint32 loadFlags = RESOURCE_LOAD_FLAG_NONE ) const override;
	virtual void UnloadResource( IRefCounted* pResoruce ) const override;
	virtual TRefPtr<IResource> GetDefaultResource() const override;
	virtual const achar* GetFormatType() const override;

	// Ensures that the factory is valid, because one can depends on specific format type.
	// If something wrong the function must crash the game
	virtual void Validate() const override;

private:
	TRefPtr<IResource>		pDefaultMaterial;
};

#endif // !MATERIAL_FACTORY_H