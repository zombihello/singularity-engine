#pragma once
#include <EASTL/type_traits.h>

//-----------------------------------------------------------------------------
// What KIND of described type a C++ type was declared as
//
// NOTE: The `is_same` is what makes these answer for the type ITSELF. The aliases
//		 a `DECLARE_SCHEMA_*` macro emits are inherited like any other member, so without it
//		 every type deriving from a described one would answer `true` while naming
//		 its ancestor's alias
//-----------------------------------------------------------------------------
template<typename TType, typename = void>
struct schemaIsInterface_t : eastl::false_type
{
};

template<typename TType, typename = void>
struct schemaIsStruct_t : eastl::false_type
{
};

template<typename TType, typename = void>
struct schemaIsClass_t : eastl::false_type
{
};

template<typename TType>
struct schemaIsInterface_t<TType, eastl::void_t<typename TType::thisInterface_t>> : eastl::is_same<TType, typename TType::thisInterface_t>
{
};

template<typename TType>
struct schemaIsStruct_t<TType, eastl::void_t<typename TType::thisStruct_t>> : eastl::is_same<TType, typename TType::thisStruct_t>
{
};

template<typename TType>
struct schemaIsClass_t<TType, eastl::void_t<typename TType::thisClass_t>> : eastl::is_same<TType, typename TType::thisClass_t>
{
};

//-----------------------------------------------------------------------------
// The same three asked about the BASE a described type declares, where `void`
// means "no base" and always passes
//-----------------------------------------------------------------------------
template<typename TBase>
struct schemaBaseIsInterface_t : schemaIsInterface_t<TBase>
{
};

template<typename TBase>
struct schemaBaseIsStruct_t : schemaIsStruct_t<TBase>
{
};

template<typename TBase>
struct schemaBaseIsClass_t : schemaIsClass_t<TBase>
{
};

template<>
struct schemaBaseIsInterface_t<void> : eastl::true_type
{
};

template<>
struct schemaBaseIsStruct_t<void> : eastl::true_type
{
};

template<>
struct schemaBaseIsClass_t<void> : eastl::true_type
{
};

//-----------------------------------------------------------------------------
// Is the base a described type names an actual C++ base of it
//-----------------------------------------------------------------------------
template<typename TType, typename TBase>
struct schemaBaseIsCppBase_t : eastl::is_base_of<TBase, TType>
{
};

template<typename TType>
struct schemaBaseIsCppBase_t<TType, void> : eastl::true_type
{
};
