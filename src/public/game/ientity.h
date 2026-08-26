#pragma once
#include "tier1/refcount.h"
#include "schemasystem/schemalib/schema.h"

//-----------------------------------------------------------------------------
// Entity interface
//-----------------------------------------------------------------------------
class IEntity : public IRefCounted
{
	DECLARE_SCHEMA_INTERFACE_NOBASE( IEntity, SCHEMA_TYPE_FLAG_NONE )

public:
	virtual ~IEntity() {}
	virtual void Destroy()		 = 0;
	virtual bool IsValid() const = 0;
};
