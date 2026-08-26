#pragma once
#include "schemasystem/schematypes.h"
#include "schemasystem/ischemaclass.h"

//-----------------------------------------------------------------------------
// An object that knows its own schema
//-----------------------------------------------------------------------------
class ISchemaObject
{
public:
	virtual ~ISchemaObject() {}

	virtual void AddSchemaFlags( uint32 flags )	   = 0;
	virtual void RemoveSchemaFlags( uint32 flags ) = 0;
	virtual void SetSchemaFlags( uint32 flags )	   = 0;

	// Is this object the given one or derived from it
	bool IsA( const ISchemaType* pType ) const;

	// Casts the object to the given type, applying the pointer adjustment
	// recorded for every step. Returns NULL when the object is not a `pType`
	void*		Cast( const ISchemaType* pType );
	const void* Cast( const ISchemaType* pType ) const;

	virtual bool				HasSchemaAllFlags( uint32 flags ) const = 0;
	virtual bool				HasSchemaAnyFlags( uint32 flags ) const = 0;
	virtual const ISchemaClass* GetSchemaClass() const					= 0;
	virtual uint32				GetSchemaFlags() const					= 0;  // see `schemaObjectFlag_t`

	// The address of the most derived object
	// NOTE: Every field operation takes the address of the object of the OWNING type.
	//		 When a class implements several interfaces, the `ISchemaObject` subobject is
	//		 not necessarily at that address, so never pass an `ISchemaObject*` straight
	//		 into a field - pass this instead
	virtual void* GetSchemaRawPtr() = 0;
	const void*	  GetSchemaRawPtr() const;
};

#include "schemasystem/ischemaobject.inl"
