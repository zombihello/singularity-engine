#include "pch_schemasystem.h"
#include "schemasystem/schematypebase.h"
#include "schemasystem/schemascope.h"
#include "schemasystem/schemasystem.h"

CSchemaSystem g_schemaSystem;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CSchemaSystem, ISchemaSystem, SCHEMASYSTEM_INTERFACE_VERSION, g_schemaSystem );

/*
==================
CSchemaSystem::CSchemaSystem
==================
*/
CSchemaSystem::CSchemaSystem()
	: bInited( false )
	, generation( 0 )
{
}

/*
==================
CSchemaSystem::Connect
==================
*/
bool CSchemaSystem::Connect( createInterfaceFn_t pFactory )
{
	// Connect Tier1
	PROFILER_SCOPE_FUNC();
	if ( !ConnectTier1( pFactory ) )
	{
		return false;
	}
	LinkCmds();
	LinkCVars();
	return true;
}

/*
==================
CSchemaSystem::Disconnect
==================
*/
void CSchemaSystem::Disconnect()
{
	PROFILER_SCOPE_FUNC();
	UnlinkCmds();
	UnlinkCVars();
	DisconnectTier1();
}

/*
==================
CSchemaSystem::Init
==================
*/
bool CSchemaSystem::Init()
{
	// Resolve each scope
	PROFILER_SCOPE_FUNC();
	for ( uint32 index = 0, count = (uint32)openScopeIndices.size(); index < count; ++index )
	{
		scopeSlot_t& scope = scopes[openScopeIndices[index]];
		scope.pScope->Resolve();
	}

	// We are done
	bInited = true;
	return true;
}

/*
==================
CSchemaSystem::Shutdown
==================
*/
void CSchemaSystem::Shutdown()
{
	// Destroy each scope
	PROFILER_SCOPE_FUNC();
	for ( uint32 index = 0, count = (uint32)openScopeIndices.size(); index < count; ++index )
	{
		scopeSlot_t& scope = scopes[openScopeIndices[index]];
		delete scope.pScope;
	}
	scopes.clear();
	openScopeIndices.clear();
	freeScopeIndexList.clear();

	// We are done
	bInited = false;
}

/*
==================
CSchemaSystem::OpenScope
==================
*/
schemaScopeId_t CSchemaSystem::OpenScope( const char* pName )
{
	// If we have free indices then use it
	PROFILER_SCOPE_FUNC();
	uint16 index = INVALID_INDEX;
	if ( !freeScopeIndexList.empty() )
	{
		index = freeScopeIndexList.front();
		freeScopeIndexList.pop_front();
	}
	else
	{
		// Make sure we don't go beyond the maximum limits
		if ( scopes.size() >= SCHEMA_SCOPE_MAX_SCOPES )
		{
			Sys_Error( "Out of schema scopes, %i is all a scope id can address", SCHEMA_SCOPE_MAX_SCOPES );
			return INVALID_INDEX;
		}

		// Otherwise all ok and we can allocate a new scope slot
		index = (uint16)scopes.size();
		scopes.emplace_back( scopeSlot_t{ NULL, 0 } );
	}

	// Open the scope under an id nothing that was handed out before can match
	scopeSlot_t&	slot	= scopes[index];
	schemaScopeId_t scopeId = MakeScopeId( index, slot.generation );
	slot.pScope				= new CSchemaScope( scopeId, pName );
	openScopeIndices.emplace_back( index );

	// Resolve the scope if the system already initialized
	if ( bInited )
	{
		slot.pScope->Resolve();
	}

	// We are done
	DevMsg( "SchemaSystem: Opened scope '%s'", slot.pScope->GetName() );
	return scopeId;
}

/*
==================
CSchemaSystem::CloseScope
==================
*/
void CSchemaSystem::CloseScope( schemaScopeId_t scopeId )
{
	// Do nothing if the id isn't valid
	PROFILER_SCOPE_FUNC();
	CSchemaScope* pScope = GetScopeById( scopeId );
	if ( !pScope )
	{
		return;
	}

	// Close and destroy the scope
	DevMsg( "SchemaSystem: Closed scope '%s'", pScope->GetName() );
	pScope->Close();
	delete pScope;

	// Mark the slot as free
	uint16		 index = GetScopeIndex( scopeId );
	scopeSlot_t& scope = scopes[index];
	scope.pScope	   = NULL;
	++scope.generation;
	++generation;
	freeScopeIndexList.emplace_back( index );

	// Remove the index from an array of opened scopes
	for ( uint32 openIndex = 0, count = (uint32)openScopeIndices.size(); openIndex < count; ++openIndex )
	{
		if ( openScopeIndices[openIndex] == index )
		{
			openScopeIndices.erase( openScopeIndices.begin() + openIndex );
			break;
		}
	}
}

/*
==================
CSchemaSystem::RegisterEnum
==================
*/
void CSchemaSystem::RegisterEnum( const schemaEnumDesc_t& enumDesc, schemaScopeId_t scopeId )
{
	PROFILER_SCOPE_FUNC();
	CSchemaScope* pScope = GetScopeById( scopeId );
	if ( EnsureMsg( pScope, "Tried to register a schema type into a scope that is not open" ) )
	{
		pScope->RegisterEnum( enumDesc );
	}
}

/*
==================
CSchemaSystem::RegisterInterface
==================
*/
void CSchemaSystem::RegisterInterface( const schemaInterfaceDesc_t& interfaceDesc, schemaScopeId_t scopeId )
{
	PROFILER_SCOPE_FUNC();
	CSchemaScope* pScope = GetScopeById( scopeId );
	if ( EnsureMsg( pScope, "Tried to register a schema type into a scope that is not open" ) )
	{
		pScope->RegisterInterface( interfaceDesc );
	}
}

/*
==================
CSchemaSystem::RegisterStruct
==================
*/
void CSchemaSystem::RegisterStruct( const schemaStructDesc_t& structDesc, schemaScopeId_t scopeId )
{
	PROFILER_SCOPE_FUNC();
	CSchemaScope* pScope = GetScopeById( scopeId );
	if ( EnsureMsg( pScope, "Tried to register a schema type into a scope that is not open" ) )
	{
		pScope->RegisterStruct( structDesc );
	}
}

/*
==================
CSchemaSystem::RegisterClass
==================
*/
void CSchemaSystem::RegisterClass( const schemaClassDesc_t& classDesc, schemaScopeId_t scopeId )
{
	PROFILER_SCOPE_FUNC();
	CSchemaScope* pScope = GetScopeById( scopeId );
	if ( EnsureMsg( pScope, "Tried to register a schema type into a scope that is not open" ) )
	{
		pScope->RegisterClass( classDesc );
	}
}

/*
==================
CSchemaSystem::FindType
==================
*/
const ISchemaType* CSchemaSystem::FindType( const char* pName ) const
{
	PROFILER_SCOPE_FUNC();
	ISchemaTypeInternal* pType = FindTypeInternal( pName );
	return pType ? pType->AsISchemaType() : NULL;
}

/*
==================
CSchemaSystem::FindTypeInternal
==================
*/
ISchemaTypeInternal* CSchemaSystem::FindTypeInternal( const char* pName ) const
{
	PROFILER_SCOPE_FUNC();
	for ( uint32 index = 0, count = (uint32)openScopeIndices.size(); index < count; ++index )
	{
		const scopeSlot_t&		   scope = scopes[openScopeIndices[index]];
		const ISchemaTypeInternal* pType = scope.pScope->FindType( pName );
		if ( pType )
		{
			return (ISchemaTypeInternal*)pType;
		}
	}
	return NULL;
}

/*
==================
CSchemaSystem::GetNumScopes
==================
*/
uint32 CSchemaSystem::GetNumScopes() const
{
	return (uint32)openScopeIndices.size();
}

/*
==================
CSchemaSystem::GetScope
==================
*/
schemaScope_t CSchemaSystem::GetScope( uint32 index ) const
{
	PROFILER_SCOPE_FUNC();
	Assert( index < openScopeIndices.size() );
	uint16			   scopeIndex = openScopeIndices[index];
	const scopeSlot_t& scope	  = scopes[scopeIndex];
	return schemaScope_t{ MakeScopeId( scopeIndex, scope.generation ), scope.pScope->GetName() };
}

/*
==================
CSchemaSystem::GetNumTypes
==================
*/
uint32 CSchemaSystem::GetNumTypes( schemaScopeId_t scopeId ) const
{
	PROFILER_SCOPE_FUNC();
	const CSchemaScope* pScope = GetScopeById( scopeId );
	Assert( pScope );
	return pScope->GetNumTypes();
}

/*
==================
CSchemaSystem::GetType
==================
*/
const ISchemaType* CSchemaSystem::GetType( schemaScopeId_t scopeId, uint32 index ) const
{
	PROFILER_SCOPE_FUNC();
	const CSchemaScope* pScope = GetScopeById( scopeId );
	Assert( pScope );
	return pScope->GetType( index )->AsISchemaType();
}

/*
==================
CSchemaSystem::GetGeneration
==================
*/
uint32 CSchemaSystem::GetGeneration() const
{
	return generation;
}
