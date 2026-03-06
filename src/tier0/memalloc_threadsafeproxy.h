#pragma once
#include "tier0/memalloc_base.h"
#include "tier1/threading.h"

//-----------------------------------------------------------------------------
// A memory allocator proxy that synchronizes access, making the used the one thread safe
//-----------------------------------------------------------------------------
class CMemAllocThreadSafeProxy : public CMemAllocBase
{
public:
	CMemAllocThreadSafeProxy( IMemAlloc* pMemAlloc );

	// IMemAlloc interface
	virtual void* TryMalloc( size numBytes, uint32 alignment = 0 ) override;
	virtual void* TryRealloc( void* pOriginal, size numBytes, uint32 alignment = 0 ) override;
	virtual void  Free( void* pOriginal ) override;
	virtual void  Trim( bool bTrimThreadCaches ) override;

	virtual bool GetAllocationSize( void* pOriginal, size& numBytes ) const override;

	// CMemAllocBase interface
	virtual bool IsThreadSafe() const override;

private:
	IMemAlloc*			 pUsedMemAlloc;
	mutable CThreadMutex synchronizationObject;
};
