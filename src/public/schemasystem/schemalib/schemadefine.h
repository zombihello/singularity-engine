#pragma once
#include "schemasystem/schemalib/schema.h"
#include "schemasystem/schemalib/schemavalue.h"
#include "schemasystem/schemalib/schemaops.h"
#include "schemasystem/schemalib/schemacheck.h"
#include "schemasystem/schemalib/schemabuilder.h"
#include "schemasystem/schemalib/schemalink.h"

//-----------------------------------------------------------------------------
// Flags a type gets from what it IS, on top of the ones it was declared with
//-----------------------------------------------------------------------------
#define SCHEMA_TYPE_FLAGS( Type )  ( Type::SCHEMA_STATIC_TYPE_FLAGS | ( schemaObjectMemberOps_t<Type>::CAN_CONSTRUCT ? 0 : SCHEMA_TYPE_FLAG_ABSTRACT ) )
#define SCHEMA_CLASS_FLAGS( Type ) ( SCHEMA_TYPE_FLAGS( Type ) | ( eastl::is_base_of<IRefCounted, Type>::value ? SCHEMA_TYPE_FLAG_REFCOUNTED : 0 ) )

//-----------------------------------------------------------------------------
// Describing a type - goes into exactly one source file
//-----------------------------------------------------------------------------
#define BEGIN_SCHEMA_ENUM( Enum )                                                                                           \
	static void				Schema_RegisterEnum_##Enum( schemaScopeId_t scopeId );                                          \
	static schemaTypeLink_t s_schemaLink_##Enum( &Schema_RegisterEnum_##Enum );                                             \
	static void				Schema_RegisterEnum_##Enum( schemaScopeId_t scopeId )                                           \
	{                                                                                                                       \
		using schemaThisType_t = Enum;                                                                                      \
		static_assert( eastl::is_enum<Enum>::value, "'" #Enum "' is described as a schema enum but it is not a C++ enum" ); \
		CSchemaEnumBuilder schema;                                                                                          \
		schema.SetName( #Enum );

#define END_SCHEMA_ENUM()                                                                      \
	schema.SetFlags( schemaEnumInfo_t<schemaThisType_t>::FLAGS );                              \
	schema.SetSize( sizeof( schemaThisType_t ) );                                              \
	schema.SetAlignment( alignof( schemaThisType_t ) );                                        \
	schema.SetUnderlyingType( (schemaFieldType_t)schemaEnumTraits_t<schemaThisType_t>::TYPE ); \
	schema.Register( scopeId );                                                                \
	}

#define BEGIN_SCHEMA_INTERFACE( Interface )                                                                                                           \
	static void				Schema_RegisterInterface_##Interface( schemaScopeId_t scopeId );                                                          \
	static schemaTypeLink_t s_schemaLink_##Interface( &Schema_RegisterInterface_##Interface );                                                        \
	static void				Schema_RegisterInterface_##Interface( schemaScopeId_t scopeId )                                                           \
	{                                                                                                                                                 \
		using schemaThisType_t = Interface;                                                                                                           \
		static_assert( schemaIsInterface_t<Interface>::value, "'" #Interface "' is not declared as a schema interface" );                             \
		static_assert( schemaBaseIsInterface_t<Interface::baseInterface_t>::value, "The base of '" #Interface "' is not a schema interface" );        \
		static_assert( schemaBaseIsCppBase_t<Interface, Interface::baseInterface_t>::value, "The base of '" #Interface "' is not a C++ base of it" ); \
		CSchemaInterfaceBuilder schema;                                                                                                               \
		schema.SetName( #Interface );

#define END_SCHEMA_INTERFACE()                                                                           \
	schema.SetFlags( schemaThisType_t::SCHEMA_STATIC_TYPE_FLAGS );                                       \
	schema.SetBaseType( schemaBaseInfo_t<schemaThisType_t, schemaThisType_t::baseInterface_t>::Name() ); \
	schema.Register( scopeId );                                                                          \
	}

#define BEGIN_SCHEMA_STRUCT( Struct )                                                                                                     \
	template<>                                                                                                                            \
	struct schemaBuilder_t<Struct>                                                                                                        \
	{                                                                                                                                     \
		static void Register( schemaScopeId_t scopeId );                                                                                  \
	};                                                                                                                                    \
	static schemaTypeLink_t s_schemaLink_##Struct( &schemaBuilder_t<Struct>::Register );                                                  \
	void					schemaBuilder_t<Struct>::Register( schemaScopeId_t scopeId )                                                  \
	{                                                                                                                                     \
		using schemaThisType_t = Struct;                                                                                                  \
		static_assert( schemaIsStruct_t<Struct>::value, "'" #Struct "' is not declared as a schema struct" );                             \
		static_assert( !eastl::is_polymorphic<Struct>::value, "'" #Struct "' has a vtable, which a schema struct may not" );              \
		static_assert( schemaBaseIsStruct_t<Struct::baseStruct_t>::value, "The base of '" #Struct "' is not a schema struct" );           \
		static_assert( schemaBaseIsCppBase_t<Struct, Struct::baseStruct_t>::value, "The base of '" #Struct "' is not a C++ base of it" ); \
		CSchemaStructBuilder schema;                                                                                                      \
		schema.SetName( #Struct );

#define END_SCHEMA_STRUCT()                                                                                                                                                         \
	schema.SetFlags( SCHEMA_TYPE_FLAGS( schemaThisType_t ) );                                                                                                                       \
	schema.SetSize( sizeof( schemaThisType_t ) );                                                                                                                                   \
	schema.SetAlignment( alignof( schemaThisType_t ) );                                                                                                                             \
	schema.SetBaseType( schemaBaseInfo_t<schemaThisType_t, schemaThisType_t::baseStruct_t>::Name(), schemaBaseInfo_t<schemaThisType_t, schemaThisType_t::baseStruct_t>::Offset() ); \
	schema.SetObjectOps( Schema_GetObjectOps<schemaThisType_t>() );                                                                                                                 \
	schema.Register( scopeId );                                                                                                                                                     \
	}

#define BEGIN_SCHEMA_CLASS( Class )                                                                                                   \
	template<>                                                                                                                        \
	struct schemaBuilder_t<Class>                                                                                                     \
	{                                                                                                                                 \
		static void Register( schemaScopeId_t scopeId );                                                                              \
	};                                                                                                                                \
	static schemaTypeLink_t s_schemaLink_##Class( &schemaBuilder_t<Class>::Register );                                                \
	void					schemaBuilder_t<Class>::Register( schemaScopeId_t scopeId )                                               \
	{                                                                                                                                 \
		using schemaThisType_t = Class;                                                                                               \
		static_assert( schemaIsClass_t<Class>::value, "'" #Class "' is not declared as a schema class" );                             \
		static_assert( eastl::is_base_of<ISchemaObject, Class>::value, "'" #Class "' does not derive from ISchemaObject" );           \
		static_assert( schemaBaseIsClass_t<Class::baseClass_t>::value, "The base of '" #Class "' is not a schema class" );            \
		static_assert( schemaBaseIsCppBase_t<Class, Class::baseClass_t>::value, "The base of '" #Class "' is not a C++ base of it" ); \
		CSchemaClassBuilder schema;                                                                                                   \
		schema.SetName( #Class );

#define END_SCHEMA_CLASS()                                                                                                                                                        \
	schema.SetFlags( SCHEMA_CLASS_FLAGS( schemaThisType_t ) );                                                                                                                    \
	schema.SetSize( sizeof( schemaThisType_t ) );                                                                                                                                 \
	schema.SetAlignment( alignof( schemaThisType_t ) );                                                                                                                           \
	schema.SetBaseType( schemaBaseInfo_t<schemaThisType_t, schemaThisType_t::baseClass_t>::Name(), schemaBaseInfo_t<schemaThisType_t, schemaThisType_t::baseClass_t>::Offset() ); \
	schema.SetObjectOffset( Schema_CastOffset<schemaThisType_t, ISchemaObject>() );                                                                                               \
	schema.SetObjectOps( Schema_GetObjectOps<schemaThisType_t>() );                                                                                                               \
	schema.Register( scopeId );                                                                                                                                                   \
	}

//-----------------------------------------------------------------------------
// Describing a type
//-----------------------------------------------------------------------------
#define SCHEMA_IMPLEMENTS( Interface )                                                                                                              \
	static_assert( schemaIsInterface_t<Interface>::value, "'" #Interface "' is implemented as a schema interface but it is not described as one" ); \
	schema.AddInterface( #Interface, Schema_CastOffset<schemaThisType_t, Interface>() )

#define SCHEMA_ALIAS( AliasName )					schema.SetAliasName( AliasName )
#define SCHEMA_ENUM_VALUE( Value )					schema.AddValue( #Value, "", Value )
#define SCHEMA_ENUM_VALUE_ALIAS( Value, AliasName ) schema.AddValue( #Value, AliasName, Value )

#if ENABLE_SCHEMA_METADATA
	#define SCHEMA_METADATA( Key, Value )		schema.AddMetadata( Key, Value )
	#define SCHEMA_FIELD_METADATA( Key, Value ) schema.AddFieldMetadata( Key, Value )
#else
	#define SCHEMA_METADATA( Key, Value )
	#define SCHEMA_FIELD_METADATA( Key, Value )
#endif	// ENABLE_SCHEMA_METADATA

//-----------------------------------------------------------------------------
// Describing a field
//-----------------------------------------------------------------------------
#define SCHEMA_FIELD_ALIAS( Member, AliasName, Flags )                                                                                                                                                                                        \
	static_assert( schemaMemberInfo_t<decltype( schemaThisType_t::Member )>::TYPE != SCHEMA_FIELD_TYPE_NONE, "The field '" #Member "' is not a type the schema can describe - declare it with DECLARE_SCHEMA_* or use SCHEMA_FIELD_CUSTOM" ); \
	Schema_AddValueField<decltype( schemaThisType_t::Member )>( schema, #Member, AliasName, OFFSET_OF( schemaThisType_t, Member ), Flags )

#define SCHEMA_FIELD( Member, Flags )									 SCHEMA_FIELD_ALIAS( Member, "", Flags )
#define SCHEMA_FIELD_CUSTOM_ALIAS( Member, CustomOps, AliasName, Flags ) Schema_AddCustomField<decltype( schemaThisType_t::Member )>( schema, #Member, AliasName, OFFSET_OF( schemaThisType_t, Member ), Flags, &CustomOps )
#define SCHEMA_FIELD_CUSTOM( Member, CustomOps, Flags )					 SCHEMA_FIELD_CUSTOM_ALIAS( Member, CustomOps, "", Flags )
