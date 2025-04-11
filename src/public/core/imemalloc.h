/**
 * @file
 * @addtogroup core core
 */

#ifndef IMEMALLOC_H
#define IMEMALLOC_H

#include "core/coreapi.h"
#include "core/types.h"

/**
 * @ingroup core
 */
enum
{
	DEFAULT_ALIGNMENT	= 0,		/**< Default allocator alignment. If the default is specified, the allocator applies to engine rules. Blocks >= 16 bytes will be 16-byte-aligned, Blocks < 16 will be 8-byte aligned. If the allocator does not support allocation alignment, the alignment will be ignored */
	MIN_ALIGNMENT		= 8			/**< Minimum allocator alignment */
};

/**
 * @ingroup core
 * @brief The memory allocator's interface
 */
class IMemAlloc
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IMemAlloc() {}

	/**
	 * @brief Allocates numBytes bytes of uninitialized storage
	 *
	 * @param numBytes		Number of bytes to allocate. An integral multiple of alignment
	 * @param alignment		Specifies the alignment. Must be a valid alignment supported by the implementation
	 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free() or Realloc(). On failure, returns a NULL pointer
	 */
	virtual void* Malloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) = 0;

	/**
	 * @brief Try allocates numBytes bytes of uninitialized storage
	 *
	 * @param numBytes		Number of bytes to allocate. An integral multiple of alignment
	 * @param alignment		Specifies the alignment. Must be a valid alignment supported by the implementation
	 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free() or Realloc(). On failure, returns a NULL pointer
	 */
	virtual void* TryMalloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) = 0;

	/**
	 * @brief Reallocates the given area of memory. It must be previously allocated by Malloc or MallocZeroed and not yet freed with Free, otherwise, the results are undefined
	 *
	 * @param pOriginal		Pointer to the memory area to be reallocated
	 * @param numBytes		New size of the array
	 * @param alignment		Alignment
	 * @return On success, returns a pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free or Realloc. The original pointer pOriginal is invalidated and any access to it is undefined behavior (even if reallocation was in-place). On failure, returns a null pointer. The original pointer pOriginal remains valid and may need to be deallocated with Free
	 */
	virtual void* Realloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) = 0;

	/**
	 * @brief Try reallocates the given area of memory. It must be previously allocated by Malloc or MallocZeroed and not yet freed with Free, otherwise, the results are undefined
	 *
	 * @param pOriginal		Pointer to the memory area to be reallocated
	 * @param numBytes		New size of the array
	 * @param alignment		Alignment
	 * @return On success, returns a pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free or Realloc. The original pointer pOriginal is invalidated and any access to it is undefined behavior (even if reallocation was in-place). On failure, returns a null pointer. The original pointer pOriginal remains valid and may need to be deallocated with Free
	 */
	virtual void* TryRealloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) = 0;

	/**
	 * @brief Deallocates the space previously allocated by Malloc or Realloc
	 * @param pOriginal		Pointer to the memory to deallocate
	 */
	virtual void Free( void* pOriginal ) = 0;

	/**
	* @brief If possible determine the size of the memory allocated at the given address
	*
	* @param pOriginal		Pointer to memory we are checking the size of
	* @param numBytes		If possible, this value is set to the size of the passed in pointer
	* @return Return TRUE if succeeded, otherwise returns FALSE
	*/
	virtual bool GetAllocationSize( void* pOriginal, size_t& numBytes ) = 0;

	/**
	* @brief Releases as much memory as possible. Must be called from the main thread
	* @param bTrimThreadCaches		Is need trim thread caches
	*/
	virtual void Trim( bool bTrimThreadCaches ) = 0;
};

/**
 * @ingroup core
 * @brief Get the global memory allocator
 * @return Return the global memory allocator
 */
CORE_INTERFACE IMemAlloc* MemAlloc();

#endif // !IMEMALLOC_H