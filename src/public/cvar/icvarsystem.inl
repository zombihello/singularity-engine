#pragma once

/*
==================
ICVarSystem::FindVariable
==================
*/
FORCEINLINE ICVar* ICVarSystem::FindVariable( const char* pName ) const
{
	return FindVariable( pName, S_Strlen( pName ) );
}
