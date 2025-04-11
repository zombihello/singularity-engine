#include "pch_engine.h"
#include "engine/version.h"

/*
==================
Version command
==================
*/
CON_COMMAND( version, "Print version info string", FCVAR_NONE )
{
	Msg( "Singularity Engine " ENGINE_VERSION_STRING " build %i (" __DATE__ " " __TIME__ ")", Engine_BuildNumber() );
}

/*
==================
Quit command
==================
*/
CON_COMMAND( quit, "Quit from the game", FCVAR_NONE )
{
	Sys_RequestExit( false );
}