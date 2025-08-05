#pragma once
#include "core/threading.h"

//-----------------------------------------------------------------------------
// Internal thread functions
//-----------------------------------------------------------------------------
void Sys_SetThreadPriority( threadHandle_t threadHandle, threadPriority_t threadPriority );
void Sys_SetThreadName( threadHandle_t threadHandle, const achar* pThreadName );