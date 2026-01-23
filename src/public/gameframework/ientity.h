#pragma once
#include "stdlib/refcount.h"

//-----------------------------------------------------------------------------
// Entity interface
//-----------------------------------------------------------------------------
class IEntity : public IRefCounted
{
public:
	virtual ~IEntity() {}
	virtual void Destroy()		 = 0;
	virtual bool IsValid() const = 0;
};
