#pragma once
#include "resourcesystem/iresourcefactory.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CEcsEntityDesc;


//-----------------------------------------------------------------------------
// ECS entity descriptor factory
// Path to the resource in the file system can be without file extension
//-----------------------------------------------------------------------------
class CEcsEntityDescFactory : public CBaseResourceFactory<IResourceFactory, RESOURCE_FACTORY_FLAG_STATIC>
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

private:
	TRefPtr<IResource>		pDefaultEntityDesc;
};