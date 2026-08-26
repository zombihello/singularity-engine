#pragma once

/*
==================
CSchemaCompoundTypeBase::CSchemaCompoundTypeBase
==================
*/
template<class TBaseClass>
FORCEINLINE CSchemaCompoundTypeBase<TBaseClass>::CSchemaCompoundTypeBase( schemaTypeKind_t kind, const schemaStructDesc_t& structDesc )
	: CSchemaTypeBase<TBaseClass>( kind, structDesc.pName, structDesc.pAliasName, structDesc.flags, structDesc.typeSize, structDesc.alignment, structDesc.pMetadata, structDesc.numMetadata )
	, pBaseTypeName( structDesc.pBaseTypeName )
	, pBaseType( NULL )
	, baseOffset( structDesc.baseOffset )
	, baseFlags( SCHEMA_TYPE_FLAG_NONE )
	, numBaseFields( 0 )
	, pObjectOps( structDesc.pObjectOps )
{
}

/*
==================
CSchemaCompoundTypeBase::OnResolve
==================
*/
template<class TBaseClass>
void CSchemaCompoundTypeBase<TBaseClass>::OnResolve()
{
	// Nothing to settle when the type has no base
	PROFILER_SCOPE_FUNC();
	if ( !pBaseTypeName || !pBaseTypeName[0] )
	{
		return;
	}

	// Try to resolve the base type
	ISchemaTypeInternal* pBaseTypeInternal = ResolveDependency( pBaseTypeName );
	if ( !pBaseTypeInternal )
	{
		Sys_Error( "The base of '%s' is declared as '%s' but that type has no schema description", GetName(), pBaseTypeName );
		return;
	}

	// Make sure that the type derive from its own kind
	ISchemaType* pBaseSchemaType = pBaseTypeInternal->AsISchemaType();
	if ( pBaseSchemaType->GetKind() != GetKind() )
	{
		Sys_Error( "The base of '%s' is declared as '%s', which is a %s and not a %s. A schema type may only derive from its own kind", GetName(), pBaseTypeName, Schema_ConvTypeKindToString( pBaseSchemaType->GetKind() ), Schema_ConvTypeKindToString( GetKind() ) );
		return;
	}

	// Inherit what a base contributes
	pBaseType	  = (ISchemaCompoundType*)pBaseSchemaType;
	numBaseFields = pBaseType->GetNumFields( true );
	baseFlags	  = pBaseType->GetFlags() & SCHEMA_TYPE_MASK_FLAGS_INHERITED;
}

/*
==================
CSchemaCompoundTypeBase::OnUnresolve
==================
*/
template<class TBaseClass>
void CSchemaCompoundTypeBase<TBaseClass>::OnUnresolve()
{
	PROFILER_SCOPE_FUNC();
	baseFlags	  = SCHEMA_TYPE_FLAG_NONE;
	numBaseFields = 0;
	pBaseType	  = NULL;
}

/*
==================
CSchemaCompoundTypeBase::CheckCanConstruct
==================
*/
template<class TBaseClass>
void CSchemaCompoundTypeBase<TBaseClass>::CheckCanConstruct( bool bAllowRefCounted /* = true */ ) const
{
	PROFILER_SCOPE_FUNC();
	if ( !pObjectOps || HasAnyFlags( SCHEMA_TYPE_FLAG_ABSTRACT ) )
	{
		Sys_Error( "Schema type '%s' cannot be constructed, it is abstract or has no default constructor", GetName() );
		return;
	}

	if ( !bAllowRefCounted && HasAnyFlags( SCHEMA_TYPE_FLAG_REFCOUNTED ) )
	{
		Sys_Error( "Schema type '%s' cannot be constructed, it is reference-counted", GetName() );
		return;
	}
}

/*
==================
CSchemaCompoundTypeBase::ReadData
==================
*/
template<class TBaseClass>
bool CSchemaCompoundTypeBase<TBaseClass>::ReadData( void* pObject, ISchemaDataReader* pReader ) const
{
	return false;
}

/*
==================
CSchemaCompoundTypeBase::WriteData
==================
*/
template<class TBaseClass>
bool CSchemaCompoundTypeBase<TBaseClass>::WriteData( const void* pObject, ISchemaDataWriter* pWriter ) const
{
	return false;
}

/*
==================
CSchemaCompoundTypeBase::Copy
==================
*/
template<class TBaseClass>
void CSchemaCompoundTypeBase<TBaseClass>::Copy( void* pDestObject, const void* pSrcObject ) const
{
}

/*
==================
CSchemaCompoundTypeBase::FindField
==================
*/
template<class TBaseClass>
const ISchemaField* CSchemaCompoundTypeBase<TBaseClass>::FindField( const char* pName, bool bFindInParents ) const
{
	return NULL;
}

/*
==================
CSchemaCompoundTypeBase::GetFlags
==================
*/
template<class TBaseClass>
uint32 CSchemaCompoundTypeBase<TBaseClass>::GetFlags() const
{
	return CSchemaTypeBase<TBaseClass>::GetFlags() | baseFlags;
}

/*
==================
CSchemaCompoundTypeBase::GetNumFields
==================
*/
template<class TBaseClass>
uint32 CSchemaCompoundTypeBase<TBaseClass>::GetNumFields( bool bWithParents ) const
{
	return numBaseFields;
}

/*
==================
CSchemaCompoundTypeBase::GetField
==================
*/
template<class TBaseClass>
const ISchemaField* CSchemaCompoundTypeBase<TBaseClass>::GetField( uint32 index, bool bWithParents ) const
{
	return NULL;
}

/*
==================
CSchemaCompoundTypeBase::GetBaseType
==================
*/
template<class TBaseClass>
const ISchemaCompoundType* CSchemaCompoundTypeBase<TBaseClass>::GetBaseType() const
{
	return pBaseType;
}

/*
==================
CSchemaCompoundTypeBase::GetBaseOffset
==================
*/
template<class TBaseClass>
FORCEINLINE uint32 CSchemaCompoundTypeBase<TBaseClass>::GetBaseOffset() const
{
	return baseOffset;
}

/*
==================
CSchemaCompoundTypeBase::GetBaseType
==================
*/
template<class TBaseClass>
FORCEINLINE const schemaObjectOps_t* CSchemaCompoundTypeBase<TBaseClass>::GetObjectOps() const
{
	return pObjectOps;
}
