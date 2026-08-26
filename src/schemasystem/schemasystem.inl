#pragma once

/*
==================
CSchemaSystem::MakeScopeId
==================
*/
FORCEINLINE schemaScopeId_t CSchemaSystem::MakeScopeId( uint16 index, uint16 generation )
{
	return (schemaScopeId_t)( ( (uint32)generation << SCHEMA_SCOPE_INDEX_BITS ) | index );
}

/*
==================
CSchemaSystem::GetScopeById
==================
*/
FORCEINLINE CSchemaScope* CSchemaSystem::GetScopeById( schemaScopeId_t scopeId ) const
{
	uint16 index = GetScopeIndex( scopeId );
	if ( index >= scopes.size() )
	{
		return NULL;
	}

	uint16			   generation = GetScopeGeneration( scopeId );
	const scopeSlot_t& slot		  = scopes[index];
	return slot.generation == generation ? slot.pScope : NULL;
}

/*
==================
CSchemaSystem::GetScopeNameById
==================
*/
FORCEINLINE const char* CSchemaSystem::GetScopeNameById( schemaScopeId_t scopeId ) const
{
	CSchemaScope* pScope = g_schemaSystem.GetScopeById( scopeId );
	return pScope ? pScope->GetName() : "(closed)";
}

/*
==================
CSchemaSystem::GetScopeIndex
==================
*/
FORCEINLINE uint16 CSchemaSystem::GetScopeIndex( schemaScopeId_t scopeId )
{
	return (uint16)( (uint32)scopeId & SCHEMA_SCOPE_INDEX_MASK );
}

/*
==================
CSchemaSystem::GetScopeGeneration
==================
*/
FORCEINLINE uint16 CSchemaSystem::GetScopeGeneration( schemaScopeId_t scopeId )
{
	return (uint16)( (uint32)scopeId >> SCHEMA_SCOPE_INDEX_BITS );
}
