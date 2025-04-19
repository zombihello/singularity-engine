#include "pch_core.h"
#include "core/memalloc_base.h"

/*
 ==================
 CMemAllocBase::TryMalloc
 ==================
 */
void* CMemAllocBase::TryMalloc( size_t numBytes, uint32 alignment /*= DEFAULT_ALIGNMENT*/ )
{
	return Malloc( numBytes, alignment );
}

/*
 ==================
 CMemAllocBase::TryRealloc
 ==================
 */
void* CMemAllocBase::TryRealloc( void* pOriginal, size_t numBytes, uint32 alignment /*= DEFAULT_ALIGNMENT*/ )
{
	return Realloc( pOriginal, numBytes, alignment );
}

/*
 ==================
 CMemAllocBase::GetAllocationSize
 ==================
 */
bool CMemAllocBase::GetAllocationSize( void* pOriginal, size_t& numBytes )
{
	// Default implementation has no way of determining this
	return false;
}

/*
 ==================
 CMemAllocBase::Trim
 ==================
 */
void CMemAllocBase::Trim( bool bTrimThreadCaches )
{}

/*
 ==================
 CMemAllocBase::IsInternallyThreadSafe
 ==================
 */
bool CMemAllocBase::IsInternallyThreadSafe() const
{
	return false;
}