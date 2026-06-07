#pragma once
#include "appframework/iappsystem.h"
#include "tier1/cmdargs.h"

//-----------------------------------------------------------------------------
// Console commands are marked as having a particular DLL identifier
//-----------------------------------------------------------------------------
typedef uint32 cmdDLLIdentifier_t;

//-----------------------------------------------------------------------------
// Console command flags
//-----------------------------------------------------------------------------
enum cmdFlag_t
{
	CMD_FLAG_NONE  = 0,
	CMD_FLAG_CHEAT = BIT( 0 ),	// Command is considered a cheat
	CMD_FLAG_ALL   = -1,
};

//-----------------------------------------------------------------------------
// Console command execution types
//-----------------------------------------------------------------------------
enum cmdExecution_t
{
	CMD_EXECUTION_NOW,			 // Execute command now
	CMD_EXECUTION_APPEND_BEGIN,	 // Add to begin of the command buffer, but don't run yet
	CMD_EXECUTION_APPEND_END	 // Add to end of the command buffer, but don't run yet
};

//-----------------------------------------------------------------------------
// Type execute function of a console command
//-----------------------------------------------------------------------------
typedef void ( *cmdFunctionFn_t )( const CCmdArgs& args );

//-----------------------------------------------------------------------------
// The console command system interface
//-----------------------------------------------------------------------------
#define CMDSYSTEM_INTERFACE_VERSION "SCmdSystem001"
class ICmdSystem : public IAppSystem
{
public:
	// Allocate a new identifier for a DLL
	virtual cmdDLLIdentifier_t AllocateDLLIdentifier() = 0;

	// Add and remove a command
	// NOTE: If set dllIdentifier then will be removed a command of specific a dll module
	virtual void AddCommand( const char* pName, cmdFunctionFn_t pFunctionFn, uint32 flags, const char* pDescription, cmdDLLIdentifier_t dllIdentifier ) = 0;
	virtual void RemoveCommand( const char* pName, cmdDLLIdentifier_t dllIdentifier = INVALID_INDEX )													= 0;
	virtual void RemoveAllCommands( cmdDLLIdentifier_t dllIdentifier = INVALID_INDEX )																	= 0;
	virtual void RemoveFlaggedCommands( uint32 flags, cmdDLLIdentifier_t dllIdentifier = INVALID_INDEX )												= 0;

	// Adds a command to the command buffer
	virtual void AppendCommandString( cmdExecution_t exec, const char* pCmd )	= 0;
	virtual void AppendCommandArgs( cmdExecution_t exec, const CCmdArgs& args ) = 0;

	// Pulls off \n \r or ; terminated lines of text from the command buffer and
	// executes the commands. Stops when the buffer is empty.
	// Normally called once per frame, but may be explicitly invoked
	virtual void ExecuteCommands() = 0;
};
