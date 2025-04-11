#include "pch_launcher.h"
#include "core/icommandline.h"
#include "launcher/launcher.h"

/*
==================
WinMain
==================
*/
int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, int nCmdShow )
{
	// Initialize the main thread
	Sys_InitMainThread();

	// Init of launcher's log output and command line
	Launcher_InitLogOutput();
	CommandLine()->Init( GetCommandLineA() );

	// Initialize OS console if it need
	if ( CommandLine()->HasParam( "console" ) )
	{
		Sys_SetupConsoleIO();
	}

	// Run application
	CSingularityEngineApp		application( hInst );
	return application.Run();
}