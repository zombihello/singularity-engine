/**
 * @file
 * @addtogroup stdlib stdlib
 */

#ifndef STDLIB_H
#define STDLIB_H

#include "core/build.h"
#include "core/platform.h"
#include "core/types.h"
#include "stdlib/template.h"
#include "stdlib/strtools.h"
#include "stdlib/interface.h"

/**
 * @ingroup stdlib
 * @brief Connect stdlib
 * 
 * @param pFactory   Factory with interfaces
 * @return Return TRUE if StdLib was successfully connected, otherwise returns FALSE
 */
bool ConnectStdLib( createInterfaceFn_t pFactory );

/**
 * @ingroup stdlib
 * @brief Disconnect stdlib
 */
void DisconnectStdLib();

#endif // !STDLIB_H