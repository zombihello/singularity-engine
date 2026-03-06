#include "pch_tier0.h"
#include "tier0/memalloc_threadsafeproxy.h"

/*
 ==================
 CMemAllocThreadSafeProxy::CMemAllocThreadSafeProxy
 ==================
 */
CMemAllocThreadSafeProxy::CMemAllocThreadSafeProxy( IMemAlloc* pMemAlloc )
	: pUsedMemAlloc( pMemAlloc )
{
}

/*
 ==================
 CMemAllocThreadSafeProxy::TryMalloc
 ==================
 */
void* CMemAllocThreadSafeProxy::TryMalloc( size numBytes, uint32 alignment /*= 0*/ )
{
	CScopeLock scopeLock( synchronizationObject );
	return pUsedMemAlloc->TryMalloc( numBytes, alignment );
}

/*
 ==================
 CMemAllocThreadSafeProxy::TryRealloc
 ==================
 */
void* CMemAllocThreadSafeProxy::TryRealloc( void* pOriginal, size numBytes, uint32 alignment /*= 0*/ )
{
	CScopeLock scopeLock( synchronizationObject );
	return pUsedMemAlloc->TryRealloc( pOriginal, numBytes, alignment );
}

/*
 ==================
 CMemAllocThreadSafeProxy::Free
 ==================
 */
void CMemAllocThreadSafeProxy::Free( void* pOriginal )
{
	CScopeLock scopeLock( synchronizationObject );
	pUsedMemAlloc->Free( pOriginal );
}

/*
 ==================
 CMemAllocThreadSafeProxy::Trim
 ==================
 */
void CMemAllocThreadSafeProxy::Trim( bool bTrimThreadCaches )
{
	CScopeLock scopeLock( synchronizationObject );
	pUsedMemAlloc->Trim( bTrimThreadCaches );
}

/*
 ==================
 CMemAllocThreadSafeProxy::GetAllocationSize
 ==================
 */
bool CMemAllocThreadSafeProxy::GetAllocationSize( void* pOriginal, size& numBytes ) const
{
	CScopeLock scopeLock( synchronizationObject );
	return pUsedMemAlloc->GetAllocationSize( pOriginal, numBytes );
}

/*
 ==================
 CMemAllocThreadSafeProxy::IsThreadSafe
 ==================
 */
bool CMemAllocThreadSafeProxy::IsThreadSafe() const
{
	return true;
}
