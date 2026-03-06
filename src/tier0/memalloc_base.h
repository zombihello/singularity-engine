#pragma once
#include "tier0/memory.h"

//-----------------------------------------------------------------------------
// Base class for the global memory allocator
//-----------------------------------------------------------------------------
enum
{
	DEFAULT_ALIGNMENT = 16,
	MIN_ALIGNMENT	  = 8
};

class CMemAllocBase : public IMemAlloc, public CUseSystemMemAllocForNew
{
public:
	// IMemAlloc interface
	virtual void* Malloc( size numBytes, uint32 alignment = 0 ) override;
	virtual void* Realloc( void* pOriginal, size numBytes, uint32 alignment = 0 ) override;
	virtual void  Trim( bool bTrimThreadCaches ) override;

	virtual bool IsThreadSafe() const;
	virtual bool GetAllocationSize( void* pOriginal, size& numBytes ) const override;

protected:
	uint32 GetAlignment( size numBytes, uint32 alignment ) const;

private:
	void OutOfMemory( size numBytes, uint32 alignment ) const;
};

#include "tier0/memalloc_base.inl"
