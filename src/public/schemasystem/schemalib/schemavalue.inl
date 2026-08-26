#pragma once

/*
==================
schemaEnumInfo_t::GetName
==================
*/
template<class TEnum>
FORCEINLINE const char* schemaEnumInfo_t<TEnum>::GetName()
{
	return "";
}

/*
==================
schemaEnumInfo_t::GetType
==================
*/
template<class TEnum>
FORCEINLINE const ISchemaEnum* schemaEnumInfo_t<TEnum>::GetType()
{
	return "";
}

/*
==================
schemaValueTraits_t::GetTypeName
==================
*/
template<typename TType, typename TEnable>
FORCEINLINE const char* schemaValueTraits_t<TType, TEnable>::GetTypeName()
{
	return "";
}

/*
==================
schemaValueTraits_t::GetTypeName
==================
*/
template<typename TType>
FORCEINLINE const char* schemaValueTraits_t<TType, typename eastl::enable_if<eastl::is_enum<TType>::value>::type>::GetTypeName()
{
	static_assert( schemaEnumInfo_t<TType>::DECLARED, "An enum used as a field type has no DECLARE_SCHEMA_ENUM" );
	return schemaEnumInfo_t<TType>::GetName();
}

/*
==================
schemaValueTraits_t::GetTypeName
==================
*/
template<typename TType>
FORCEINLINE const char* schemaValueTraits_t<TType, typename eastl::enable_if<schemaIsStruct_t<TType>::value>::type>::GetTypeName()
{
	return TType::GetSchemaTypeNameStatic();
}

/*
==================
schemaValueTraits_t::GetTypeName
==================
*/
template<typename TType>
FORCEINLINE const char* schemaValueTraits_t<TType*, typename eastl::enable_if<schemaIsClass_t<TType>::value>::type>::GetTypeName()
{
	return TType::GetSchemaTypeNameStatic();
}

/*
==================
schemaMemberInfo_t::GetTypeName
==================
*/
template<typename TMember>
FORCEINLINE const char* schemaMemberInfo_t<TMember>::GetTypeName()
{
	return schemaValueTraits_t<element_t>::GetTypeName();
}
