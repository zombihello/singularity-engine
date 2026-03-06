#pragma once
#include <cstdlib>

#include "tier0/tier0_api.h"
#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier0/imemalloc.h"

//-----------------------------------------------------------------------------
// Constants and types
//-----------------------------------------------------------------------------
struct systemMemoryInfo_t
{
	uint64 totalSize;
	uint64 freeSize;
	uint64 usedSize;
};

struct appMemoryInfo_t
{
	uint64 usedSize;
};

//-----------------------------------------------------------------------------
// Memory functions
//-----------------------------------------------------------------------------
void* Mem_Memmove( void* pDest, const void* pSrc, size numBytes );
int32 Mem_Memcmp( const void* pBuf1, const void* pBuf2, size numBytes );
void* Mem_Memset( void* pDest, uint8 c, size numBytes );
void* Mem_Memzero( void* pDest, size numBytes );
void* Mem_Memcpy( void* pDest, const void* pSrc, size numBytes );
void* Mem_Malloc( size numBytes, uint32 alignment = 0 );
void* Mem_MallocZero( size numBytes, uint32 alignment = 0 );
void* Mem_Realloc( void* pOriginal, size numBytes, uint32 alignment = 0 );
void  Mem_Free( void* pOriginal );

// If possible determine the size of the memory allocated at the given address
size Mem_AllocSize( void* pOriginal );

// C style memory allocator that fall back to C runtime
void* Mem_MallocSystem( size numBytes );
void* Mem_ReallocSystem( void* pOriginal, size numBytes );
void  Mem_FreeSystem( void* pOriginal );

// Functions to get information about memory
TIER0_INTERFACE systemMemoryInfo_t Mem_SystemMemoryInfo();
TIER0_INTERFACE appMemoryInfo_t	   Mem_AppMemoryInfo();

//-----------------------------------------------------------------------------
// Inherit from CUseSystemMemAllocForNew if you want your objects to be placed in memory
// allocated by the system memory allocator routines, bypassing engine's global the one
//-----------------------------------------------------------------------------
class CUseSystemMemAllocForNew
{
public:
	void* operator new( size numBytes );
	void  operator delete( void* pPtr );
	void* operator new[]( size numBytes );
	void  operator delete[]( void* pPtr );
};

#include "tier0/memory.inl"
#if PLATFORM_WINDOWS
	#include "tier0/platforms/windows/win_memory.inl"
#else
	#error Unknown platform
#endif	// PLATFORM_WINDOWS

// Override the global memory allocator
#include "tier0/memoverride.h"
