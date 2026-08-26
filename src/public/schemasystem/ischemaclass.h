#pragma once
#include "schemasystem/ischemacompoundtype.h"

//-----------------------------------------------------------------------------
// A described class
//
// NOTES:
//	* The base of a class is always a class
//	* Where a struct is a value, a class is an object - it can be pointed at, and
//	  it knows which class it really is through its `ISchemaObject` subobject.
//	  The vtable and the interfaces it may implement are what that costs
//	* There are TWO lifetime pairs here and they must not be mixed:
//		- `New`/`Delete`: own the allocation and do it inside the module that describes the class
//		- `Construct`/`Destruct`: place into memory the CALLER owns. A refcounted class
//		  only supports the first - it frees itself when its last reference goes
//-----------------------------------------------------------------------------
class ISchemaClass : public ISchemaCompoundType
{
public:
	using ISchemaCompoundType::Cast;

	// Construct/destruct an object of this class in memory the caller owns
	// NOTE: `pMemory` must be at least `GetSize()` bytes and `GetAlignment()` aligned
	virtual ISchemaObject* Construct( void* pMemory ) const = 0;
	virtual void		   Destruct( void* pObject ) const	= 0;

	// Allocate and construct/destruct and free an object of this class
	virtual ISchemaObject* New() const							  = 0;
	virtual void		   Delete( ISchemaObject* pObject ) const = 0;

	// Casts an object to the given class or interface, applying the pointer
	// adjustment recorded for every step. Returns NULL when the object is not a
	// `pType`
	virtual void* Cast( ISchemaObject* pObject, const ISchemaType* pType ) const = 0;
	const void*	  Cast( const ISchemaObject* pObject, const ISchemaType* pType ) const;

	// Interfaces implemented by this class
	virtual uint32					GetNumInterfaces( bool bWithParents = false ) const			  = 0;
	virtual const ISchemaInterface* GetInterface( uint32 index, bool bWithParents = false ) const = 0;

	// The class this one derives from, NULL when there is none
	const ISchemaClass* GetBaseClass() const;
};

#include "schemasystem/ischemaclass.inl"
