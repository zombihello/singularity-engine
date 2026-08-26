#pragma once

/*
==================
ISchemaCompoundType::Cast
==================
*/
FORCEINLINE const void* ISchemaCompoundType::Cast( const void* pObject, const ISchemaType* pType ) const
{
	return Cast( (void*)pObject, pType );
}
