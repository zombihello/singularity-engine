#pragma once
#include "resourcesystem/iresourcefactory.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CTexture;

//-----------------------------------------------------------------------------
// Texture factory
// Path to the resource in the file system can be without file extension
//-----------------------------------------------------------------------------
class CTextureFactory : public CBaseResourceFactory<IResourceFactory>
{
public:
	void Init();
	void Shutdown();

	// IResourceFactory interface
	virtual CRefPtr<IRefCounted> CreateProceduralResource() const override;
	virtual CRefPtr<IRefCounted> LoadResource( const char* pPath, uint32 loadFlags = RESOURCE_LOAD_FLAG_NONE ) const override;
	virtual void				 UnloadResource( IRefCounted* pResoruce ) const override;
	virtual CRefPtr<IResource>	 GetDefaultResource() const override;
	virtual const char*		 GetFormatType() const override;

private:
	CRefPtr<IResource> pDefaultTexture;
};