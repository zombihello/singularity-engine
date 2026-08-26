#include "pch_schemasystem.h"
#include "schemasystem/ischemaclass.h"
#include "schemasystem/schemaenum.h"
#include "schemasystem/schemainterface.h"
#include "schemasystem/schemastruct.h"
#include "schemasystem/schemaclass.h"
#include "schemasystem/schemascope.h"

/*
==================
CSchemaScope::CSchemaScope
==================
*/
CSchemaScope::CSchemaScope( schemaScopeId_t scopeId, const char* pName )
	: id( scopeId )
	, name( pName ? pName : "" )
	, bResolved( false )
{
}

/*
==================
CSchemaScope::~CSchemaScope
==================
*/
CSchemaScope::~CSchemaScope()
{
	Clear();
}

/*
==================
CSchemaScope::Resolve
==================
*/
void CSchemaScope::Resolve()
{
	// Do nothing if the scope is resolved
	PROFILER_SCOPE_FUNC();
	if ( bResolved )
	{
		return;
	}

	// Resolve each type
	for ( uint32 index = 0, count = (uint32)types.size(); index < count; ++index )
	{
		types[index]->Resolve();
	}

	// We are done
	bResolved = true;
	return;
}

/*
==================
CSchemaScope::Unresolve
==================
*/
void CSchemaScope::Unresolve()
{
	// Do nothing if the scope isn't resolved
	PROFILER_SCOPE_FUNC();
	if ( !bResolved )
	{
		return;
	}

	// Unresolve each type
	for ( uint32 index = 0, count = (uint32)types.size(); index < count; ++index )
	{
		types[index]->Unresolve();
	}

	// We are done
	bResolved = false;
}

/*
==================
CSchemaScope::Close
==================
*/
void CSchemaScope::Close()
{
	// A scope has to outlive everything that reaches into it, so nothing may be left
	// pointing here by now
	PROFILER_SCOPE_FUNC();
	CheckHasNoDependents();
	Clear();
}

/*
==================
CSchemaScope::CheckHasNoDependents
==================
*/
void CSchemaScope::CheckHasNoDependents() const
{
	// Make sure the scope has no external dependents
	PROFILER_SCOPE_FUNC();
	for ( uint32 typeIndex = 0, typeCount = (uint32)types.size(); typeIndex < typeCount; ++typeIndex )
	{
		const ISchemaTypeInternal* pType = types[typeIndex];
		for ( uint32 dependentIndex = 0, dependentCount = pType->GetNumDependents(); dependentIndex < dependentCount; ++dependentIndex )
		{
			// Ignore dependent types in the scope
			const ISchemaTypeInternal* pDependentType = pType->GetDependent( dependentIndex );
			if ( pDependentType->GetScopeId() == id )
			{
				continue;
			}

			// Otherwise it is an external dependent and it shouldn't have happened
			Sys_Error( "Cannot close schema scope '%s', type '%s' is still referenced by type '%s' from schema scope '%s'", name.c_str(), pType->AsISchemaType()->GetName(), pDependentType->AsISchemaType()->GetName(), g_schemaSystem.GetScopeNameById( pDependentType->GetScopeId() ) );
			return;
		}
	}
}

/*
==================
CSchemaScope::Clear
==================
*/
void CSchemaScope::Clear()
{
	PROFILER_SCOPE_FUNC();
	Unresolve();
	for ( uint32 index = 0, count = (uint32)types.size(); index < count; ++index )
	{
		delete types[index];
	}

	types.clear();
	typeDict.clear();
	bResolved = false;
}

/*
==================
CSchemaScope::RegisterEnum
==================
*/
void CSchemaScope::RegisterEnum( const schemaEnumDesc_t& enumDesc )
{
	PROFILER_SCOPE_FUNC();
	RegisterType<CSchemaEnum>( enumDesc );
}

/*
==================
CSchemaScope::RegisterStruct
==================
*/
void CSchemaScope::RegisterStruct( const schemaStructDesc_t& structDesc )
{
	PROFILER_SCOPE_FUNC();
	RegisterType<CSchemaStruct>( structDesc );
}

/*
==================
CSchemaScope::RegisterClass
==================
*/
void CSchemaScope::RegisterClass( const schemaClassDesc_t& classDesc )
{
	PROFILER_SCOPE_FUNC();
	RegisterType<CSchemaClass>( classDesc );
}

/*
==================
CSchemaScope::RegisterInterface
==================
*/
void CSchemaScope::RegisterInterface( const schemaInterfaceDesc_t& interfaceDesc )
{
	PROFILER_SCOPE_FUNC();
	RegisterType<CSchemaInterface>( interfaceDesc );
}
