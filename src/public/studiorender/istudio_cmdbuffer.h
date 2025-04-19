#ifndef ISTUDIO_CMDBUFFER_H
#define ISTUDIO_CMDBUFFER_H

#include "core/types.h"
#include "core/platform.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioCmdBuffer;


//-----------------------------------------------------------------------------
// Studio render command buffer interface
//-----------------------------------------------------------------------------
struct studioCmdAlloc_t
{
	byte*	pAllocation;
	uint32	allocatedSize;
};


class IStudioCmdBuffer
{
public:
	// Allocate a chunk
	// NOTE: After filling in the allocated chunk you have to call CommitAllocation
	virtual studioCmdAlloc_t GetAllocation( uint32 allocationSize ) = 0;

	virtual void CommitAllocation( studioCmdAlloc_t& allocContext ) = 0;
	virtual void Flush() = 0;
};

#endif // !ISTUDIO_CMDBUFFER_H