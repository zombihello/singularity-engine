#pragma once
#include "tier0/memalloc_base.h"

//-----------------------------------------------------------------------------
// Mimalloc memory allocator
//-----------------------------------------------------------------------------
#if PLATFORM_SUPPORTS_MIMALLOC
class CMemAllocMimalloc : public CMemAllocBase
{
public:
	// IMemAlloc interface
	virtual void* TryMalloc( size numBytes, uint32 alignment = 0 ) override;
	virtual void* TryRealloc( void* pOriginal, size numBytes, uint32 alignment = 0 ) override;
	virtual void  Free( void* pOriginal ) override;
	virtual void  Trim( bool bTrimThreadCaches ) override;

	virtual bool GetAllocationSize( void* pOriginal, size& numBytes ) const override;

	// CMemAllocBase interface
	virtual bool IsThreadSafe() const override;
};
#endif	// PLATFORM_SUPPORTS_MIMALLOC
