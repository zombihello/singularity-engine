#pragma once
#include "tier1/refcount.h"

//-----------------------------------------------------------------------------
// Entity descriptor interface
// By this you can create an entity
//-----------------------------------------------------------------------------
class IEntityDesc : public IRefCounted
{
public:
	virtual ~IEntityDesc() {}
};
