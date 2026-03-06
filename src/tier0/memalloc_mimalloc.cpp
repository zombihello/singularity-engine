#include "pch_tier0.h"

#if PLATFORM_SUPPORTS_MIMALLOC
	#include <mimalloc.h>
	#include "tier0/memalloc_mimalloc.h"

	#define MEMALLOC_MIMALLOC_NAME "MemAlloc Mimalloc"

/*
==================
CMemAllocMimalloc::TryMalloc
==================
*/
void* CMemAllocMimalloc::TryMalloc( size numBytes, uint32 alignment /*= 0*/ )
{
	PROFILE_SCOPE()
	alignment	  = GetAlignment( numBytes, alignment );
	void* pNewPtr = mi_malloc_aligned( numBytes, alignment );
	if ( pNewPtr )
	{
		// TODO BS yehor.pohuliaka - CIT-13 Integrate Tracy profiler
		// PROFILER_MEM_ALLOC( pNewPtr, Align( numBytes, alignment ), MEMALLOC_MIMALLOC_NAME );
	}
	return pNewPtr;
}

/*
==================
CMemAllocMimalloc::TryRealloc
==================
*/
void* CMemAllocMimalloc::TryRealloc( void* pOriginal, size numBytes, uint32 alignment /*= 0*/ )
{
	PROFILE_SCOPE()
	alignment	  = GetAlignment( numBytes, alignment );
	void* pNewPtr = NULL;

	if ( pOriginal && numBytes )
	{
		pNewPtr = mi_realloc_aligned( pOriginal, numBytes, alignment );
	}
	else if ( !pOriginal )
	{
		pNewPtr = mi_malloc_aligned( numBytes, alignment );
	}
	else
	{
		mi_free( pOriginal );
	}

	if ( pOriginal )
	{
		// TODO BS yehor.pohuliaka - CIT-13 Integrate Tracy profiler
		// PROFILER_MEM_FREE( pOriginal, MEMALLOC_MIMALLOC_NAME );
	}
	if ( pNewPtr )
	{
		// TODO BS yehor.pohuliaka - CIT-13 Integrate Tracy profiler
		// PROFILER_MEM_ALLOC( pNewPtr, Align( numBytes, alignment ), MEMALLOC_MIMALLOC_NAME );
	}
	return pNewPtr;
}

/*
==================
CMemAllocMimalloc::Free
==================
*/
void CMemAllocMimalloc::Free( void* pOriginal )
{
	PROFILE_SCOPE()
	if ( pOriginal )
	{
		mi_free( pOriginal );
		// TODO BS yehor.pohuliaka - CIT-13 Integrate Tracy profiler
		// PROFILER_MEM_FREE( pOriginal, MEMALLOC_MIMALLOC_NAME );
	}
}

/*
==================
CMemAllocMimalloc::Trim
==================
*/
void CMemAllocMimalloc::Trim( bool bTrimThreadCaches )
{
	PROFILE_SCOPE()
	mi_collect( bTrimThreadCaches );
}

/*
==================
CMemAllocMimalloc::IsThreadSafe
==================
*/
bool CMemAllocMimalloc::IsThreadSafe() const
{
	return true;
}

/*
==================
CMemAllocMimalloc::GetAllocationSize
==================
*/
bool CMemAllocMimalloc::GetAllocationSize( void* pOriginal, size& numBytes ) const
{
	PROFILE_SCOPE()
	numBytes = mi_malloc_size( pOriginal );
	return true;
}
#endif	// PLATFORM_SUPPORTS_MIMALLOC
