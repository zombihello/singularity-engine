#pragma once
#include "cvar/icmdsystem.h"

//-----------------------------------------------------------------------------
// The CONSOLE_COMMAND macro is an even easier way to create a console command by
// automatically generating the cmdLink_t variable
//
// For example:
// CONSOLE_COMMAND( mycommand, "My console command" )
// {
//		Msg( "It's work!" );
// }
//-----------------------------------------------------------------------------
#define CONSOLE_COMMAND( Name, Description, Flags )                            \
	static void		 CmdExec_##Name( const CCmdArgs& args );                   \
	static cmdLink_t s_cmd##Name( #Name, CmdExec_##Name, Description, Flags ); \
	static void		 CmdExec_##Name( const CCmdArgs& args )

#define CONSOLE_COMMAND_EXTERN( Name, FuncName, Description, Flags )     \
	void			 FuncName( const CCmdArgs& args );                   \
	static cmdLink_t s_cmd##Name( #Name, FuncName, Description, Flags ); \
	void			 FuncName( const CCmdArgs& args )

#define CONSOLE_COMMAND_METHOD( Name, MethodName, Description, Flags ) \
	static cmdLink_t s_cmd##Name( #Name, MethodName, Description, Flags );

//-----------------------------------------------------------------------------
// cmdLink_t is a convenient way to get a function registered as a
// console command without having to add an explicit call to ICmdSystem::AddCommand() in a startup
// function somewhere. Simply declare a static variable with the parameters and it will get
// executed before main()
//
// For example:
// static cmdLink_t sys_dumpMemory( "sys_dumpMemory", Sys_DumpMemory_f, "Walks the heap and reports stats" )
//-----------------------------------------------------------------------------
struct cmdLink_t
{
	cmdLink_t( const char* pName, cmdFunctionFn_t pFunctionFn, const char* pDescription, uint32 flags = CMD_FLAG_NONE );

	cmdLink_t*		pNext;
	const char*		pName;
	const char*		pDescription;
	cmdFunctionFn_t pFunctionFn;
	uint32			flags;
};

//-----------------------------------------------------------------------------
// Register console commands implemented in a local module
//-----------------------------------------------------------------------------
class CCmdsLocalRegister
{
	friend cmdLink_t;
	friend void LinkCmds( uint32 flags );
	friend void UnlinkCmds();

protected:
	static cmdLink_t*		  s_pCmdList;  // Statically constructed list of cmdLink_t, used for link them in the command system
	static cmdDLLIdentifier_t s_dllIdentifier;
};

//-----------------------------------------------------------------------------
// Link and unlink commands in the command system
// flags - Add these flags to commands
//
// NOTE: Call them only once `ConnectTier1` has given us the command system
//-----------------------------------------------------------------------------
void LinkCmds( uint32 flags = CMD_FLAG_NONE );
void UnlinkCmds();

#include "tier1/cmdlink.inl"
