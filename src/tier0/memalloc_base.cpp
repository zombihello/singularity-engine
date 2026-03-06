#include "pch_tier0.h"
#include "tier0/memalloc_base.h"

/*
 ==================
 CMemAllocBase::Malloc
 ==================
 */
void* CMemAllocBase::Malloc( size numBytes, uint32 alignment /*= 0*/ )
{
	void* pResult = TryMalloc( numBytes, alignment );
	if ( !pResult && numBytes )
	{
		OutOfMemory( numBytes, alignment );
	}

	return pResult;
}

/*
 ==================
 CMemAllocBase::Realloc
 ==================
 */
void* CMemAllocBase::Realloc( void* pOriginal, size numBytes, uint32 alignment /*= 0*/ )
{
	void* pResult = TryRealloc( pOriginal, numBytes, alignment );
	if ( !pResult && numBytes )
	{
		OutOfMemory( numBytes, alignment );
	}

	return pResult;
}

/*
==================
CMemAllocBase::Trim
==================
*/
void CMemAllocBase::Trim( bool bTrimThreadCaches )
{
}

/*
 ==================
 CMemAllocBase::IsThreadSafe
 ==================
 */
bool CMemAllocBase::IsThreadSafe() const
{
	return false;
}

/*
 ==================
 CMemAllocBase::GetAllocationSize
 ==================
 */
bool CMemAllocBase::GetAllocationSize( void* pOriginal, size& numBytes ) const
{
	// Default implementation has no way of determining this
	return false;
}
