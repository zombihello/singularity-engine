#pragma once

/*
==================
CSchemaObject::CSchemaObject
==================
*/
template<class TBaseClass>
FORCEINLINE CSchemaObject<TBaseClass>::CSchemaObject()
	: schemaFlags( SCHEMA_OBJECT_FLAG_NONE )
{
}

/*
==================
CSchemaObject::AddSchemaFlags
==================
*/
template<class TBaseClass>
FORCEINLINE void CSchemaObject<TBaseClass>::AddSchemaFlags( uint32 flags )
{
	schemaFlags |= flags;
}

/*
==================
CSchemaObject::RemoveSchemaFlags
==================
*/
template<class TBaseClass>
FORCEINLINE void CSchemaObject<TBaseClass>::RemoveSchemaFlags( uint32 flags )
{
	schemaFlags &= ~flags;
}

/*
==================
CSchemaObject::SetSchemaFlags
==================
*/
template<class TBaseClass>
FORCEINLINE void CSchemaObject<TBaseClass>::SetSchemaFlags( uint32 flags )
{
	schemaFlags = flags;
}

/*
==================
CSchemaObject::HasSchemaAllFlags
==================
*/
template<class TBaseClass>
FORCEINLINE bool CSchemaObject<TBaseClass>::HasSchemaAllFlags( uint32 flags ) const
{
	return ( GetSchemaFlags() & flags ) == flags;
}

/*
==================
CSchemaObject::HasSchemaAnyFlags
==================
*/
template<class TBaseClass>
FORCEINLINE bool CSchemaObject<TBaseClass>::HasSchemaAnyFlags( uint32 flags ) const
{
	return ( GetSchemaFlags() & flags ) != 0;
}

/*
==================
CSchemaObject::GetSchemaFlags
==================
*/
template<class TBaseClass>
FORCEINLINE uint32 CSchemaObject<TBaseClass>::GetSchemaFlags() const
{
	return schemaFlags;
}
