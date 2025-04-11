/**
 * @file
 * @addtogroup core core
 */

#ifndef TYPES_H
#define TYPES_H

#include "core/build.h"

/**
 * @ingroup core
 * @brief Signed int 8-bit
 */
typedef	signed char			int8;

/**
 * @ingroup core
 * @brief Signed int 16-bit
 */
typedef	short				int16;

/**
 * @ingroup core
 * @brief Signed int 32-bit
 */
typedef	int					int32;

/**
 * @ingroup core
 * @brief Signed int 64-bit
 */
typedef	long long			int64;

/**
 * @ingroup core
 * @brief Unsigned int 8-bit
 */
typedef	unsigned char		uint8;

/**
 * @ingroup core
 * @brief Unsigned int 16-bit
 */
typedef	unsigned short		uint16;

/**
 * @ingroup core
 * @brief Unsigned int 32-bit
 */
typedef	unsigned int		uint32;

/**
 * @ingroup core
 * @brief Unsigned int 64-bit
 */
typedef	unsigned long long	uint64;

/**
 * @ingroup core
 * @brief Unsigned int 8-bit
 */
typedef	uint8		        byte;

/**
 * @ingroup core
 * @brief ANSI char
 */
typedef char				achar;

/**
 * @ingroup core
 * @brief Unicode char
 */
typedef wchar_t				wchar;

/**
 * @ingroup core
 * @brief Hash type
 */
typedef uint64				hash_t;

#if PLATFORM_64BIT
	/**
	 * @ingroup core
	 * @brief Pointer type
	 */
	typedef int64           ptrint;

	/**
	 * @ingroup core
	 * @brief Unsigned pointer type
	 */
	typedef uint64          uptrint;
#elif PLATFORM_32BIT
	typedef int32           ptrint;
	typedef uint32          uptrint;
#else
	#error Unknown platform bit
#endif // PLATFORM_64BIT || PLATFORM_32BIT

/**
 * @ingroup core
 */
enum
{
	INVALID_HASH	= ( hash_t )-1,		/**< Invalid hash */
	INVALID_INDEX	= ( uint32 )-1		/**< Invalid index */
};

#endif // !TYPES_H