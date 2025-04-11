/**
 * @file
 * @addtogroup core core
 */

#ifndef MEMALLOC_MIMALLOC_H
#define MEMALLOC_MIMALLOC_H

#include "core/memalloc_base.h"
#include "core/platform_private.h"

#if PLATFORM_SUPPORTS_MIMALLOC
/**
 * @ingroup core
 * @brief Mimalloc memory allocator
 */
class CMemAllocMimalloc : public CMemAllocBase
{
public:
	/**
	 * @brief Constructor
	 */
	CMemAllocMimalloc();

	/**
	 * @brief Allocates numBytes bytes of uninitialized storage
	 *
	 * @param numBytes		Number of bytes to allocate. An integral multiple of alignment
	 * @param alignment		Specifies the alignment. Must be a valid alignment supported by the implementation
	 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free() or Realloc(). On failure, returns a NULL pointer
	 */
	virtual void* Malloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;

	/**
	 * @brief Try allocates numBytes bytes of uninitialized storage
	 *
	 * @param numBytes		Number of bytes to allocate. An integral multiple of alignment
	 * @param alignment		Specifies the alignment. Must be a valid alignment supported by the implementation
	 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free() or Realloc(). On failure, returns a NULL pointer
	 */
	virtual void* TryMalloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;

	/**
	 * @brief Reallocates the given area of memory. It must be previously allocated by Malloc or MallocZeroed and not yet freed with Free, otherwise, the results are undefined
	 *
	 * @param pOriginal		Pointer to the memory area to be reallocated
	 * @param numBytes		New size of the array
	 * @param alignment		Alignment
	 * @return On success, returns a pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free or Realloc. The original pointer pOriginal is invalidated and any access to it is undefined behavior (even if reallocation was in-place). On failure, returns a null pointer. The original pointer pOriginal remains valid and may need to be deallocated with Free
	 */
	virtual void* Realloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;

	/**
	 * @brief Try reallocates the given area of memory. It must be previously allocated by Malloc or MallocZeroed and not yet freed with Free, otherwise, the results are undefined
	 *
	 * @param pOriginal		Pointer to the memory area to be reallocated
	 * @param numBytes		New size of the array
	 * @param alignment		Alignment
	 * @return On success, returns a pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free or Realloc. The original pointer pOriginal is invalidated and any access to it is undefined behavior (even if reallocation was in-place). On failure, returns a null pointer. The original pointer pOriginal remains valid and may need to be deallocated with Free
	 */
	virtual void* TryRealloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;

	/**
	 * @brief Deallocates the space previously allocated by Malloc or Realloc
	 * @param pOriginal		Pointer to the memory to deallocate
	 */
	virtual void Free( void* pOriginal ) override;

	/**
	* @brief If possible determine the size of the memory allocated at the given address
	*
	* @param pOriginal		Pointer to memory we are checking the size of
	* @param numBytes		If possible, this value is set to the size of the passed in pointer
	* @return Return TRUE if succeeded, otherwise returns FALSE
	*/
	virtual bool GetAllocationSize( void* pOriginal, size_t& numBytes ) override;

	/**
	* @brief Releases as much memory as possible. Must be called from the main thread
	* @param bTrimThreadCaches		Is need trim thread caches
	*/
	virtual void Trim( bool bTrimThreadCaches ) override;

	/**
	 * @brief Returns if the allocator is guaranteed to be thread-safe and therefore
	 * doesn't need a unnecessary thread-safety wrapper around it
	 * 
	 * @return Return TRUE if the allocator is guaranteed to be thread-safe, otherwise returns FALSE
	 */
	virtual bool IsInternallyThreadSafe() const override;
};
#endif // PLATFORM_SUPPORTS_MIMALLOC

#endif // !MEMALLOC_MIMALLOC_H