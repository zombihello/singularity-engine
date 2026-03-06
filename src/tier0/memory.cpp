#include "pch_tier0.h"
#include "tier0/memalloc_std.h"
#include "tier0/memalloc_mimalloc.h"
#include "tier0/memalloc_threadsafeproxy.h"

/*
==================
AllocateMemAlloc
==================
*/
static CMemAllocBase* AllocateMemAlloc()
{
#if PLATFORM_SUPPORTS_MIMALLOC
	// Mimalloc default allocator because it has great performance
	return new CMemAllocMimalloc();
#endif	// PLATFORM_SUPPORTS_MIMALLOC

	// Fallback allocator
	return new CMemAllocStd();
}

/*
==================
MemAlloc
==================
*/
IMemAlloc* MemAlloc()
{
	// If the global memory allocator not created yet then do it now!
	static IMemAlloc* s_pMemAlloc = NULL;
	if ( !s_pMemAlloc )
	{
		// Allocate a new memory allocator
		CMemAllocBase* pMemAlloc = AllocateMemAlloc();

		// If the allocator is already thread safe, there is no need for the thread safe proxy
		if ( !pMemAlloc->IsThreadSafe() )
		{
			pMemAlloc = new CMemAllocThreadSafeProxy( pMemAlloc );
		}

		// Remember our the global memory allocator
		s_pMemAlloc = pMemAlloc;
	}

	return s_pMemAlloc;
}
