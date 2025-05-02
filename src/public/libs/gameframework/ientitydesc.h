#ifndef IENTITYDESC_H
#define IENTITYDESC_H

#include "stdlib/refcount.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IEntity;


//-----------------------------------------------------------------------------
// Entity descriptor interface
// By this you can create an entity
//-----------------------------------------------------------------------------
class IEntityDesc : public IRefCounted
{
public:
	virtual ~IEntityDesc() {}
	virtual IEntity* Create( const achar* pName = "" ) const = 0;
};

#endif // !IENTITYDESC_H
