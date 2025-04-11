/**
 * @file
 * @addtogroup core core
 */

#ifndef BUILD_H
#define BUILD_H

// Define macroses PLATFORM_XXXBIT if they not defined
#ifndef PLATFORM_32BIT
	#define PLATFORM_32BIT 0
#endif // !PLATFORM_32BIT

#ifndef PLATFORM_64BIT
	#define PLATFORM_64BIT 0
#endif // !PLATFORM_64BIT

// Enable or disable asserts in a build
#ifndef ENABLE_ASSERT
	#define ENABLE_ASSERT !RETAIL
#endif // !ENABLE_ASSERT

// Enable or disable logging in a build
#ifndef ENABLE_LOGGING
	#define ENABLE_LOGGING !RETAIL
#endif // !ENABLE_LOGGING

// Enable or disable profiling in a build
#ifndef ENABLE_PROFILING
	#define ENABLE_PROFILING !RETAIL
#endif // !ENABLE_PROFILING


#endif // !BUILD_H