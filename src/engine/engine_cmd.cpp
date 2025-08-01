#include "pch_engine.h"
#include "stdlib/buildnum.h"
#include "engine/version.h"

/*
==================
Version command
==================
*/
CON_COMMAND( version, "Print version info string", FCVAR_NONE )
{
	TBuildNumber<ENGINE_GOLDDATE>	engineBuildNumber;
	Msg( "Singularity Engine " ENGINE_VERSION_STRING " build %i (" __DATE__ " " __TIME__ ")", engineBuildNumber.GetBuildNumber() );
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