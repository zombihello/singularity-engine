#pragma once
#include "stdlib/builddefines.h"
#include "stdlib/defines.h"
#include "stdlib/types.h"
#include "stdlib/template.h"
#include "stdlib/strtools.h"
#include "stdlib/interface.h"

//-----------------------------------------------------------------------------
// Connect and disconnect StdLib
//-----------------------------------------------------------------------------
bool ConnectStdLib( createInterfaceFn_t pFactory );
void DisconnectStdLib();