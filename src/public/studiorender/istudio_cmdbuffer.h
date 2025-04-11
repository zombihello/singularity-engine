/**
 * @file
 * @addtogroup studiorender studiorender
 */

#ifndef ISTUDIO_CMDBUFFER_H
#define ISTUDIO_CMDBUFFER_H

#include "core/types.h"
#include "core/platform.h"

// Forward declarations
class IStudioCmdBuffer;

/**
 * @ingroup studiorender
 * @brief A reference to an allocated chunk of IStudioCmdBuffer
 */
struct studioCmdAlloc_t
{
	byte*	pAllocation;	/**< Pointer to the begin allocation data */
	uint32	allocatedSize;	/**< Allocated size */
};

/**
 * @ingroup studiorender
 * @brief Studio render command buffer interface
 */
class IStudioCmdBuffer
{
public:
	/**
	 * @brief Allocate a chunk
	 * @warning After filling in the allocated chunk you have to call CommitAllocation
	 * 
	 * @param allocationSize	The size of the allocation to make.
	 * @return Return context of allocated chunk
	 */
	virtual studioCmdAlloc_t GetAllocation( uint32 allocationSize ) = 0;

	/**
	 * @brief Commit the allocated chunk
	 * @param allocContext	Allocated chunk context
	 */
	virtual void CommitAllocation( studioCmdAlloc_t& allocContext ) = 0;

	/**
	 * @brief Flush render commands
	 */
	virtual void Flush() = 0;
};

#endif // !ISTUDIO_CMDBUFFER_H