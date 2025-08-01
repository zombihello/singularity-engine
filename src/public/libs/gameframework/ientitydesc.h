#pragma once
#include "stdlib/refcount.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IEntity;
class IMap;


//-----------------------------------------------------------------------------
// Entity descriptor interface
// By this you can create an entity
//-----------------------------------------------------------------------------
class IEntityDesc : public IRefCounted
{
public:
	virtual ~IEntityDesc() {}
	virtual IEntity* Create( IMap* pMap, const achar* pName = "" ) const = 0;
};
