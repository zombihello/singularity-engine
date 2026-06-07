#include "pch_cvar.h"
#include "cvar/cvars.h"

//-----------------------------------------------------------------------------
// CVars
//-----------------------------------------------------------------------------
void CheatsCVarChanged( ICVar* pCVar, const char* pOldValueStr, float oldValue );
void DeveloperCVarChanged( ICVar* pCVar, const char* pOldValueStr, float oldValue );

CCVar cheats( "cheats", "0", "Allow cheats in the game", CVAR_FLAG_NONE, CheatsCVarChanged );
CCVar developer( "developer", "0", "Enables developer messages", CVAR_FLAG_NONE, DeveloperCVarChanged );

/*
==================
CheatsChanged
==================
*/
static void CheatsCVarChanged( ICVar* pCVar, const char* pOldValueStr, float oldValue )
{
	// Cheats were disabled, revert all cheat cvars to their default values
	if ( g_pCVarSystem && oldValue != 0 )
	{
		g_pCVarSystem->ResetFlaggedVariables( CVAR_FLAG_CHEAT );
		Msg( "CVarSystem: CVAR_FLAG_CHEAT cvars reverted to defaults" );
	}
}

/*
==================
DeveloperCVarChanged
==================
*/
static void DeveloperCVarChanged( ICVar* pCVar, const char* pOldValueStr, float oldValue )
{
	Logger()->SetGroupActivate( LOG_GROUP_DEVELOPER, pCVar->GetInt() != 0 );
}
