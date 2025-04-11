#include "pch_core.h"
#include "core/memalloc_std.h"
#include "core/memalloc_mimalloc.h"
#include "core/memalloc_threadsafeproxy.h"

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
#endif // PLATFORM_SUPPORTS_MIMALLOC

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
	static IMemAlloc*	s_pMemAlloc = nullptr;
	
	// If the global memory allocator not created yet then do it now!
	if ( !s_pMemAlloc )
	{
		// Allocate a new memory allocator
		CMemAllocBase*	pMemAlloc = AllocateMemAlloc();

		// If the allocator is already thread safe, there is no need for the thread safe proxy
		if ( !pMemAlloc->IsInternallyThreadSafe() )
		{
			pMemAlloc = new CMemAllocThreadSafeProxy( pMemAlloc );
		}

		// Remember our the global memory allocator
		s_pMemAlloc = pMemAlloc;
	}

	return s_pMemAlloc;
}

/*
==================
Sys_OutOfMemory
==================
*/
void Sys_OutOfMemory( size_t numBytes, uint32 alignment )
{
	Sys_Error( "Ran out of memory allocating %llu bytes with alignment %u", numBytes, alignment );
}