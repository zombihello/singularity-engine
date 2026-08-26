#pragma once
#include <EASTL/atomic.h>

#include "schemasystem/ischemasystem.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
template<typename TType>
struct schemaBuilder_t;

template<typename TEnum>
struct schemaEnumInfo_t;

//-----------------------------------------------------------------------------
// Reaching a base type without the describing macro having to name it again
//-----------------------------------------------------------------------------
template<typename TType, typename TBase>
struct schemaBaseInfo_t
{
	static const char* Name();
	static ptrint	   Offset();
};

template<typename TType>
struct schemaBaseInfo_t<TType, void>
{
	static const char* Name();
	static ptrint	   Offset();
};

//-----------------------------------------------------------------------------
// One described type, looked up by name once and kept
//
// NOTE: Closing a scope destroys every type in it, so a pointer kept across one is
//		 dangling. The cache remembers the generation it resolved at and looks the
//		 type up again the moment the schema system moved past it
//-----------------------------------------------------------------------------
template<class TSchemaType>
struct schemaTypeCache_t
{
	schemaTypeCache_t( const char* pName );
	const TSchemaType* Get();

	const char*						  pName;
	eastl::atomic<const TSchemaType*> pType;
	eastl::atomic<uint32>			  generation;
};

//-----------------------------------------------------------------------------
// Declaring a described type
//
// NOTE: These only DECLARE. The matching `BEGIN_SCHEMA_*`/`END_SCHEMA_*` goes into
//		 exactly one source file, in the module that owns the type
//-----------------------------------------------------------------------------
#define DECLARE_SCHEMA_ENUM( Enum, Flags )                                \
	FORCEINLINE static const ISchemaEnum* Schema_GetEnum_##Enum()         \
	{                                                                     \
		static schemaTypeCache_t<ISchemaEnum> s_schemaEnumCache( #Enum ); \
		return s_schemaEnumCache.Get();                                   \
	}                                                                     \
	template<>                                                            \
	struct schemaEnumInfo_t<Enum>                                         \
	{                                                                     \
		enum                                                              \
		{                                                                 \
			FLAGS	 = Flags,                                             \
			DECLARED = 1                                                  \
		};                                                                \
		static const char* GetName()                                      \
		{                                                                 \
			return #Enum;                                                 \
		}                                                                 \
		static const ISchemaEnum* GetType()                               \
		{                                                                 \
			return Schema_GetEnum_##Enum();                               \
		}                                                                 \
	};

#define DECLARE_SCHEMA_INTERFACE( Interface, BaseInterface, Flags )                      \
public:                                                                                  \
	using thisInterface_t = Interface;                                                   \
	using baseInterface_t = BaseInterface;                                               \
	enum                                                                                 \
	{                                                                                    \
		SCHEMA_STATIC_TYPE_FLAGS = Flags                                                 \
	};                                                                                   \
	static const char* GetSchemaTypeNameStatic()                                         \
	{                                                                                    \
		return #Interface;                                                               \
	}                                                                                    \
	static const ISchemaInterface* GetSchemaTypeStatic()                                 \
	{                                                                                    \
		static schemaTypeCache_t<ISchemaInterface> s_schemaInterfaceCache( #Interface ); \
		return s_schemaInterfaceCache.Get();                                             \
	}

#define DECLARE_SCHEMA_STRUCT( Struct, BaseStruct, Flags )                      \
public:                                                                         \
	friend struct schemaBuilder_t<Struct>;                                      \
	using thisStruct_t = Struct;                                                \
	using baseStruct_t = BaseStruct;                                            \
	enum                                                                        \
	{                                                                           \
		SCHEMA_STATIC_TYPE_FLAGS = Flags                                        \
	};                                                                          \
	static const char* GetSchemaTypeNameStatic()                                \
	{                                                                           \
		return #Struct;                                                         \
	}                                                                           \
	static const ISchemaStruct* GetSchemaTypeStatic()                           \
	{                                                                           \
		static schemaTypeCache_t<ISchemaStruct> s_schemaStructCache( #Struct ); \
		return s_schemaStructCache.Get();                                       \
	}

#define DECLARE_SCHEMA_CLASS( Class, BaseClass, Flags )                      \
public:                                                                      \
	friend struct schemaBuilder_t<Class>;                                    \
	using thisClass_t = Class;                                               \
	using baseClass_t = BaseClass;                                           \
	enum                                                                     \
	{                                                                        \
		SCHEMA_STATIC_TYPE_FLAGS = Flags                                     \
	};                                                                       \
	static const char* GetSchemaTypeNameStatic()                             \
	{                                                                        \
		return #Class;                                                       \
	}                                                                        \
	static const ISchemaClass* GetSchemaTypeStatic()                         \
	{                                                                        \
		static schemaTypeCache_t<ISchemaClass> s_schemaClassCache( #Class ); \
		return s_schemaClassCache.Get();                                     \
	}                                                                        \
	virtual const ISchemaClass* GetSchemaClass() const override              \
	{                                                                        \
		return Class::GetSchemaTypeStatic();                                 \
	}                                                                        \
	virtual void* GetSchemaRawPtr() override                                 \
	{                                                                        \
		return (void*)this;                                                  \
	}

#define DECLARE_SCHEMA_INTERFACE_NOBASE( Interface, Flags ) DECLARE_SCHEMA_INTERFACE( Interface, void, Flags )
#define DECLARE_SCHEMA_STRUCT_NOBASE( Struct, Flags )		DECLARE_SCHEMA_STRUCT( Struct, void, Flags )
#define DECLARE_SCHEMA_CLASS_NOBASE( Class, Flags )			DECLARE_SCHEMA_CLASS( Class, void, Flags )

//-----------------------------------------------------------------------------
// The offset that has to be applied to go from a `TClass*` to a `TCastTo*`
//-----------------------------------------------------------------------------
template<typename TClass, typename TCastTo>
ptrint Schema_CastOffset();

//-----------------------------------------------------------------------------
// Helpers to check and cast a described object
//-----------------------------------------------------------------------------
template<typename TType>
bool Schema_IsA( const ISchemaObject* pObject );
template<typename TType>
TType* Schema_Cast( ISchemaObject* pObject );
template<typename TType>
const TType* Schema_Cast( const ISchemaObject* pObject );

#include "schemasystem/schemalib/schema.inl"
