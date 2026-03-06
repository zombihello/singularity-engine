#pragma once
#include "tier0/memalloc_base.h"

//-----------------------------------------------------------------------------
// Std C memory allocator
//-----------------------------------------------------------------------------
class CMemAllocStd : public CMemAllocBase
{
public:
	CMemAllocStd();

	// IMemAlloc interface
	virtual void* TryMalloc( size numBytes, uint32 alignment = 0 ) override;
	virtual void* TryRealloc( void* pOriginal, size numBytes, uint32 alignment = 0 ) override;
	virtual void  Free( void* pOriginal ) override;

	virtual bool GetAllocationSize( void* pOriginal, size& numBytes ) const override;

	// CMemAllocBase interface
	virtual bool IsThreadSafe() const override;
};
