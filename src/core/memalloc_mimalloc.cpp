#include "pch_core.h"
#include "core/memalloc_mimalloc.h"

#if PLATFORM_SUPPORTS_MIMALLOC
#include <mimalloc.h>

/*
==================
CMemAllocMimalloc::Malloc
==================
*/
void* CMemAllocMimalloc::Malloc( size_t numBytes, uint32 alignment /* = DEFAULT_ALIGNMENT */ )
{
	PROFILE_SCOPE()
	void*	pResult = TryMalloc( numBytes, alignment );
	if ( !pResult && numBytes )
	{
		Sys_OutOfMemory( numBytes, alignment );
	}
	return pResult;
}

/*
==================
CMemAllocMimalloc::TryMalloc
==================
*/
void* CMemAllocMimalloc::TryMalloc( size_t numBytes, uint32 alignment /* = DEFAULT_ALIGNMENT */ )
{
	PROFILE_SCOPE()
	void*	pNewPtr = nullptr;
	if ( alignment != DEFAULT_ALIGNMENT )
	{
		pNewPtr = mi_malloc_aligned( numBytes, Max<uint32>( numBytes >= 16 ? 16 : 8, alignment ) );
	}
	else
	{
		pNewPtr = mi_malloc_aligned( numBytes, numBytes >= 16 ? 16 : 8 );
	}
	return pNewPtr;
}

/*
==================
CMemAllocMimalloc::Realloc
==================
*/
void* CMemAllocMimalloc::Realloc( void* pOriginal, size_t numBytes, uint32 alignment /* = DEFAULT_ALIGNMENT */ )
{
	PROFILE_SCOPE()
	void*	pResult = TryRealloc( pOriginal, numBytes, alignment );
	if ( !pResult && numBytes )
	{
		Sys_OutOfMemory( numBytes, alignment );
	}

	return pResult;
}

/*
==================
CMemAllocMimalloc::TryRealloc
==================
*/
void* CMemAllocMimalloc::TryRealloc( void* pOriginal, size_t numBytes, uint32 alignment /* = DEFAULT_ALIGNMENT */ )
{
	PROFILE_SCOPE()
	void*	pNewPtr = nullptr;
	if ( numBytes == 0 )
	{
		mi_free( pOriginal );
		return nullptr;
	}

	if ( alignment != DEFAULT_ALIGNMENT )
	{
		pNewPtr = mi_realloc_aligned( pOriginal, numBytes, Max<uint32>( numBytes >= 16 ? 16 : 8, alignment ) );
	}
	else
	{
		pNewPtr = mi_realloc( pOriginal, numBytes );
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
	if ( !pOriginal )
	{
		return;
	}

	mi_free( pOriginal );
}

/*
==================
CMemAllocMimalloc::GetAllocationSize
==================
*/
bool CMemAllocMimalloc::GetAllocationSize( void* pOriginal, size_t& numBytes )
{
	PROFILE_SCOPE()
	numBytes = mi_malloc_size( pOriginal );
	return true;
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
CMemAllocMimalloc::IsInternallyThreadSafe
==================
*/
bool CMemAllocMimalloc::IsInternallyThreadSafe() const
{
	return true;
}
#endif // PLATFORM_SUPPORTS_MIMALLOC