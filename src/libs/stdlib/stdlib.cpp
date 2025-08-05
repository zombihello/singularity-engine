#include "pch_stdlib.h"
#include "interfaces/interfaces.h"
#include "filesystem/ifilesystem.h"
#include "cvar/icvar.h"
#include "stdlib/stdlib.h"

// Is StdLib already connected
static bool		s_bConnected = false;

/*
==================
ConnectStdLib
==================
*/
bool ConnectStdLib( createInterfaceFn_t pFactory )
{
	// Do nothing if StdLib already is connected
	if ( s_bConnected )
	{
		return true;
	}

	// Try connect file system
	if ( !g_pFileSystem )
	{
		g_pFileSystem = ( IFileSystem* )pFactory( FILESYSTEM_INTERFACE_VERSION );
		if ( !g_pFileSystem )
		{
			return false;
		}
	}

	// Try connect cvar system
	if ( !g_pCvar )
	{
		g_pCvar = ( ICvar* )pFactory( CVAR_INTERFACE_VERSION );
		if ( !g_pCvar )
		{
			return false;
		}
	}

	// StdLib was successfully connected!
	s_bConnected = true;
	return true;
}

/*
==================
DisconnectStdLib
==================
*/
void DisconnectStdLib()
{
	// Do nothing if StdLib already is disconnected
	if ( !s_bConnected )
	{
		return;
	}

	// Reset all app systems and variables
	s_bConnected = false;
	g_pFileSystem = NULL;
	g_pCvar = NULL;
}