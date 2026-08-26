#pragma once
#include "schemasystem/ischematype.h"

//-----------------------------------------------------------------------------
// What a described struct and a described class have in common - a type that is
// LAID OUT: it owns fields and takes part in an inheritance chain
//
// NOTE: An inheritance chain NEVER mixes kinds - the base of a struct is a struct
//		 and the base of a class is a class. `GetBaseType` therefore always hands
//		 back a type of the same kind as this one
//-----------------------------------------------------------------------------
class ISchemaCompoundType : public ISchemaType
{
public:
	// Reads/writes/copies every field of this type and of all its base types
	virtual bool ReadData( void* pObject, ISchemaDataReader* pReader ) const		= 0;
	virtual bool WriteData( const void* pObject, ISchemaDataWriter* pWriter ) const = 0;
	virtual void Copy( void* pDestObject, const void* pSrcObject ) const			= 0;

	// Looks a field up by its name or by its alias name
	virtual const ISchemaField* FindField( const char* pName, bool bFindInParents = true ) const = 0;

	// Casts an object of this type to the given type, applying the pointer adjustment
	// recorded for every step. Returns NULL when the object is not a `pType`
	virtual void* Cast( void* pObject, const ISchemaType* pType ) const = 0;
	const void*	  Cast( const void* pObject, const ISchemaType* pType ) const;

	// Iterate over every field
	virtual uint32					   GetNumFields( bool bWithParents = false ) const			 = 0;
	virtual const ISchemaField*		   GetField( uint32 index, bool bWithParents = false ) const = 0;
	virtual const ISchemaCompoundType* GetBaseType() const										 = 0;
};

#include "schemasystem/ischemacompoundtype.inl"
