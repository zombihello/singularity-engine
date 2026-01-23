#include "core/core.h"
#include "launcher/launcher.h"

/*
==================
WinMain
==================
*/
int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, int nCmdShow )
{
	// Load launcher.dll
	dllHandle_t launcherHandle = Sys_DLL_LoadModule( "launcher" DLL_EXT_STRING );
	if ( !launcherHandle )
	{
		Sys_Error( "Failed to load 'launcher" DLL_EXT_STRING "'" );
		return 1;
	}

	// Get function address
	launcherMainFn_t pLauncherMainFunc = (launcherMainFn_t)Sys_DLL_GetProcAddress( launcherHandle, LAUNCHER_MAIN_FUNCNAME );
	if ( !pLauncherMainFunc )
	{
		Sys_Error( "In 'launcher" DLL_EXT_STRING "' not found function '" LAUNCHER_MAIN_FUNCNAME "'" );
		return 2;
	}

	// Run the launcher
	return pLauncherMainFunc( hInst, DEFAULT_GAMEDIR, GetCommandLineA() );
}
