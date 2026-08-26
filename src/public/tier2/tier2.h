#pragma once
#include "tier1/interface.h"

//-----------------------------------------------------------------------------
// Connect and disconnect Tier2
//
// NOTE: Tier2 is where the engine level subsystems live, the ones Tier1 is not
//		 allowed to know about. Connect it after Tier1, the same way
//-----------------------------------------------------------------------------
bool ConnectTier2( createInterfaceFn_t pFactory );
void DisconnectTier2();
