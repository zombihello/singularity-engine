#include "pch_tier2.h"
#include "schemasystem/ischemasystem.h"
#include "tier2/tier2.h"

// Is Tier2 already connected
static bool s_bConnected = false;

/*
==================
ConnectTier2
==================
*/
bool ConnectTier2( createInterfaceFn_t pFactory )
{
	// Do nothing if Tier2 already is connected
	PROFILER_SCOPE_FUNC();
	if ( s_bConnected )
	{
		return true;
	}

	// Try connect the schema system
	if ( !g_pSchemaSystem )
	{
		g_pSchemaSystem = (ISchemaSystem*)pFactory( SCHEMASYSTEM_INTERFACE_VERSION );
		if ( !g_pSchemaSystem )
		{
			return false;
		}
	}

	// Tier2 was successfully connected!
	s_bConnected = true;
	return true;
}

/*
==================
DisconnectTier2
==================
*/
void DisconnectTier2()
{
	// Do nothing if Tier2 already is disconnected
	PROFILER_SCOPE_FUNC();
	if ( !s_bConnected )
	{
		return;
	}

	// Reset all app systems and variables
	s_bConnected	= false;
	g_pSchemaSystem = NULL;
}
