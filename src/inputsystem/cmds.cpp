#include "pch_inputsystem.h"
#include "inputsystem/inputsystem.h"

/*
==================
bind
==================
*/
CONSOLE_COMMAND( bind, "Bind a key", CMD_FLAG_NONE )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_INPUT );
	if ( args.Argc() < 2 )
	{
		Msg( "usage: bind <key> <command>" );
		return;
	}

	// Get button code by name
	buttonCode_t buttonCode = g_inputSystem.GetButtonCodeByName( args.Argv( 1 ) );

	// Do nothing if button isn't valid
	if ( buttonCode == BUTTON_CODE_NONE )
	{
		Warning( "bind: \"%s\" isn't a valid key", args.Argv( 1 ) );
		return;
	}

	// Set binding
	g_inputSystem.SetBinding( buttonCode, args.Argv( 2 ) );
	Msg( "bind: \"%s\" = \"%s\"", args.Argv( 1 ), args.Argv( 2 ) );
}

/*
==================
unbind
==================
*/
CONSOLE_COMMAND( unbind, "Unbind a key", CMD_FLAG_NONE )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_INPUT );
	if ( args.Argc() < 1 )
	{
		Msg( "usage: unbind <key>" );
		return;
	}

	// Get button code by name
	buttonCode_t buttonCode = g_inputSystem.GetButtonCodeByName( args.Argv( 1 ) );

	// Do nothing if button isn't valid
	if ( buttonCode == BUTTON_CODE_NONE )
	{
		Warning( "unbind: \"%s\" isn't a valid key", args.Argv( 1 ) );
		return;
	}

	// Unbind a key
	g_inputSystem.SetBinding( buttonCode, "" );
	Msg( "unbind: \"%s\" is unbind", args.Argv( 1 ) );
}

/*
==================
unbindall
==================
*/
CONSOLE_COMMAND( unbindall, "Unbind all keys", CMD_FLAG_NONE )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_INPUT );
	g_inputSystem.UnbindAll();
	Msg( "unbindall: All keys has been unbind" );
}
