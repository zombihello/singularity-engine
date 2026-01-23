#pragma once
#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier1/template.h"
#include "tier1/strtools.h"
#include "tier1/interface.h"

//-----------------------------------------------------------------------------
// Connect and disconnect Tier1
//-----------------------------------------------------------------------------
bool ConnectTier1( createInterfaceFn_t pFactory );
void DisconnectTier1();
