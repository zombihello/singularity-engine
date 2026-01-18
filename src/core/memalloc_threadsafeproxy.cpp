#include "pch_core.h"
#include "core/memalloc_threadsafeproxy.h"

/*
 ==================
 CMemAllocThreadSafeProxy::Malloc
 ==================
 */
void* CMemAllocThreadSafeProxy::Malloc( size_t numBytes, uint32 alignment /*= DEFAULT_ALIGNMENT*/ )
{
	CScopeLock scopeLock( &synchronizationObject );
	return pUsedMemAlloc->Malloc( numBytes, alignment );
}

/*
 ==================
 CMemAllocThreadSafeProxy::Malloc
 ==================
 */
void* CMemAllocThreadSafeProxy::Realloc( void* pOriginal, size_t numBytes, uint32 alignment /*= DEFAULT_ALIGNMENT*/ )
{
	CScopeLock scopeLock( &synchronizationObject );
	return pUsedMemAlloc->Realloc( pOriginal, numBytes, alignment );
}

/*
 ==================
 CMemAllocThreadSafeProxy::Malloc
 ==================
 */
void CMemAllocThreadSafeProxy::Free( void* pOriginal )
{
	if ( pOriginal )
	{
		CScopeLock scopeLock( &synchronizationObject );
		pUsedMemAlloc->Free( pOriginal );
	}
}

/*
 ==================
 CMemAllocThreadSafeProxy::Malloc
 ==================
 */
bool CMemAllocThreadSafeProxy::GetAllocationSize( void* pOriginal, size_t& numBytes )
{
	CScopeLock scopeLock( &synchronizationObject );
	return pUsedMemAlloc->GetAllocationSize( pOriginal, numBytes );
}

/*
 ==================
 CMemAllocThreadSafeProxy::Malloc
 ==================
 */
void CMemAllocThreadSafeProxy::Trim( bool bTrimThreadCaches )
{
	CScopeLock scopeLock( &synchronizationObject );
	pUsedMemAlloc->Trim( bTrimThreadCaches );
}

/*
 ==================
 CMemAllocThreadSafeProxy::Malloc
 ==================
 */
bool CMemAllocThreadSafeProxy::IsInternallyThreadSafe() const
{
	return true;
}
