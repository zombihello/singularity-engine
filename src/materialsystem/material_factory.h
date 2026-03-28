#pragma once
#include "resourcesystem/iresourcetype.h"

//-----------------------------------------------------------------------------
// Material factory
//-----------------------------------------------------------------------------
class CMaterialFactory : public IResourceTypeFactory
{
public:
	// IResourceTypeFactory interface
	virtual void* Create() const override;
	virtual void  Delete( void* pData ) const override;
};
