/**
 * @file
 * @addtogroup core core
 */

#ifndef MEMORY_PRIVATE_H
#define MEMORY_PRIVATE_H

#include "core/memory.h"

/**
 * @ingroup core
 * @brief Handle out of memory
 *
 * @param numBytes		OOM allocation size
 * @param alignment		OOM	allocation alignment
 */
void Sys_OutOfMemory( size_t numBytes, uint32 alignment );

#endif // !MEMORY_PRIVATE_H