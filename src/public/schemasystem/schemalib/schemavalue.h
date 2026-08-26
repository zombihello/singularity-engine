#pragma once
#include "tier0/types.h"
#include "tier1/stringid.h"
#include "tier1/math/math.h"
#include "tier1/math/rotator.h"
#include "tier1/math/color.h"
#include "tier1/math/transform.h"
#include "schemasystem/schematypes.h"
#include "schemasystem/schemalib/schemacheck.h"

//-----------------------------------------------------------------------------
// How many values a described member holds
//-----------------------------------------------------------------------------
enum schemaMemberShape_t
{
	SCHEMA_MEMBER_SHAPE_SINGLE,		  // `Type`
	SCHEMA_MEMBER_SHAPE_FIXED_ARRAY,  // `Type[N]`
	SCHEMA_MEMBER_SHAPE_ARRAY		  // `eastl::vector<Type>`
};

//-----------------------------------------------------------------------------
// Maps a described member type onto its shape and its element type
//-----------------------------------------------------------------------------
template<typename TType>
struct schemaShapeTraits_t
{
	using element_t = TType;
	enum
	{
		SHAPE			   = SCHEMA_MEMBER_SHAPE_SINGLE,
		NUM_FIXED_ELEMENTS = 1
	};
};

template<typename TType, size numElements>
struct schemaShapeTraits_t<TType[numElements]>
{
	using element_t = TType;
	enum
	{
		SHAPE			   = SCHEMA_MEMBER_SHAPE_FIXED_ARRAY,
		NUM_FIXED_ELEMENTS = (uint32)numElements
	};
};

template<typename TType, typename TAllocator>
struct schemaShapeTraits_t<eastl::vector<TType, TAllocator>>
{
	using element_t = TType;
	enum
	{
		SHAPE			   = SCHEMA_MEMBER_SHAPE_ARRAY,
		NUM_FIXED_ELEMENTS = 0
	};
};

//-----------------------------------------------------------------------------
// What a described enum carries besides its values
//-----------------------------------------------------------------------------
template<typename TEnum>
struct schemaEnumInfo_t
{
	enum
	{
		FLAGS	 = SCHEMA_TYPE_FLAG_NONE,
		DECLARED = 0
	};

	static const char*		  GetName();
	static const ISchemaEnum* GetType();
};

//-----------------------------------------------------------------------------
// Maps a described element type onto its field type
//-----------------------------------------------------------------------------
template<typename TType, typename = void>
struct schemaValueTraits_t
{
	enum
	{
		TYPE = SCHEMA_FIELD_TYPE_NONE
	};

	static const char* GetTypeName();
};

#define SCHEMA_DECLARE_VALUE_TRAITS( ValueType, FieldType ) \
	template<>                                              \
	struct schemaValueTraits_t<ValueType, void>             \
	{                                                       \
		enum                                                \
		{                                                   \
			TYPE = FieldType                                \
		};                                                  \
		static const char* GetTypeName()                    \
		{                                                   \
			return "";                                      \
		}                                                   \
	}

// Number types
SCHEMA_DECLARE_VALUE_TRAITS( bool, SCHEMA_FIELD_TYPE_BOOL );
SCHEMA_DECLARE_VALUE_TRAITS( int8, SCHEMA_FIELD_TYPE_INT8 );
SCHEMA_DECLARE_VALUE_TRAITS( int16, SCHEMA_FIELD_TYPE_INT16 );
SCHEMA_DECLARE_VALUE_TRAITS( int32, SCHEMA_FIELD_TYPE_INT32 );
SCHEMA_DECLARE_VALUE_TRAITS( int64, SCHEMA_FIELD_TYPE_INT64 );
SCHEMA_DECLARE_VALUE_TRAITS( uint8, SCHEMA_FIELD_TYPE_UINT8 );
SCHEMA_DECLARE_VALUE_TRAITS( uint16, SCHEMA_FIELD_TYPE_UINT16 );
SCHEMA_DECLARE_VALUE_TRAITS( uint32, SCHEMA_FIELD_TYPE_UINT32 );
SCHEMA_DECLARE_VALUE_TRAITS( uint64, SCHEMA_FIELD_TYPE_UINT64 );
SCHEMA_DECLARE_VALUE_TRAITS( float, SCHEMA_FIELD_TYPE_FLOAT );
SCHEMA_DECLARE_VALUE_TRAITS( double, SCHEMA_FIELD_TYPE_DOUBLE );

// Math types
SCHEMA_DECLARE_VALUE_TRAITS( vector2_t, SCHEMA_FIELD_TYPE_VECTOR2 );
SCHEMA_DECLARE_VALUE_TRAITS( vector3_t, SCHEMA_FIELD_TYPE_VECTOR3 );
SCHEMA_DECLARE_VALUE_TRAITS( vector4_t, SCHEMA_FIELD_TYPE_VECTOR4 );
SCHEMA_DECLARE_VALUE_TRAITS( vector2i_t, SCHEMA_FIELD_TYPE_VECTOR2I );
SCHEMA_DECLARE_VALUE_TRAITS( vector3i_t, SCHEMA_FIELD_TYPE_VECTOR3I );
SCHEMA_DECLARE_VALUE_TRAITS( vector4i_t, SCHEMA_FIELD_TYPE_VECTOR4I );
SCHEMA_DECLARE_VALUE_TRAITS( matrix3x3_t, SCHEMA_FIELD_TYPE_MATRIX3X3 );
SCHEMA_DECLARE_VALUE_TRAITS( matrix4x4_t, SCHEMA_FIELD_TYPE_MATRIX4X4 );
SCHEMA_DECLARE_VALUE_TRAITS( matrix3x4_t, SCHEMA_FIELD_TYPE_MATRIX3X4 );
SCHEMA_DECLARE_VALUE_TRAITS( matrix4x3_t, SCHEMA_FIELD_TYPE_MATRIX4X3 );
SCHEMA_DECLARE_VALUE_TRAITS( quaternion_t, SCHEMA_FIELD_TYPE_QUATERNION );
SCHEMA_DECLARE_VALUE_TRAITS( CRotator, SCHEMA_FIELD_TYPE_ROTATOR );
SCHEMA_DECLARE_VALUE_TRAITS( CTransform, SCHEMA_FIELD_TYPE_TRANSFORM );
SCHEMA_DECLARE_VALUE_TRAITS( CColor, SCHEMA_FIELD_TYPE_COLOR );

// String types
SCHEMA_DECLARE_VALUE_TRAITS( CStringID, SCHEMA_FIELD_TYPE_STRINGID );
SCHEMA_DECLARE_VALUE_TRAITS( eastl::string, SCHEMA_FIELD_TYPE_STRING );
SCHEMA_DECLARE_VALUE_TRAITS( const char*, SCHEMA_FIELD_TYPE_CSTRING );
#undef SCHEMA_DECLARE_VALUE_TRAITS

// An enum
template<typename TType>
struct schemaValueTraits_t<TType, typename eastl::enable_if<eastl::is_enum<TType>::value>::type>
{
	enum
	{
		TYPE = SCHEMA_FIELD_TYPE_ENUM
	};

	static const char* GetTypeName();
};

// An embedded described struct
template<typename TType>
struct schemaValueTraits_t<TType, typename eastl::enable_if<schemaIsStruct_t<TType>::value>::type>
{
	enum
	{
		TYPE = SCHEMA_FIELD_TYPE_STRUCT
	};

	static const char* GetTypeName();
};

// A pointer to a described object
template<typename TType>
struct schemaValueTraits_t<TType*, typename eastl::enable_if<schemaIsClass_t<TType>::value>::type>
{
	enum
	{
		TYPE = SCHEMA_FIELD_TYPE_OBJECTPTR
	};

	static const char* GetTypeName();
};

//-----------------------------------------------------------------------------
// What a described member holds, without ever spelling a dependent type name
//-----------------------------------------------------------------------------
template<typename TMember>
struct schemaMemberInfo_t
{
	using element_t = typename schemaShapeTraits_t<TMember>::element_t;
	enum
	{
		TYPE  = schemaValueTraits_t<element_t>::TYPE,
		SHAPE = schemaShapeTraits_t<TMember>::SHAPE
	};

	// The type this member refers to BY NAME, "" when it refers to none
	static const char* GetTypeName();
};

//-----------------------------------------------------------------------------
// Maps an enum onto the field type of its underlying integer
//-----------------------------------------------------------------------------
template<typename TEnum>
struct schemaEnumTraits_t
{
	using underlying_t = typename eastl::underlying_type<TEnum>::type;
	enum
	{
		TYPE = schemaValueTraits_t<underlying_t>::TYPE
	};
};

#include "schemasystem/schemalib/schemavalue.inl"
