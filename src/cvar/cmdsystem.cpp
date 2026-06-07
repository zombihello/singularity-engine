#include "pch_cvar.h"
#include "cvar/cmdsystem.h"
#include "cvar/cvars.h"

CCmdSystem s_cmdSystem;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CCmdSystem, ICmdSystem, CMDSYSTEM_INTERFACE_VERSION, s_cmdSystem );

/*
==================
CCmdSystem::CCmdSystem
==================
*/
CCmdSystem::CCmdSystem()
	: nextDLLIdentifier( 0 )
{
}

/*
==================
CCmdSystem::Connect
==================
*/
bool CCmdSystem::Connect( createInterfaceFn_t pFactory )
{
	PROFILER_SCOPE_FUNC();
	if ( !ConnectTier1( pFactory ) )
	{
		return false;
	}

	LinkCmds();
	return true;
}

/*
==================
CCmdSystem::Disconnect
==================
*/
void CCmdSystem::Disconnect()
{
	PROFILER_SCOPE_FUNC();
	UnlinkCmds();
	DisconnectTier1();
}

/*
==================
CCmdSystem::Shutdown
==================
*/
void CCmdSystem::Shutdown()
{
	PROFILER_SCOPE_FUNC();
	nextDLLIdentifier = 0;
	cmdList.clear();
	tokenizedCmdList.clear();
	cmdBuffer.clear();
}

/*
==================
CCmdSystem::AllocateDLLIdentifier
==================
*/
cmdDLLIdentifier_t CCmdSystem::AllocateDLLIdentifier()
{
	return nextDLLIdentifier++;
}

/*
==================
CCmdSystem::AddCommand
==================
*/
void CCmdSystem::AddCommand( const char* pName, cmdFunctionFn_t pFunctionFn, uint32 flags, const char* pDescription, cmdDLLIdentifier_t dllIdentifier )
{
	// Fail if the command already exists
	for ( auto it = cmdList.begin(), itEnd = cmdList.end(); it != itEnd; ++it )
	{
		commandDef_t& cmd = *it;
		if ( S_Stricmp( pName, cmd.name.c_str() ) != 0 )
		{
			continue;
		}

		if ( pFunctionFn != cmd.pFunctionFn )
		{
			Warning( "CmdSystem: Command '%s' already defined", pName );
		}
		return;
	}

	// Add the command into the list
	commandDef_t& cmd = cmdList.emplace_back();
	cmd.name		  = pName;
	cmd.description	  = pDescription;
	cmd.pFunctionFn	  = pFunctionFn;
	cmd.flags		  = flags;
	cmd.dllIdentifier = dllIdentifier;
}

/*
==================
CCmdSystem::RemoveCommand
==================
*/
void CCmdSystem::RemoveCommand( const char* pName, cmdDLLIdentifier_t dllIdentifier /* = INVALID_INDEX */ )
{
	for ( auto it = cmdList.begin(), itEnd = cmdList.end(); it != itEnd; ++it )
	{
		commandDef_t& cmd = *it;
		if ( !S_Stricmp( cmd.name.c_str(), pName ) && ( dllIdentifier == INVALID_INDEX || cmd.dllIdentifier == dllIdentifier ) )
		{
			cmdList.erase( it );
			break;
		}
	}
}

/*
==================
CCmdSystem::RemoveAllCommands
==================
*/
void CCmdSystem::RemoveAllCommands( cmdDLLIdentifier_t dllIdentifier /* = INVALID_INDEX */ )
{
	if ( dllIdentifier == INVALID_INDEX )
	{
		cmdList.clear();
		return;
	}

	for ( auto it = cmdList.begin(); it != cmdList.end(); )
	{
		commandDef_t& cmd = *it;
		if ( cmd.dllIdentifier == dllIdentifier )
		{
			it = cmdList.erase( it );
		}
		else
		{
			++it;
		}
	}
}

/*
==================
CCmdSystem::RemoveFlaggedCommands
==================
*/
void CCmdSystem::RemoveFlaggedCommands( uint32 flags, cmdDLLIdentifier_t dllIdentifier /* = INVALID_INDEX */ )
{
	for ( auto it = cmdList.begin(); it != cmdList.end(); )
	{
		commandDef_t& cmd = *it;
		if ( cmd.flags & flags && ( dllIdentifier == INVALID_INDEX || cmd.dllIdentifier == dllIdentifier ) )
		{
			it = cmdList.erase( it );
		}
		else
		{
			++it;
		}
	}
}

/*
==================
CCmdSystem::AppendCommandString
==================
*/
void CCmdSystem::AppendCommandString( cmdExecution_t exec, const char* pCmd )
{
	PROFILER_SCOPE_FUNC();
	switch ( exec )
	{
	case CMD_EXECUTION_NOW:
		ExecuteTokenizedCommand( CCmdArgs( pCmd ) );
		break;

	case CMD_EXECUTION_APPEND_BEGIN:
		cmdBuffer.insert( 0, pCmd );
		cmdBuffer.insert( S_Strlen( pCmd ), "\n" );
		break;

	case CMD_EXECUTION_APPEND_END:
		cmdBuffer += pCmd;
		cmdBuffer += "\n";
		break;

	default:
		AssertMsg( false, "Unknown command execution type 0x%X", exec );
		Error( "CmdSystem: Bad command execution type 0x%X", exec );
		break;
	}
}

/*
==================
CCmdSystem::AppendCommandArgs
==================
*/
void CCmdSystem::AppendCommandArgs( cmdExecution_t exec, const CCmdArgs& args )
{
	PROFILER_SCOPE_FUNC();
	switch ( exec )
	{
	case CMD_EXECUTION_NOW:
		ExecuteTokenizedCommand( args );
		break;

	case CMD_EXECUTION_APPEND_BEGIN:
		AppendCommandString( CMD_EXECUTION_APPEND_BEGIN, "_execTokenized" );
		tokenizedCmdList.emplace_front( args );
		break;

	case CMD_EXECUTION_APPEND_END:
		AppendCommandString( CMD_EXECUTION_APPEND_END, "_execTokenized" );
		tokenizedCmdList.emplace_back( args );
		break;

	default:
		AssertMsg( false, "Unknown command execution type 0x%X", exec );
		Error( "CmdSystem: Bad command execution type 0x%X", exec );
		break;
	}
}

/*
==================
CCmdSystem::ExecuteCommands
==================
*/
void CCmdSystem::ExecuteCommands()
{
	PROFILER_SCOPE_FUNC();
	CCmdArgs args;
	while ( !cmdBuffer.empty() )
	{
		uint32 splitPos		= 0;
		uint32 cmdBufferLen = (uint32)cmdBuffer.size();
		bool   bInQuotes	= false;
		for ( splitPos = 0; splitPos < cmdBufferLen; ++splitPos )
		{
			const char& c = cmdBuffer[splitPos];
			if ( c == '"' )
			{
				bInQuotes = !bInQuotes;
			}
			if ( !bInQuotes && c == ';' )
			{
				break;	// Don't break if we inside a quoted string
			}
			if ( c == '\n' || c == '\r' )
			{
				break;
			}
		}

		// Trim the command
		if ( splitPos < cmdBufferLen )
		{
			cmdBuffer[splitPos] = '\0';
		}

		bool bIsComment = !S_Strncmp( cmdBuffer.c_str(), "//", 2 ) || !S_Strncmp( cmdBuffer.c_str(), "#", 1 );
		if ( !bIsComment )
		{
			if ( !S_Strcmp( cmdBuffer.c_str(), "_execTokenized" ) )
			{
				Assert( !tokenizedCmdList.empty() );
				args = tokenizedCmdList.front();
				tokenizedCmdList.pop_front();
			}
			else
			{
				args.TokenizeString( cmdBuffer.c_str() );
			}
		}

		// Remove the processed command from the buffer
		if ( splitPos < cmdBufferLen )
		{
			cmdBuffer.erase( 0, splitPos + 1 );
		}
		else
		{
			cmdBuffer.clear();
		}

		// Execute the command
		if ( !bIsComment )
		{
			ExecuteTokenizedCommand( args );
		}
	}
}

/*
==================
CCmdSystem::ExecuteTokenizedCommand
==================
*/
void CCmdSystem::ExecuteTokenizedCommand( const CCmdArgs& args )
{
	PROFILER_SCOPE_FUNC();

	// Do nothing if we haven't any tokens
	if ( args.Argc() <= 0 )
	{
		return;
	}

	// Try to find the command to execute
	for ( auto it = cmdList.begin(), itEnd = cmdList.end(); it != itEnd; ++it )
	{
		commandDef_t& cmd = *it;
		if ( !S_Stricmp( args.Argv( 0 ), cmd.name.c_str() ) )
		{
			// Rearrange the links so that the command will be
			// near the head of the list next time it is used
			cmdList.splice( cmdList.begin(), cmdList, it );

			// Allow cheat commands only with 'cheats' on
			if ( ( cmd.flags & CMD_FLAG_CHEAT ) && !cheats.GetInt() )
			{
				Warning( "CmdSystem: Can't use cheat command '%s', unless has cheats set to 1", cmd.name.c_str() );
				return;
			}

			// Perform the action
			if ( !cmd.pFunctionFn )
			{
				break;
			}

			cmd.pFunctionFn( args );
			return;
		}
	}

	// Check cvars
	if ( g_pCVarSystem->ExecuteCommand( args ) )
	{
		return;
	}

	// Otherwise print message about the command is unknown
	Warning( "CmdSystem: Unknown command '%s'", args.Argv( 0 ) );
}
