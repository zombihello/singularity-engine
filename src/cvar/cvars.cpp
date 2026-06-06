#include "pch_cvar.h"
#include "cvar/cvars.h"

//-----------------------------------------------------------------------------
// CVars
//-----------------------------------------------------------------------------
void CheatsCVarChanged( IConVar* pConVar );
void DeveloperCVarChanged( IConVar* pConVar );

CConVar cheats( "cheats", "0", "Allow cheats in the game", FCVAR_NONE, CheatsCVarChanged );
CConVar developer( "developer", "0", "Enables developer messages", FCVAR_NONE, DeveloperCVarChanged );

/*
==================
CheatsChanged
==================
*/
static void CheatsCVarChanged( IConVar* pConVar )
{
	PROFILER_SCOPE_FUNC();

	// Cheats were disabled, revert all cheat cvars to their default values
	if ( g_pCvar && pConVar->GetInt() == 0 )
	{
		g_pCvar->ResetFlaggedVars( FCVAR_CHEAT );
		Msg( "Cvar: FCVAR_CHEAT cvars reverted to defaults" );
	}
}

/*
==================
DeveloperCVarChanged
==================
*/
static void DeveloperCVarChanged( IConVar* pConVar )
{
	Logger()->SetGroupActivate( LOG_GROUP_DEVELOPER, pConVar->GetBool() );
}
