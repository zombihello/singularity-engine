#pragma once
#include "core/memalloc_base.h"

//-----------------------------------------------------------------------------
// Std C memory allocator
//-----------------------------------------------------------------------------
class CMemAllocStd : public CMemAllocBase
{
public:
	CMemAllocStd();

	// IMemAlloc interface
	virtual void* Malloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;
	virtual void* TryMalloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;
	virtual void* Realloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;
	virtual void* TryRealloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;
	virtual void  Free( void* pOriginal ) override;

	virtual bool GetAllocationSize( void* pOriginal, size_t& numBytes ) override;

	// CMemAllocBase interface
	virtual bool IsInternallyThreadSafe() const override;
};
