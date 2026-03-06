#pragma once
#include "tier0/tier0_api.h"
#include "tier0/types.h"

//-----------------------------------------------------------------------------
// The memory allocator's interface
//-----------------------------------------------------------------------------
class IMemAlloc
{
public:
	virtual ~IMemAlloc() {}

	virtual void* Malloc( size numBytes, uint32 alignment = 0 )						 = 0;
	virtual void* Realloc( void* pOriginal, size numBytes, uint32 alignment = 0 )	 = 0;
	virtual void* TryMalloc( size numBytes, uint32 alignment = 0 )					 = 0;
	virtual void* TryRealloc( void* pOriginal, size numBytes, uint32 alignment = 0 ) = 0;
	virtual void  Free( void* pOriginal )											 = 0;
	virtual void  Trim( bool bTrimThreadCaches )									 = 0;

	virtual bool GetAllocationSize( void* pOriginal, size& numBytes ) const = 0;
};

TIER0_INTERFACE IMemAlloc* MemAlloc();
