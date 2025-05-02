#ifndef IENTITY_H
#define IENTITY_H

#include "stdlib/refcount.h"

//-----------------------------------------------------------------------------
// Entity interface
//-----------------------------------------------------------------------------
class IEntity : public IRefCounted
{
public:
	virtual ~IEntity() {}
};

#endif // !IENTITY_H
