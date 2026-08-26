#pragma once

/*
==================
ISchemaClass::Cast
==================
*/
FORCEINLINE const void* ISchemaClass::Cast( const ISchemaObject* pObject, const ISchemaType* pType ) const
{
	return Cast( (ISchemaObject*)pObject, pType );
}

/*
==================
ISchemaClass::GetBaseClass
==================
*/
FORCEINLINE const ISchemaClass* ISchemaClass::GetBaseClass() const
{
	const ISchemaCompoundType* pBaseType = GetBaseType();
	return pBaseType ? pBaseType->As<ISchemaClass>() : NULL;
}
