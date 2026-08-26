#pragma once

/*
==================
ISchemaTypeInternal::AsISchemaType
==================
*/
FORCEINLINE const ISchemaType* ISchemaTypeInternal::AsISchemaType() const
{
	return ( (ISchemaTypeInternal*)this )->AsISchemaType();
}

#if ENABLE_SCHEMA_METADATA
/*
==================
CSchemaMetadata::Make
==================
*/
FORCEINLINE CSchemaMetadata* CSchemaMetadata::Make( const schemaMetadataDesc_t* pMetadata, uint32 numMetadata )
{
	CSchemaMetadata* pResult = new CSchemaMetadata();
	pResult->Init( pMetadata, numMetadata );
	return pResult;
}

/*
==================
CSchemaMetadata::IsEmpty
==================
*/
FORCEINLINE bool CSchemaMetadata::IsEmpty() const
{
	return metadata.empty();
}
#endif	// ENABLE_SCHEMA_METADATA

/*
==================
CSchemaTypeBase::CSchemaTypeBase
==================
*/
template<class TBaseClass>
FORCEINLINE CSchemaTypeBase<TBaseClass>::CSchemaTypeBase( schemaTypeKind_t kind, const char* pName, const char* pAliasName, uint32 flags, size typeSize, uint32 alignment, const schemaMetadataDesc_t* pMetadata /* = NULL */, uint32 numMetadata /* = 0 */ )
	: pName( pName )
	, pAliasName( pAliasName )
	, typeSize( typeSize )
	, alignment( alignment )
	, flags( flags )
	, kind( kind )
	, scopeId( INVALID_INDEX )
	, resolveState( SCHEMA_TYPE_RESOLVE_STATE_UNRESOLVED )
{
#if ENABLE_SCHEMA_METADATA
	metadata.Init( pMetadata, numMetadata );
#endif	// ENABLE_SCHEMA_METADATA
}

/*
==================
CSchemaTypeBase::HasAllFlags
==================
*/
template<class TBaseClass>
bool CSchemaTypeBase<TBaseClass>::HasAllFlags( uint32 flags ) const
{
	return ( GetFlags() & flags ) == flags;
}

/*
==================
CSchemaTypeBase::HasAnyFlags
==================
*/
template<class TBaseClass>
bool CSchemaTypeBase<TBaseClass>::HasAnyFlags( uint32 flags ) const
{
	return ( GetFlags() & flags ) != 0;
}

/*
==================
CSchemaTypeBase::GetKind
==================
*/
template<class TBaseClass>
schemaTypeKind_t CSchemaTypeBase<TBaseClass>::GetKind() const
{
	return kind;
}

/*
==================
CSchemaTypeBase::GetName
==================
*/
template<class TBaseClass>
const char* CSchemaTypeBase<TBaseClass>::GetName() const
{
	return pName;
}

/*
==================
CSchemaTypeBase::GetAliasName
==================
*/
template<class TBaseClass>
const char* CSchemaTypeBase<TBaseClass>::GetAliasName() const
{
	return pAliasName;
}

/*
==================
CSchemaTypeBase::GetFlags
==================
*/
template<class TBaseClass>
uint32 CSchemaTypeBase<TBaseClass>::GetFlags() const
{
	return flags;
}

/*
==================
CSchemaTypeBase::GetSize
==================
*/
template<class TBaseClass>
size CSchemaTypeBase<TBaseClass>::GetSize() const
{
	return typeSize;
}

/*
==================
CSchemaTypeBase::GetAlignment
==================
*/
template<class TBaseClass>
uint32 CSchemaTypeBase<TBaseClass>::GetAlignment() const
{
	return alignment;
}

/*
==================
CSchemaTypeBase::GetMetadata
==================
*/
template<class TBaseClass>
const ISchemaMetadata* CSchemaTypeBase<TBaseClass>::GetMetadata() const
{
#if ENABLE_SCHEMA_METADATA
	return &metadata;
#else
	return NULL;
#endif	// ENABLE_SCHEMA_METADATA
}

/*
==================
CSchemaTypeBase::IsA
==================
*/
template<class TBaseClass>
bool CSchemaTypeBase<TBaseClass>::IsA( const ISchemaType* pType ) const
{
	return AsISchemaType() == pType;
}

/*
==================
CSchemaTypeBase::Resolve
==================
*/
template<class TBaseClass>
void CSchemaTypeBase<TBaseClass>::Resolve()
{
	// Do nothing if the type is resolved
	PROFILER_SCOPE_FUNC();
	if ( resolveState == SCHEMA_TYPE_RESOLVE_STATE_RESOLVED )
	{
		return;
	}

	// Make sure that we don't enter a resolve cycle
	if ( resolveState == SCHEMA_TYPE_RESOLVE_STATE_RESOLVING )
	{
		Sys_Error( "The schema type '%s' is involved in a cyclic resolve", pName );
		return;
	}

	// Settle whatever this kind of type needs the others for
	resolveState = SCHEMA_TYPE_RESOLVE_STATE_RESOLVING;
	OnResolve();
	resolveState = SCHEMA_TYPE_RESOLVE_STATE_RESOLVED;
}

/*
==================
CSchemaTypeBase::Unresolve
==================
*/
template<class TBaseClass>
void CSchemaTypeBase<TBaseClass>::Unresolve()
{
	// Do nothing if the type isn't resolved
	PROFILER_SCOPE_FUNC();
	if ( resolveState != SCHEMA_TYPE_RESOLVE_STATE_RESOLVED )
	{
		return;
	}

	// Drop everything that was settled and clear dependencies
	OnUnresolve();
	ClearDependencies();
	resolveState = SCHEMA_TYPE_RESOLVE_STATE_UNRESOLVED;
}

/*
==================
CSchemaTypeBase::OnResolve
==================
*/
template<class TBaseClass>
void CSchemaTypeBase<TBaseClass>::OnResolve()
{
}

/*
==================
CSchemaTypeBase::OnUnresolve
==================
*/
template<class TBaseClass>
void CSchemaTypeBase<TBaseClass>::OnUnresolve()
{
}

/*
==================
CSchemaTypeBase::ResolveDependency
==================
*/
template<class TBaseClass>
ISchemaTypeInternal* CSchemaTypeBase<TBaseClass>::ResolveDependency( const char* pTypeName )
{
	// Try to find the type by name
	PROFILER_SCOPE_FUNC();
	ISchemaTypeInternal* pType = g_schemaSystem.FindTypeInternal( pTypeName );
	if ( !pType )
	{
		return NULL;
	}

	// The pointer about to be handed back has to outlive this type, so record which
	// way the dependency runs while both ends are in hand
	pType->Resolve();
	pType->AddDependent( this );
	dependencies.emplace_back( pType );
	return pType;
}

/*
==================
CSchemaTypeBase::AddDependent
==================
*/
template<class TBaseClass>
void CSchemaTypeBase<TBaseClass>::AddDependent( ISchemaTypeInternal* pType )
{
	PROFILER_SCOPE_FUNC();
	dependents.emplace_back( pType );
}

/*
==================
CSchemaTypeBase::RemoveDependent
==================
*/
template<class TBaseClass>
void CSchemaTypeBase<TBaseClass>::RemoveDependent( ISchemaTypeInternal* pType )
{
	PROFILER_SCOPE_FUNC();
	for ( uint32 index = 0, count = (uint32)dependents.size(); index < count; ++index )
	{
		if ( dependents[index] == pType )
		{
			dependents.erase( dependents.begin() + index );
			return;
		}
	}
}

/*
==================
CSchemaTypeBase::ClearDependencies
==================
*/
template<class TBaseClass>
void CSchemaTypeBase<TBaseClass>::ClearDependencies()
{
	PROFILER_SCOPE_FUNC();
	for ( uint32 index = 0, count = (uint32)dependencies.size(); index < count; ++index )
	{
		dependencies[index]->RemoveDependent( this );
	}
	dependencies.clear();
}

/*
==================
CSchemaTypeBase::SetScopeId
==================
*/
template<class TBaseClass>
void CSchemaTypeBase<TBaseClass>::SetScopeId( schemaScopeId_t scopeId )
{
	CSchemaTypeBase::scopeId = scopeId;
}

/*
==================
CSchemaTypeBase::IsResolved
==================
*/
template<class TBaseClass>
bool CSchemaTypeBase<TBaseClass>::IsResolved() const
{
	return resolveState == SCHEMA_TYPE_RESOLVE_STATE_RESOLVED;
}

/*
==================
CSchemaTypeBase::GetScopeId
==================
*/
template<class TBaseClass>
schemaScopeId_t CSchemaTypeBase<TBaseClass>::GetScopeId() const
{
	return scopeId;
}

/*
==================
CSchemaTypeBase::GetNumDependents
==================
*/
template<class TBaseClass>
uint32 CSchemaTypeBase<TBaseClass>::GetNumDependents() const
{
	return (uint32)dependents.size();
}

/*
==================
CSchemaTypeBase::GetDependent
==================
*/
template<class TBaseClass>
ISchemaTypeInternal* CSchemaTypeBase<TBaseClass>::GetDependent( uint32 index ) const
{
	Assert( index < (uint32)dependents.size() );
	return dependents[index];
}

/*
==================
CSchemaTypeBase::AsISchemaType
==================
*/
template<class TBaseClass>
ISchemaType* CSchemaTypeBase<TBaseClass>::AsISchemaType()
{
	return this;
}
