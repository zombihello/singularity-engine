#pragma once
#include "schemasystem/schematypes.h"

//-----------------------------------------------------------------------------
// A described type
//-----------------------------------------------------------------------------
class ISchemaType
{
public:
	virtual bool				   HasAllFlags( uint32 flags ) const = 0;
	virtual bool				   HasAnyFlags( uint32 flags ) const = 0;
	virtual schemaTypeKind_t	   GetKind() const					 = 0;
	virtual const char*			   GetName() const					 = 0;
	virtual const char*			   GetAliasName() const				 = 0;
	virtual uint32				   GetFlags() const					 = 0;  // see `schemaTypeFlag_t`
	virtual size				   GetSize() const					 = 0;
	virtual uint32				   GetAlignment() const				 = 0;
	virtual const ISchemaMetadata* GetMetadata() const				 = 0;  // NULL when metadata is compiled out

	// Is this type the given one or derived from it. A class also answers for every
	// interface it implements
	virtual bool IsA( const ISchemaType* pType ) const = 0;

	// Narrowing to the interface of one kind of type
	// NOTE: A miss gives NULL rather than a pointer to the wrong thing
	template<class TSchemaType>
	const TSchemaType* As() const;
};

//-----------------------------------------------------------------------------
// Which kinds of type answer for which interface
//-----------------------------------------------------------------------------
template<class TSchemaType>
struct schemaTypeNarrow_t;

#define SCHEMA_DECLARE_TYPE_NARROW( Interface, FirstKind, LastKind ) \
	template<>                                                       \
	struct schemaTypeNarrow_t<Interface>                             \
	{                                                                \
		enum                                                         \
		{                                                            \
			FIRST_KIND = FirstKind,                                  \
			LAST_KIND  = LastKind                                    \
		};                                                           \
	}

SCHEMA_DECLARE_TYPE_NARROW( ISchemaEnum, SCHEMA_TYPE_KIND_ENUM, SCHEMA_TYPE_KIND_ENUM );
SCHEMA_DECLARE_TYPE_NARROW( ISchemaCompoundType, SCHEMA_TYPE_KIND_STRUCT, SCHEMA_TYPE_KIND_CLASS );
SCHEMA_DECLARE_TYPE_NARROW( ISchemaStruct, SCHEMA_TYPE_KIND_STRUCT, SCHEMA_TYPE_KIND_STRUCT );
SCHEMA_DECLARE_TYPE_NARROW( ISchemaClass, SCHEMA_TYPE_KIND_CLASS, SCHEMA_TYPE_KIND_CLASS );
SCHEMA_DECLARE_TYPE_NARROW( ISchemaInterface, SCHEMA_TYPE_KIND_INTERFACE, SCHEMA_TYPE_KIND_INTERFACE );
#undef SCHEMA_DECLARE_TYPE_NARROW

#include "schemasystem/ischematype.inl"
