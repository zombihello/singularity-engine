#pragma once
#include "tier0/memalloc_base.h"
#include "tier1/threading.h"

//-----------------------------------------------------------------------------
// A memory allocator proxy that synchronizes access, making the used the one thread safe
//-----------------------------------------------------------------------------
class CMemAllocThreadSafeProxy : public CMemAllocBase
{
public:
	CMemAllocThreadSafeProxy( IMemAlloc* pMemAlloc )
		: pUsedMemAlloc( pMemAlloc )
	{
	}

	// IMemAlloc interface
	virtual void* Malloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;
	virtual void* Realloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;
	virtual void  Free( void* pOriginal ) override;
	virtual void  Trim( bool bTrimThreadCaches ) override;

	virtual bool GetAllocationSize( void* pOriginal, size_t& numBytes ) override;

	// CMemAllocBase interface
	virtual bool IsInternallyThreadSafe() const override;

private:
	IMemAlloc*			 pUsedMemAlloc;
	mutable CThreadMutex synchronizationObject;
};
