#pragma once
#include "resourcesystem/iresourcetype.h"

//-----------------------------------------------------------------------------
// ECS entity descriptor factory
//-----------------------------------------------------------------------------
class CEcsEntityDescFactory : public IResourceTypeFactory
{
public:
	// IResourceTypeFactory interface
	virtual void* Create() const override;
	virtual void  Delete( void* pData ) const override;
};
