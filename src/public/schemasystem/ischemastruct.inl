#pragma once

/*
==================
ISchemaStruct::GetBaseStruct
==================
*/
FORCEINLINE const ISchemaStruct* ISchemaStruct::GetBaseStruct() const
{
	const ISchemaCompoundType* pBaseType = GetBaseType();
	return pBaseType ? pBaseType->As<ISchemaStruct>() : NULL;
}
