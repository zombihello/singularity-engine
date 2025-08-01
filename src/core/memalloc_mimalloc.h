#pragma once
#include "core/memalloc_base.h"
#include "core/defines.h"

//-----------------------------------------------------------------------------
// Mimalloc memory allocator
//-----------------------------------------------------------------------------

#if PLATFORM_SUPPORTS_MIMALLOC
class CMemAllocMimalloc : public CMemAllocBase
{
public:
	CMemAllocMimalloc();

	// IMemAlloc interface
	virtual void* Malloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;
	virtual void* TryMalloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;
	virtual void* Realloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;
	virtual void* TryRealloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;
	virtual void Free( void* pOriginal ) override;
	virtual void Trim( bool bTrimThreadCaches ) override;

	virtual bool GetAllocationSize( void* pOriginal, size_t& numBytes ) override;

	// CMemAllocBase interafce
	virtual bool IsInternallyThreadSafe() const override;
};
#endif // PLATFORM_SUPPORTS_MIMALLOC