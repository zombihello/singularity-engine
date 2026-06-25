#pragma once
#include "resourcesystem/iresourcetype.h"

//-----------------------------------------------------------------------------
// ECS entity descriptor loader
//-----------------------------------------------------------------------------
class CEcsEntityDescLoader : public IResourceTypeLoader
{
public:
	// IResourceTypeLoader interface
	virtual bool		Load( const char* pPath, IResourceData* pData ) const override;
	virtual const char* GetFormatName() const override;
};
