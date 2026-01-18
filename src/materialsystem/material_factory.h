#pragma once
#include "resourcesystem/iresourcefactory.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CMaterial;

//-----------------------------------------------------------------------------
// Material factory
// Path to the resource in the file system can be without file extension
//-----------------------------------------------------------------------------
class CMaterialFactory : public CBaseResourceFactory<IResourceFactory>
{
public:
	void Init();
	void Shutdown();

	// IResourceFactory interface
	virtual TRefPtr<IRefCounted> CreateProceduralResource() const override;
	virtual TRefPtr<IRefCounted> LoadResource( const char* pPath, uint32 loadFlags = RESOURCE_LOAD_FLAG_NONE ) const override;
	virtual void				 UnloadResource( IRefCounted* pResoruce ) const override;
	virtual TRefPtr<IResource>	 GetDefaultResource() const override;
	virtual const char*		 GetFormatType() const override;

private:
	TRefPtr<IResource> pDefaultMaterial;
};