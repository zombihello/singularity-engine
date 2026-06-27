#pragma once
#include "resourcesystem/iresourcetype.h"

//-----------------------------------------------------------------------------
// Texture loader
//-----------------------------------------------------------------------------
class CTextureLoader : public IResourceTypeLoader
{
public:
	// IResourceTypeLoader interface
	virtual bool		Load( const char* pPath, IResourceData* pData ) const override;
	virtual const char* GetFormatName() const override;
};
