#pragma once
#include "stdlib/refcount.h"

//-----------------------------------------------------------------------------
// Entity descriptor interface
// By this you can create an entity
//-----------------------------------------------------------------------------
class IEntityDesc : public IRefCounted
{
public:
	virtual ~IEntityDesc() {}
};
