#ifndef MEMORY_PRIVATE_H
#define MEMORY_PRIVATE_H

#include "core/memory.h"

//-----------------------------------------------------------------------------
// Internal core functions
//-----------------------------------------------------------------------------
void Sys_OutOfMemory( size_t numBytes, uint32 alignment );

#endif // !MEMORY_PRIVATE_H