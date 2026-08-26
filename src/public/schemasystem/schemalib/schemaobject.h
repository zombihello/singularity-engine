#pragma once
#include "tier0/defines.h"
#include "schemasystem/ischemaobject.h"

//-----------------------------------------------------------------------------
// Mixin that implements `ISchemaObject`
//-----------------------------------------------------------------------------
template<class TBaseClass>
class CSchemaObject : public TBaseClass
{
	static_assert( eastl::is_base_of<ISchemaObject, TBaseClass>::value, "CSchemaObject<> takes ISchemaObject or an interface deriving from it" );

public:
	// ISchemaObject interface
	virtual void AddSchemaFlags( uint32 flags ) override;
	virtual void RemoveSchemaFlags( uint32 flags ) override;
	virtual void SetSchemaFlags( uint32 flags ) override;

	virtual bool   HasSchemaAllFlags( uint32 flags ) const override;
	virtual bool   HasSchemaAnyFlags( uint32 flags ) const override;
	virtual uint32 GetSchemaFlags() const override;

	CSchemaObject();

private:
	uint32 schemaFlags;
};

#include "schemasystem/schemalib/schemaobject.inl"
