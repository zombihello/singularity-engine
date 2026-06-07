#include "pch_tier1.h"
#include "utils/interfaces/interfaces.h"
#include "filesystem/ifilesystem.h"
#include "cvar/icmdsystem.h"
#include "cvar/icvarsystem.h"
#include "tier1/tier1.h"

// Is Tier1 already connected
static bool s_bConnected = false;

/*
==================
ConnectTier1
==================
*/
bool ConnectTier1( createInterfaceFn_t pFactory )
{
	PROFILER_SCOPE_FUNC();

	// Do nothing if Tier1 already is connected
	if ( s_bConnected )
	{
		return true;
	}

	// Try connect file system
	if ( !g_pFileSystem )
	{
		g_pFileSystem = (IFileSystem*)pFactory( FILESYSTEM_INTERFACE_VERSION );
		if ( !g_pFileSystem )
		{
			return false;
		}
	}

	// Try connect the console command system
	if ( !g_pCmdSystem )
	{
		g_pCmdSystem = (ICmdSystem*)pFactory( CMDSYSTEM_INTERFACE_VERSION );
		if ( !g_pCmdSystem )
		{
			return false;
		}
	}

	// Try connect the console variable system
	if ( !g_pCVarSystem )
	{
		g_pCVarSystem = (ICVarSystem*)pFactory( CVARSYSTEM_INTERFACE_VERSION );
		if ( !g_pCVarSystem )
		{
			return false;
		}
	}

	// Tier1 was successfully connected!
	s_bConnected = true;
	return true;
}

/*
==================
DisconnectTier1
==================
*/
void DisconnectTier1()
{
	PROFILER_SCOPE_FUNC();

	// Do nothing if Tier1 already is disconnected
	if ( !s_bConnected )
	{
		return;
	}

	// Reset all app systems and variables
	s_bConnected  = false;
	g_pFileSystem = NULL;
	g_pCmdSystem  = NULL;
	g_pCVarSystem = NULL;
}
