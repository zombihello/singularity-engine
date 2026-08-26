#pragma once
#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier1/math/math_types.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class ISchemaMetadata;
class ISchemaEnum;
class ISchemaCompoundType;
class ISchemaStruct;
class ISchemaClass;
class ISchemaInterface;
class ISchemaField;
class ISchemaObject;
class ISchemaDataReader;
class ISchemaDataWriter;

//-----------------------------------------------------------------------------
// Schema scope identifier
//-----------------------------------------------------------------------------
using schemaScopeId_t = uint32;

//-----------------------------------------------------------------------------
// Schema scope
//-----------------------------------------------------------------------------
struct schemaScope_t
{
	schemaScopeId_t id;
	const char*		pName;
};

//-----------------------------------------------------------------------------
// A kind of a described type
//-----------------------------------------------------------------------------
enum schemaTypeKind_t
{
	SCHEMA_TYPE_KIND_ENUM,
	SCHEMA_TYPE_KIND_STRUCT,
	SCHEMA_TYPE_KIND_CLASS,
	SCHEMA_TYPE_KIND_INTERFACE,
	SCHEMA_TYPE_NUM_KINDS
};

//-----------------------------------------------------------------------------
// What a described C++ member actually is
//-----------------------------------------------------------------------------
enum schemaFieldType_t
{
	SCHEMA_FIELD_TYPE_NONE,			// Unknown type
	SCHEMA_FIELD_TYPE_BOOL,			// A boolean value
	SCHEMA_FIELD_TYPE_INT8,			// An 8-bit integer
	SCHEMA_FIELD_TYPE_INT16,		// A 16-bit integer
	SCHEMA_FIELD_TYPE_INT32,		// A 32-bit integer
	SCHEMA_FIELD_TYPE_INT64,		// A 64-bit integer
	SCHEMA_FIELD_TYPE_UINT8,		// An 8-bit unsigned integer
	SCHEMA_FIELD_TYPE_UINT16,		// A 16-bit unsigned integer
	SCHEMA_FIELD_TYPE_UINT32,		// A 32-bit unsigned integer
	SCHEMA_FIELD_TYPE_UINT64,		// A 64-bit unsigned integer
	SCHEMA_FIELD_TYPE_FLOAT,		// A floating point value
	SCHEMA_FIELD_TYPE_DOUBLE,		// A double floating point value
	SCHEMA_FIELD_TYPE_ENUM,			// An enum type
	SCHEMA_FIELD_TYPE_VECTOR2,		// A vector with 2 float
	SCHEMA_FIELD_TYPE_VECTOR3,		// A vector with 3 float
	SCHEMA_FIELD_TYPE_VECTOR4,		// A vector with 4 float
	SCHEMA_FIELD_TYPE_VECTOR2I,		// A vector with 2 integer
	SCHEMA_FIELD_TYPE_VECTOR3I,		// A vector with 3 integer
	SCHEMA_FIELD_TYPE_VECTOR4I,		// A vector with 4 integer
	SCHEMA_FIELD_TYPE_MATRIX3X3,	// A matrix 3x3
	SCHEMA_FIELD_TYPE_MATRIX4X4,	// A matrix 4x4
	SCHEMA_FIELD_TYPE_MATRIX3X4,	// A matrix 3x4
	SCHEMA_FIELD_TYPE_MATRIX4X3,	// A matrix 4x3
	SCHEMA_FIELD_TYPE_QUATERNION,	// A quaternion
	SCHEMA_FIELD_TYPE_ROTATOR,		// A rotator (`CRotator`)
	SCHEMA_FIELD_TYPE_TRANSFORM,	// A transform (`CTransform`)
	SCHEMA_FIELD_TYPE_COLOR,		// A color (`CColor`)
	SCHEMA_FIELD_TYPE_STRINGID,		// A string id (`CStringID`)
	SCHEMA_FIELD_TYPE_STRING,		// A string (`eastl::string`)
	SCHEMA_FIELD_TYPE_CSTRING,		// A c-string (`const char*`). Read only through the schema
	SCHEMA_FIELD_TYPE_STRUCT,		// An embedded struct
	SCHEMA_FIELD_TYPE_OBJECTPTR,	// An object pointer (`ISchemaObject*`)
	SCHEMA_FIELD_TYPE_CUSTOM,		// An custom type
	SCHEMA_FIELD_TYPE_FIXED_ARRAY,	// An fixed array (`TType[N]`)
	SCHEMA_FIELD_TYPE_ARRAY,		// A dynamic array (`eastl::vector<TType>`)
	SCHEMA_FIELD_NUM_TYPES
};

//-----------------------------------------------------------------------------
// Flags of a described type
//-----------------------------------------------------------------------------
enum schemaTypeFlag_t
{
	SCHEMA_TYPE_FLAG_NONE		= 0,
	SCHEMA_TYPE_FLAG_ABSTRACT	= BIT( 0 ),	 // Abstract and cannot be constructed
	SCHEMA_TYPE_FLAG_DEPRECATED = BIT( 1 ),	 // Still works, but is considered deprecated and may be removed soon
	SCHEMA_TYPE_FLAG_HIDDEN		= BIT( 2 ),	 // Not shown in an editor/tool
	SCHEMA_TYPE_FLAG_TRANSIENT	= BIT( 3 ),	 // Never serialized
	SCHEMA_TYPE_FLAG_REFCOUNTED = BIT( 4 )	 // Has `IRefCounted` interface
};

//-----------------------------------------------------------------------------
// Flags of one described object
//-----------------------------------------------------------------------------
enum schemaObjectFlag_t
{
	SCHEMA_OBJECT_FLAG_NONE		 = 0,
	SCHEMA_OBJECT_FLAG_TRANSIENT = BIT( 0 )	 // Never serialized
};

//-----------------------------------------------------------------------------
// Flags of a described field
//-----------------------------------------------------------------------------
enum schemaFieldFlag_t
{
	SCHEMA_FIELD_FLAG_NONE			  = 0,
	SCHEMA_FIELD_FLAG_SAVE			  = BIT( 0 ),  // Written into a save
	SCHEMA_FIELD_FLAG_KEY			  = BIT( 1 ),  // Can be set from content
	SCHEMA_FIELD_FLAG_EDITOR		  = BIT( 2 ),  // Shown in an editor/tool
	SCHEMA_FIELD_FLAG_EDITOR_READONLY = BIT( 3 ),  // Shown in an editor/tool, but cannot be changed there
	SCHEMA_FIELD_FLAG_DEPRECATED	  = BIT( 4 )   // Still works, but is considered deprecated and may be removed soon
};
