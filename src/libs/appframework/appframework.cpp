#include "pch_appframework.h"
#include "appframework/appframework.h"
#include "stdlib/convar.h"
#include "cvar/icvar.h"
#include "filesystem/ifilesystem.h"

/*
==================
CApplication::Create
==================
*/
bool CApplication::Create()
{
	// Load base application systems
	appSystemInfo_t		appSystemInfos[] =
	{
		{ "cvar"			DLL_EXT_STRING,		CVAR_QUERY_INTERFACE_VERSION		},	// This one must be first
		{ "filesystem"		DLL_EXT_STRING,		FILESYSTEM_INTERFACE_VERSION		},
		{ "cvar"			DLL_EXT_STRING,		CVAR_INTERFACE_VERSION				},
		{ "", "" }																		// Required to terminate the list
	};

	// Add all systems from the array
	if ( !AddSystems( appSystemInfos ) )
	{
		return false;
	}
	return true;
}

/*
==================
CApplication::PreInit
==================
*/
bool CApplication::PreInit()
{
	// Connect StdLib and register cvars
	if ( !ConnectStdLib( GetFactory() ) )
	{
		return false;
	}

	ConVar_Register();
	return true;
}

/*
==================
CApplication::Main
==================
*/
int32 CApplication::Main()
{
	// Now all base application systems are loaded, we can now load up main libraries
	Assert( pChildAppSystemGroup );
	pChildAppSystemGroup->Setup( this );
	return pChildAppSystemGroup->Run();
}

/*
==================
CApplication::PostShutdown
==================
*/
void CApplication::PostShutdown()
{
	ConVar_Unregister();
	DisconnectStdLib();
}