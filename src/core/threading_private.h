#ifndef THREADING_PRIVATE_H
#define THREADING_PRIVATE_H

#include "core/threading.h"

//-----------------------------------------------------------------------------
// Internal thread functions
//-----------------------------------------------------------------------------
void Sys_SetThreadPriority( threadHandle_t threadHandle, threadPriority_t threadPriority );
void Sys_SetThreadName( threadHandle_t threadHandle, const achar* pThreadName );

#endif // !THREADING_PRIVATE_H