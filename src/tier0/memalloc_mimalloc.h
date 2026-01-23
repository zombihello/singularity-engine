#pragma once
#include "tier0/memalloc_base.h"
#include "tier0/defines_internal.h"

//-----------------------------------------------------------------------------
// Mimalloc memory allocator
//-----------------------------------------------------------------------------

#if PLATFORM_SUPPORTS_MIMALLOC
class CMemAllocMimalloc : public CMemAllocBase
{
public:
	// IMemAlloc interface
	virtual void* Malloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;
	virtual void* TryMalloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;
	virtual void* Realloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;
	virtual void* TryRealloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;
	virtual void  Free( void* pOriginal ) override;
	virtual void  Trim( bool bTrimThreadCaches ) override;

	virtual bool GetAllocationSize( void* pOriginal, size_t& numBytes ) override;

	// CMemAllocBase interface
	virtual bool IsInternallyThreadSafe() const override;
};
#endif	// PLATFORM_SUPPORTS_MIMALLOC
