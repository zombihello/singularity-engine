#pragma once

/*
==================
ISchemaField::GetValuePtr
==================
*/
FORCEINLINE void* ISchemaField::GetValuePtr( void* pObject ) const
{
	Assert( pObject );
	return (byte*)pObject + GetOffset();
}

/*
==================
ISchemaField::GetValuePtr
==================
*/
FORCEINLINE const void* ISchemaField::GetValuePtr( const void* pObject ) const
{
	Assert( pObject );
	return (const byte*)pObject + GetOffset();
}

/*
==================
ISchemaFieldContainer::GetElementPtr
==================
*/
FORCEINLINE const void* ISchemaFieldContainer::GetElementPtr( const void* pObject, uint32 index ) const
{
	return GetElementPtr( (void*)pObject, index );
}

/*
==================
ISchemaField::As
==================
*/
template<class TSchemaFieldType>
FORCEINLINE const TSchemaFieldType* ISchemaField::As() const
{
	using fieldNarrow_t			 = schemaFieldNarrow_t<TSchemaFieldType>;
	const schemaFieldType_t type = GetType();
	return ( type >= fieldNarrow_t::FIRST_TYPE && type <= fieldNarrow_t::LAST_TYPE ) ? (const TSchemaFieldType*)this : NULL;
}
