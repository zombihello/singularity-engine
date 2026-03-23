#pragma once
#include "resourcesystem/iresourcetype.h"

//-----------------------------------------------------------------------------
// Texture factory
//-----------------------------------------------------------------------------
class CTextureFactory : public IResourceTypeFactory
{
public:
	// IResourceTypeFactory interface
	virtual void* Create() const override;
	virtual void  Delete( void* pData ) const override;
};
