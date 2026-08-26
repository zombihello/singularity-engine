#include "pch_schemasystem.h"
#include "schemasystem/schemautils.h"

// Table of schema type kind names
static const char* s_pSchemaTypeKindNames[] = {
	"enum",		 // SCHEMA_TYPE_KIND_ENUM
	"struct",	 // SCHEMA_TYPE_KIND_STRUCT
	"class",	 // SCHEMA_TYPE_KIND_CLASS
	"interface"	 // SCHEMA_TYPE_KIND_INTERFACE
};
static_assert( ARRAYSIZE( s_pSchemaTypeKindNames ) == SCHEMA_TYPE_NUM_KINDS, "Array size 's_pSchemaTypeKindNames' must be equal to SCHEMA_TYPE_NUM_KINDS" );

/*
==================
Schema_ConvTypeKindToString
==================
*/
const char* Schema_ConvTypeKindToString( schemaTypeKind_t typeKind )
{
	Assert( typeKind < SCHEMA_TYPE_NUM_KINDS );
	return s_pSchemaTypeKindNames[(uint32)typeKind];
}
