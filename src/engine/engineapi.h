/**
 * @file
 * @addtogroup engine engine
 */

#ifndef ENGINEAPI_H
#define ENGINEAPI_H

#include "stdlib/interface.h"

/**
 * @ingroup engine
 * @brief This factory gets to many of the major app-single systems
 */
extern createInterfaceFn_t		g_pAppSystemFactory;

#endif // !ENGINEAPI_H