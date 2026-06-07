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
		Msg( "exec <filename> : Execute a command file" );
		return;
	}

	const char*	  pPath = args.Argv( 1 );
	eastl::string data;
	if ( !S_LoadFileToString( pPath, data ) )
	{
		Warning( "Exec: Couldn't exec '%s'", pPath );
		return;
	}

	Msg( "Exec: Executing '%s'", pPath );
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
	const eastl::list<commandDef_t>& cmdList = s_cmdSystem.GetCmdList();
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
		Msg( "%-21s %s", pCmd->name.c_str(), pCmd->description.c_str() );
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
	// TODO BS yehor.pohuliaka - Implement it
}

/*
==================
toggle
==================
*/
CONSOLE_COMMAND( toggle, "Toggles a cvar", CMD_FLAG_NONE )
{
	// TODO BS yehor.pohuliaka - Implement it
}

/*
==================
reset
==================
*/
CONSOLE_COMMAND( reset, "Resets a cvar", CMD_FLAG_NONE )
{
	// TODO BS yehor.pohuliaka - Implement it
}
