#pragma once
#include "tier1/builddefines.h"
#include "tier1/defines.h"
#include "tier1/types.h"
#include "tier1/template.h"
#include "tier1/strtools.h"
#include "tier1/interface.h"

//-----------------------------------------------------------------------------
// Connect and disconnect Tier1
//-----------------------------------------------------------------------------
bool ConnectTier1( createInterfaceFn_t pFactory );
void DisconnectTier1();
