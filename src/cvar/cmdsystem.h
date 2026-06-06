#pragma once
#include "cvar/icmdsystem.h"

//-----------------------------------------------------------------------------
// A command definition
//-----------------------------------------------------------------------------
struct commandDef_t
{
	eastl::string	   name;
	eastl::string	   description;
	cmdFunctionFn_t	   pFunctionFn;
	uint32			   flags;
	cmdDLLIdentifier_t dllIdentifier;
};

//-----------------------------------------------------------------------------
// The console command system
//-----------------------------------------------------------------------------
class CCmdSystem : public CBaseAppSystem<ICmdSystem>
{
public:
	// IAppSystem interface
	// Here's where the app systems get to learn about each other
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// Initialize and shutdown
	virtual void Shutdown() override;

	// ICmdSystem interface
	// Allocate a new identifier for a DLL
	virtual cmdDLLIdentifier_t AllocateDLLIdentifier() override;

	// Add and remove a command
	// NOTE: If set dllIdentifier then will be removed a command of specific a dll module
	virtual void AddCommand( const char* pName, cmdFunctionFn_t pFunctionFn, uint32 flags, const char* pDescription, cmdDLLIdentifier_t dllIdentifier ) override;
	virtual void RemoveCommand( const char* pName, cmdDLLIdentifier_t dllIdentifier = INVALID_INDEX ) override;
	virtual void RemoveAllCommands( cmdDLLIdentifier_t dllIdentifier = INVALID_INDEX ) override;
	virtual void RemoveFlaggedCommands( uint32 flags, cmdDLLIdentifier_t dllIdentifier = INVALID_INDEX ) override;

	// Adds a command to the command buffer
	virtual void AppendCommandString( cmdExecution_t exec, const char* pCmd ) override;
	virtual void AppendCommandArgs( cmdExecution_t exec, const CCmdArgs& args ) override;

	// Pulls off \n \r or ; terminated lines of text from the command buffer and
	// executes the commands. Stops when the buffer is empty.
	// Normally called once per frame, but may be explicitly invoked
	virtual void ExecuteCommands() override;

	CCmdSystem();
	const eastl::list<commandDef_t>& GetCmdList() const;

private:
	void ExecuteTokenizedCommand( const CCmdArgs& args );

	cmdDLLIdentifier_t		  nextDLLIdentifier;
	eastl::string			  cmdBuffer;
	eastl::list<CCmdArgs>	  tokenizedCmdList;	 // Piggybacks on the text buffer, avoids tokenize again and screwing it up
	eastl::list<commandDef_t> cmdList;
};

extern CCmdSystem s_cmdSystem;
#include "cvar/cmdsystem.inl"
