#pragma once
#include "core/coreapi.h"
#include "stdlib/types.h"

//-----------------------------------------------------------------------------
// The memory allocator's interface
//-----------------------------------------------------------------------------
enum
{
	DEFAULT_ALIGNMENT = 0,	// Default allocator alignment. If the default is specified, the allocator applies to engine rules. Blocks >= 16 bytes will be 16-byte-aligned, Blocks < 16 will be 8-byte aligned. If the allocator does not support allocation alignment, the alignment will be ignored
	MIN_ALIGNMENT	  = 8	// Minimum allocator alignment
};

class IMemAlloc
{
public:
	virtual ~IMemAlloc() {}

	virtual void* Malloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT )					   = 0;
	virtual void* TryMalloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT )				   = 0;
	virtual void* Realloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT )	   = 0;
	virtual void* TryRealloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) = 0;
	virtual void  Free( void* pOriginal )															   = 0;
	virtual void  Trim( bool bTrimThreadCaches )													   = 0;

	virtual bool GetAllocationSize( void* pOriginal, size_t& numBytes ) = 0;
};

CORE_INTERFACE IMemAlloc* MemAlloc();
