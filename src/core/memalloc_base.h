#ifndef MEMALLOC_BASE_H
#define MEMALLOC_BASE_H

#include "core/memory.h"

//-----------------------------------------------------------------------------
// Base class for the global memory allocator
//-----------------------------------------------------------------------------
class CMemAllocBase : public IMemAlloc, public CUseSystemMemAllocForNew
{
public:
	// IMemAlloc interface
	virtual void* TryMalloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;
	void* TryRealloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;
	virtual void Trim( bool bTrimThreadCaches ) override;

	virtual bool IsInternallyThreadSafe() const;
	virtual bool GetAllocationSize( void* pOriginal, size_t& numBytes ) override;
};

#endif // !MEMALLOC_BASE_H