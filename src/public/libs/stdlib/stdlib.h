#ifndef STDLIB_H
#define STDLIB_H

#include "core/build.h"
#include "core/platform.h"
#include "core/types.h"
#include "stdlib/template.h"
#include "stdlib/strtools.h"
#include "stdlib/interface.h"

//-----------------------------------------------------------------------------
// Connect and disconnect StdLib
//-----------------------------------------------------------------------------
bool ConnectStdLib( createInterfaceFn_t pFactory );
void DisconnectStdLib();

#endif // !STDLIB_H