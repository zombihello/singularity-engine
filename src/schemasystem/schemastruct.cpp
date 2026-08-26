#include "pch_schemasystem.h"
#include "schemasystem/schemastruct.h"

/*
==================
CSchemaStruct::CSchemaStruct
==================
*/
CSchemaStruct::CSchemaStruct( const schemaStructDesc_t& structDesc )
	: CSchemaCompoundTypeBase<ISchemaStruct>( SCHEMA_TYPE_KIND_STRUCT, structDesc )
{
}

/*
==================
CSchemaStruct::IsA
==================
*/
bool CSchemaStruct::IsA( const ISchemaType* pType ) const
{
	// Do nothing if the target type isn't valid or not a struct
	PROFILER_SCOPE_FUNC();
	if ( !pType || pType->GetKind() != SCHEMA_TYPE_KIND_STRUCT )
	{
		return false;
	}

	// Check all our base types (and ourselves)
	for ( const ISchemaCompoundType* pCurType = this; pCurType; pCurType = pCurType->GetBaseType() )
	{
		if ( pCurType == pType )
		{
			return true;
		}
	}
	return false;
}

/*
==================
CSchemaStruct::Cast
==================
*/
void* CSchemaStruct::Cast( void* pObject, const ISchemaType* pType ) const
{
	// Do nothing if the target type/the object isn't valid or not a struct
	PROFILER_SCOPE_FUNC();
	if ( !pObject || !pType )
	{
		return NULL;
	}
	if ( pType == this )
	{
		return pObject;
	}
	if ( pType->GetKind() != SCHEMA_TYPE_KIND_STRUCT )
	{
		return NULL;
	}

	// The adjustment recorded for every step is what makes this correct under
	// multiple inheritance
	const ISchemaCompoundType* pBaseType = GetBaseType();
	return pBaseType ? pBaseType->Cast( (byte*)pObject + GetBaseOffset(), pType ) : NULL;
}

/*
==================
CSchemaStruct::Construct
==================
*/
void CSchemaStruct::Construct( void* pMemory ) const
{
	PROFILER_SCOPE_FUNC();
	CheckCanConstruct();
	GetObjectOps()->pConstructFn( pMemory );
}

/*
==================
CSchemaStruct::Destruct
==================
*/
void CSchemaStruct::Destruct( void* pObject ) const
{
	PROFILER_SCOPE_FUNC();
	if ( pObject )
	{
		GetObjectOps()->pDestructFn( pObject );
	}
}
