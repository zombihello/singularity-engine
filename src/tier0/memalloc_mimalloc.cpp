#include "pch_tier0.h"

#if PLATFORM_SUPPORTS_MIMALLOC
	#include <mimalloc.h>
	#include "tier0/memalloc_mimalloc.h"

static const char* s_pMemAllocName = "MemAlloc Mimalloc";

/*
==================
CMemAllocMimalloc::TryMalloc
==================
*/
void* CMemAllocMimalloc::TryMalloc( size numBytes, uint32 alignment /*= 0*/ )
{
	alignment	  = GetAlignment( numBytes, alignment );
	void* pNewPtr = mi_malloc_aligned( numBytes, alignment );
	if ( pNewPtr )
	{
		PROFILER_MEM_ALLOC( pNewPtr, S_Align( numBytes, alignment ), s_pMemAllocName );
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
		PROFILER_MEM_FREE( pOriginal, s_pMemAllocName );
	}
	if ( pNewPtr )
	{
		PROFILER_MEM_ALLOC( pNewPtr, S_Align( numBytes, alignment ), s_pMemAllocName );
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
	if ( pOriginal )
	{
		mi_free( pOriginal );
		PROFILER_MEM_FREE( pOriginal, s_pMemAllocName );
	}
}

/*
==================
CMemAllocMimalloc::Trim
==================
*/
void CMemAllocMimalloc::Trim( bool bTrimThreadCaches )
{
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
	numBytes = mi_malloc_size( pOriginal );
	return true;
}
#endif	// PLATFORM_SUPPORTS_MIMALLOC
