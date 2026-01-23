#include "pch_tier0.h"
#include "tier0/memalloc_std.h"

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
CMemAllocStd::Malloc
==================
*/
void* CMemAllocStd::Malloc( size_t numBytes, uint32 alignment /* = DEFAULT_ALIGNMENT */ )
{
	PROFILE_SCOPE()
	void* pResult = TryMalloc( numBytes, alignment );
	if ( !pResult && numBytes )
	{
		Sys_OutOfMemory( numBytes, alignment );
	}

	return pResult;
}

/*
==================
CMemAllocStd::TryMalloc
==================
*/
void* CMemAllocStd::TryMalloc( size_t numBytes, uint32 alignment /* = DEFAULT_ALIGNMENT */ )
{
	PROFILE_SCOPE()
	alignment	  = Max<uint32>( numBytes >= 16 ? 16 : 8, alignment );
	void* pResult = nullptr;

	// Allocate memory
#if PLATFORM_USE__ALIGNED_MALLOC
	pResult = _aligned_malloc( numBytes, alignment );
#else
	void* pPtr = L_malloc_system( numBytes + alignment + sizeof( void* ) + sizeof( size_t ) );
	if ( pPtr )
	{
		pResult																   = Align( (uint8*)pPtr + sizeof( void* ) + sizeof( size_t ), alignment );
		*( (void**)( (uint8*)pResult - sizeof( void* ) ) )					   = pPtr;
		*( (size_t*)( (uint8*)pResult - sizeof( void* ) - sizeof( size_t ) ) ) = numBytes;
	}
#endif	// PLATFORM_USE__ALIGNED_MALLOC

	return pResult;
}

/*
==================
CMemAllocStd::Realloc
==================
*/
void* CMemAllocStd::Realloc( void* pOriginal, size_t numBytes, uint32 alignment /* = DEFAULT_ALIGNMENT */ )
{
	PROFILE_SCOPE()
	void* pResult = TryRealloc( pOriginal, numBytes, alignment );
	if ( !pResult && numBytes != 0 )
	{
		Sys_OutOfMemory( numBytes, alignment );
	}

	return pResult;
}

/*
==================
CMemAllocStd::TryRealloc
==================
*/
void* CMemAllocStd::TryRealloc( void* pOriginal, size_t numBytes, uint32 alignment /* = DEFAULT_ALIGNMENT */ )
{
	PROFILE_SCOPE()
	alignment	  = Max<uint32>( numBytes >= 16 ? 16 : 8, alignment );
	void* pResult = nullptr;

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
		pResult = nullptr;
	}
#else
	if ( pOriginal && numBytes )
	{
		// Can't use realloc as it might screw with alignment
		pResult		   = TryMalloc( numBytes, alignment );
		size_t ptrSize = 0;
		bool   bResult = GetAllocationSize( pOriginal, ptrSize );
		Assert( ptrSize );
		L_memcpy( pResult, pOriginal, Min( numBytes, ptrSize ) );
		Free( pOriginal );
	}
	else if ( !pOriginal )
	{
		pResult = TryMalloc( numBytes, alignment );
	}
	else
	{
		L_free_system( *( (void**)( (uint8*)pOriginal - sizeof( void* ) ) ) );
		pResult = nullptr;
	}
#endif	// PLATFORM_USE__ALIGNED_MALLOC

	return pResult;
}

/*
==================
CMemAllocStd::Free
==================
*/
void CMemAllocStd::Free( void* pOriginal )
{
	PROFILE_SCOPE()
#if PLATFORM_USE__ALIGNED_MALLOC
	_aligned_free( pOriginal );
#else
	if ( pOriginal )
	{
		L_free_system( *( (void**)( (uint8*)pOriginal - sizeof( void* ) ) ) );
	}
#endif	// PLATFORM_USE__ALIGNED_MALLOC
}

/*
==================
CMemAllocStd::GetAllocationSize
==================
*/
bool CMemAllocStd::GetAllocationSize( void* pOriginal, size_t& numBytes )
{
	PROFILE_SCOPE()
	if ( !pOriginal )
	{
		return false;
	}

#if PLATFORM_USE__ALIGNED_MALLOC
	numBytes = _aligned_msize( pOriginal, 16, 0 );	// Assumes alignment of 16
	return true;
#else
	numBytes = *( (size_t*)( (uint8*)pOriginal - sizeof( void* ) - sizeof( size_t ) ) );
	return true;
#endif	// PLATFORM_USE__ALIGNED_MALLOC
}

/*
==================
CMemAllocStd::IsInternallyThreadSafe
==================
*/
bool CMemAllocStd::IsInternallyThreadSafe() const
{
#if PLATFORM_IS_ANSI_MALLOC_THREADSAFE
	return true;
#else
	return false;
#endif
}
