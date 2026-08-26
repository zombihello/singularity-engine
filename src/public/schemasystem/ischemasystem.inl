#pragma once

/*
==================
ISchemaSystem::FindEnum
==================
*/
FORCEINLINE const ISchemaEnum* ISchemaSystem::FindEnum( const char* pName ) const
{
	const ISchemaType* pType = FindType( pName );
	return pType ? pType->As<ISchemaEnum>() : NULL;
}

/*
==================
ISchemaSystem::FindStruct
==================
*/
FORCEINLINE const ISchemaStruct* ISchemaSystem::FindStruct( const char* pName ) const
{
	const ISchemaType* pType = FindType( pName );
	return pType ? pType->As<ISchemaStruct>() : NULL;
}

/*
==================
ISchemaSystem::FindClass
==================
*/
FORCEINLINE const ISchemaClass* ISchemaSystem::FindClass( const char* pName ) const
{
	const ISchemaType* pType = FindType( pName );
	return pType ? pType->As<ISchemaClass>() : NULL;
}

/*
==================
ISchemaSystem::FindInterface
==================
*/
FORCEINLINE const ISchemaInterface* ISchemaSystem::FindInterface( const char* pName ) const
{
	const ISchemaType* pType = FindType( pName );
	return pType ? pType->As<ISchemaInterface>() : NULL;
}
