#include "pch_cvar.h"
#include "tier1/buildnum.h"
#include "tier0/version.h"
#include "filesystem/ifilesystem.h"
#include "cvar/cmdsystem.h"
#include "cvar/cvarsystem.h"

/*
==================
version
==================
*/
CONSOLE_COMMAND( version, "Print version info string", CMD_FLAG_NONE )
{
	CBuildNumber<ENGINE_GOLDDATE> engineBuildNumber;
	Msg( "Singularity Engine " ENGINE_VERSION_STRING " build %i (" __DATE__ " " __TIME__ ")", engineBuildNumber.GetBuildNumber() );
}

/*
==================
quit
==================
*/
CONSOLE_COMMAND( quit, "Quit from the game", CMD_FLAG_NONE )
{
	Sys_RequestExit( false );
}

/*
==================
echo
==================
*/
CONSOLE_COMMAND( echo, "Prints text", CMD_FLAG_NONE )
{
	Msg( args.Args( 1, INVALID_INDEX ).c_str() );
}

/*
==================
exec
==================
*/
CONSOLE_COMMAND( exec, "Execute a command file", CMD_FLAG_NONE )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	if ( args.Argc() != 2 )
	{
		Msg( "usage: exec <filename>" );
		return;
	}

	const char*	  pPath = args.Argv( 1 );
	eastl::string data;
	if ( !S_LoadFileToString( pPath, data ) )
	{
		Warning( "exec: Couldn't exec '%s'", pPath );
		return;
	}

	Msg( "exec: Executing '%s'", pPath );
	g_pCmdSystem->AppendCommandString( CMD_EXECUTION_APPEND_BEGIN, data.c_str() );
}

/*
==================
listCmds
==================
*/
CONSOLE_COMMAND( listCmds, "Lists commands", CMD_FLAG_NONE )
{
	eastl::string					 match;
	eastl::list<const commandDef_t*> foundCmdList;
	const eastl::list<commandDef_t>& cmdList = g_cmdSystem.GetCmdList();
	if ( args.Argc() > 1 )
	{
		match = args.Args( 1, INVALID_INDEX );
	}

	// Try to find all commands with matches the pattern
	for ( auto it = cmdList.begin(), itEnd = cmdList.end(); it != itEnd; ++it )
	{
		const commandDef_t& cmd = *it;
		if ( !match.empty() && !S_StringMatchesPattern( cmd.name.c_str(), match.c_str() ) )
		{
			continue;
		}

		foundCmdList.emplace_back( &cmd );
	}

	// Print all founded commands
	for ( auto it = foundCmdList.begin(), itEnd = foundCmdList.end(); it != itEnd; ++it )
	{
		const commandDef_t* pCmd = *it;
		Msg( "%-32s %s", pCmd->name.c_str(), pCmd->description.c_str() );
	}
	Msg( "%i commands", foundCmdList.size() );
}

/*
==================
listCVars
==================
*/
CONSOLE_COMMAND( listCVars, "Lists cvars", CMD_FLAG_NONE )
{
	eastl::string			   match;
	eastl::list<ICVar*>		   foundCVarList;
	const eastl::list<ICVar*>& cvarList = g_cvarSystem.GetCVarList();
	if ( args.Argc() > 1 )
	{
		match = args.Args( 1, INVALID_INDEX );
	}

	// Try to find all cvars with matches the pattern
	for ( auto it = cvarList.begin(), itEnd = cvarList.end(); it != itEnd; ++it )
	{
		ICVar* pCVar = *it;
		if ( !match.empty() && !S_StringMatchesPattern( pCVar->GetName(), match.c_str() ) )
		{
			continue;
		}

		foundCVarList.emplace_back( pCVar );
	}

	// Print all founded cvars
	for ( auto it = foundCVarList.begin(), itEnd = foundCVarList.end(); it != itEnd; ++it )
	{
		ICVar* pCVar = *it;
		Msg( "%-32s %s", pCVar->GetName(), pCVar->GetDescription() );
	}
	Msg( "%i cvars", foundCVarList.size() );
}

/*
==================
toggle
==================
*/
CONSOLE_COMMAND( toggle, "Toggles a cvar", CMD_FLAG_NONE )
{
	uint32 argc = args.Argc();
	if ( argc < 2 )
	{
		Msg( "usage:" );
		Msg( "\ttoggle <variable> - Toggles between 0 and 1" );
		Msg( "\ttoggle <variable> <value> - Toggles between 0 and <value>" );
		Msg( "\ttoggle <string 1>...<string n> - Cycles through all strings" );
		return;
	}

	// Try to find a cvar
	const char* pCVarName = args.Argv( 1 );
	ICVar*		pCVar	  = g_cvarSystem.FindVariable( pCVarName, S_Strlen( pCVarName ) );
	if ( !pCVar )
	{
		Warning( "toggle: CVar '%s' not found", pCVarName );
		return;
	}

	// Cycle through multiple values
	if ( argc > 3 )
	{
		uint32		index		  = 2;
		const char* pCurrentValue = pCVar->GetString();
		for ( index = 2; index < argc; ++index )
		{
			if ( !S_Stricmp( pCurrentValue, args.Argv( index ) ) )
			{
				// Point to the next value
				++index;
				break;
			}
		}
		if ( index >= argc )
		{
			index = 2;
		}

		Msg( "toggle: '%s' = '%s'", pCVarName, args.Argv( index ) );
		pCVar->SetString( args.Argv( index ) );
	}
	// Otherwise toggle between 0 and 1
	else
	{
		float valueToSet = argc == 3 ? S_Atof( args.Argv( 2 ) ) : 1.f;
		if ( pCVar->GetFloat() != 0.f )
		{
			valueToSet = 0.f;
		}

		Msg( "toggle: '%s' = %f", pCVarName, valueToSet );
		pCVar->SetString( va( "%f", valueToSet ) );
	}
}

/*
==================
reset
==================
*/
CONSOLE_COMMAND( reset, "Resets a cvar", CMD_FLAG_NONE )
{
	if ( args.Argc() != 2 )
	{
		Msg( "usage: reset <variable>" );
		return;
	}

	// Try to find a cvar
	const char* pCVarName = args.Argv( 1 );
	ICVar*		pCVar	  = g_cvarSystem.FindVariable( pCVarName, S_Strlen( pCVarName ) );
	if ( !pCVar )
	{
		Warning( "reset: CVar '%s' not found", pCVarName );
		return;
	}

	// Reset the cvar
	pCVar->Reset();
}
