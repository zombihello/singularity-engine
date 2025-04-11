/**
 * @file
 * @addtogroup core core
 */

#ifndef THREADING_PRIVATE_H
#define THREADING_PRIVATE_H

#include "core/threading.h"

/**
 * @ingroup core
 * @brief Set thread priority
 * @note Need implement on each platform
 * 
 * @param threadHandle		OS thread handle
 * @param threadPriority	Thread priority
 */
void Sys_SetThreadPriority( threadHandle_t threadHandle, threadPriority_t threadPriority );

/**
 * @ingroup core
 * @brief Set thread name
 * @note Need implement on each platform
 * 
 * @param threadHandle		OS thread handle
 * @param pThreadName		Thread name
 */
void Sys_SetThreadName( threadHandle_t threadHandle, const achar* pThreadName );

#endif // !THREADING_PRIVATE_H