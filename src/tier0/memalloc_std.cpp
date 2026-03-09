#include "pch_tier0.h"
#include "tier0/memalloc_std.h"

static const char* s_pMemAllocName = "MemAlloc Std";

/*
==================
CMemAllocStd::CMemAllocStd
==================
*/
CMemAllocStd::CMemAllocStd()
{
#if PLATFORM_WINDOWS
	// Enable low fragmentation heap http://msdn2.microsoft.com/en-US/library/aa366750.aspx
	intptr_t pCRTHeapHandle = _get_heap_handle();
	ULONG	 enableLFH		= 2;
	HeapSetInformation( (void*)pCRTHeapHandle, HeapCompatibilityInformation, &enableLFH, sizeof( enableLFH ) );
#endif	// PLATFORM_WINDOWS
}

/*
==================
CMemAllocStd::TryMalloc
==================
*/
void* CMemAllocStd::TryMalloc( size numBytes, uint32 alignment /* = 0 */ )
{
	alignment	  = GetAlignment( numBytes, alignment );
	void* pResult = NULL;

	// Allocate memory
#if PLATFORM_USE__ALIGNED_MALLOC
	pResult = _aligned_malloc( numBytes, alignment );
#else
	void* pPtr = Mem_MallocSystem( numBytes + alignment + sizeof( void* ) + sizeof( size ) );
	if ( pPtr )
	{
		pResult															   = Align( (uint8*)pPtr + sizeof( void* ) + sizeof( size ), alignment );
		*( (void**)( (uint8*)pResult - sizeof( void* ) ) )				   = pPtr;
		*( (size*)( (uint8*)pResult - sizeof( void* ) - sizeof( size ) ) ) = numBytes;
	}
#endif	// PLATFORM_USE__ALIGNED_MALLOC

	if ( pResult )
	{
		PROFILER_MEM_ALLOC( pResult, Align( numBytes, alignment ), s_pMemAllocName );
	}
	return pResult;
}

/*
==================
CMemAllocStd::TryRealloc
==================
*/
void* CMemAllocStd::TryRealloc( void* pOriginal, size numBytes, uint32 alignment /* = 0 */ )
{
	alignment	  = GetAlignment( numBytes, alignment );
	void* pResult = NULL;

#if PLATFORM_USE__ALIGNED_MALLOC
	if ( pOriginal && numBytes )
	{
		pResult = _aligned_realloc( pOriginal, numBytes, alignment );
	}
	else if ( !pOriginal )
	{
		pResult = _aligned_malloc( numBytes, alignment );
	}
	else
	{
		_aligned_free( pOriginal );
	}
#else
	if ( pOriginal && numBytes )
	{
		// Can't use realloc as it might screw with alignment
		pResult		 = TryMalloc( numBytes, alignment );
		size ptrSize = 0;
		bool bResult = GetAllocationSize( pOriginal, ptrSize );
		Assert( ptrSize );
		Mem_Memcpy( pResult, pOriginal, Min( numBytes, ptrSize ) );
		Free( pOriginal );
	}
	else if ( !pOriginal )
	{
		pResult = TryMalloc( numBytes, alignment );
	}
	else
	{
		Mem_FreeSystem( *( (void**)( (uint8*)pOriginal - sizeof( void* ) ) ) );
	}
#endif	// PLATFORM_USE__ALIGNED_MALLOC

	if ( pOriginal )
	{
		PROFILER_MEM_FREE( pOriginal, s_pMemAllocName );
	}
	if ( pResult )
	{
		PROFILER_MEM_ALLOC( pResult, Align( numBytes, alignment ), s_pMemAllocName );
	}
	return pResult;
}

/*
==================
CMemAllocStd::Free
==================
*/
void CMemAllocStd::Free( void* pOriginal )
{
	if ( pOriginal )
	{
#if PLATFORM_USE__ALIGNED_MALLOC
		_aligned_free( pOriginal );
#else
		Mem_FreeSystem( *( (void**)( (uint8*)pOriginal - sizeof( void* ) ) ) );
#endif	// PLATFORM_USE__ALIGNED_MALLOC
		PROFILER_MEM_FREE( pOriginal, s_pMemAllocName );
	}
}

/*
==================
CMemAllocStd::GetAllocationSize
==================
*/
bool CMemAllocStd::GetAllocationSize( void* pOriginal, size& numBytes ) const
{
	if ( !pOriginal )
	{
		return false;
	}

#if PLATFORM_USE__ALIGNED_MALLOC
	numBytes = _aligned_msize( pOriginal, DEFAULT_ALIGNMENT, 0 );  // Assumes default alignment
	return true;
#else
	numBytes = *( (size*)( (uint8*)pOriginal - sizeof( void* ) - sizeof( size ) ) );
	return true;
#endif	// PLATFORM_USE__ALIGNED_MALLOC
}

/*
==================
CMemAllocStd::IsThreadSafe
==================
*/
bool CMemAllocStd::IsThreadSafe() const
{
#if PLATFORM_IS_ANSI_MALLOC_THREADSAFE
	return true;
#else
	return false;
#endif	// PLATFORM_IS_ANSI_MALLOC_THREADSAFE
}
