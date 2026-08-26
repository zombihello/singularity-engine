#pragma once

/*
==================
ISchemaObject::IsA
==================
*/
FORCEINLINE bool ISchemaObject::IsA( const ISchemaType* pType ) const
{
	const ISchemaClass* pSchemaClass = GetSchemaClass();
	Assert( pSchemaClass );
	return pSchemaClass->IsA( pType );
}

/*
==================
ISchemaObject::Cast
==================
*/
FORCEINLINE void* ISchemaObject::Cast( const ISchemaType* pType )
{
	const ISchemaClass* pSchemaClass = GetSchemaClass();
	Assert( pSchemaClass );
	return pSchemaClass->Cast( this, pType );
}

/*
==================
ISchemaObject::Cast
==================
*/
FORCEINLINE const void* ISchemaObject::Cast( const ISchemaType* pType ) const
{
	const ISchemaClass* pSchemaClass = GetSchemaClass();
	Assert( pSchemaClass );
	return pSchemaClass->Cast( this, pType );
}

/*
==================
ISchemaObject::GetSchemaRawPtr
==================
*/
FORCEINLINE const void* ISchemaObject::GetSchemaRawPtr() const
{
	return ( (ISchemaObject*)this )->GetSchemaRawPtr();
}
