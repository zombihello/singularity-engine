#pragma once
#include "stdlib/refcount.h"

//-----------------------------------------------------------------------------
// Entity interface
//-----------------------------------------------------------------------------
class IEntity : public IRefCounted
{
public:
	virtual ~IEntity() {}
};
