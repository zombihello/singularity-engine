#pragma once

/*
==================
CSchemaScope::IsResolved
==================
*/
FORCEINLINE bool CSchemaScope::IsResolved() const
{
	return bResolved;
}

/*
==================
CSchemaScope::RegisterType
==================
*/
template<class TSchemaType, typename TSchemaTypeDesc>
void CSchemaScope::RegisterType( const TSchemaTypeDesc& typeDesc )
{
	// Make sure that the type has name
	PROFILER_SCOPE_FUNC();
	if ( !typeDesc.pName || !typeDesc.pName[0] )
	{
		AssertMsg( false, "Tried to register a type without a name" );
		return;
	}

	// Make sure that the name is free in EVERY scope, not only in the one
	ISchemaTypeInternal* pExistingType = g_schemaSystem.FindTypeInternal( typeDesc.pName );
	if ( pExistingType )
	{
		Sys_Error( "Duplicate schema type '%s' in scopes '%s' and '%s'. Type names must be unique across every scope", typeDesc.pName, g_schemaSystem.GetScopeNameById( pExistingType->GetScopeId() ), name.c_str() );
		return;
	}

	// Create a new schema type
	uint32		 typeId = (uint32)types.size();
	TSchemaType* pType	= new TSchemaType( typeDesc );
	pType->SetScopeId( id );
	types.emplace_back( pType );
	typeDict.emplace( typeDesc.pName, typeId );

	// Make alias with the type if it is free
	bool bHasAliasName = typeDesc.pAliasName && typeDesc.pAliasName[0];
	if ( bHasAliasName )
	{
		if ( !g_schemaSystem.FindTypeInternal( typeDesc.pAliasName ) )
		{
			typeDict.emplace( typeDesc.pAliasName, typeId );
		}
		else
		{
			DevWarning( "SchemaSystem: The type '%s' is registered without its alias name '%s', the name is already taken", typeDesc.pName, typeDesc.pAliasName );
		}
	}

	// Resolve the type if the scope is resolved
	if ( bResolved )
	{
		pType->Resolve();
	}

	// We are done
	DevMsg( "SchemaSystem: Registered '%s'%s in scope '%s'", typeDesc.pName, bHasAliasName ? va( " (%s)", typeDesc.pAliasName ) : "", name.c_str() );
}

/*
==================
CSchemaScope::FindType
==================
*/
FORCEINLINE const ISchemaTypeInternal* CSchemaScope::FindType( const char* pName ) const
{
	PROFILER_SCOPE_FUNC();
	auto it = typeDict.find( pName );
	return it != typeDict.end() ? types[it->second] : NULL;
}

/*
==================
CSchemaScope::GetId
==================
*/
FORCEINLINE schemaScopeId_t CSchemaScope::GetId() const
{
	return id;
}

/*
==================
CSchemaScope::GetName
==================
*/
FORCEINLINE const char* CSchemaScope::GetName() const
{
	return name.c_str();
}

/*
==================
CSchemaScope::GetNumTypes
==================
*/
FORCEINLINE uint32 CSchemaScope::GetNumTypes() const
{
	return (uint32)types.size();
}

/*
==================
CSchemaScope::GetType
==================
*/
FORCEINLINE const ISchemaTypeInternal* CSchemaScope::GetType( uint32 index ) const
{
	Assert( index < (uint32)types.size() );
	return types[index];
}
