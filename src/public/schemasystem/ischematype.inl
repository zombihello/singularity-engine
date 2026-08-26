#pragma once

/*
==================
ISchemaType::As
==================
*/
template<class TSchemaType>
FORCEINLINE const TSchemaType* ISchemaType::As() const
{
	using typeNarrow_t			= schemaTypeNarrow_t<TSchemaType>;
	const schemaTypeKind_t kind = GetKind();
	return ( kind >= typeNarrow_t::FIRST_KIND && kind <= typeNarrow_t::LAST_KIND ) ? (const TSchemaType*)this : NULL;
}
