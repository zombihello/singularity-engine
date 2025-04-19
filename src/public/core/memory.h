#ifndef MEMORY_H
#define MEMORY_H

#include <cstdlib>

#include "core/coreapi.h"
#include "core/platform.h"
#include "core/imemalloc.h"

//-----------------------------------------------------------------------------
// Memory functions
//-----------------------------------------------------------------------------
void* Mem_Memmove( void* pDest, const void* pSrc, size_t numBytes );
int32 Mem_Memcmp( const void* pBuf1, const void* pBuf2, size_t numBytes );
void* Mem_Memset( void* pDest, uint8 c, size_t numBytes );
void* Mem_Memzero( void* pDest, size_t numBytes );
void* Mem_Memcpy( void* pDest, const void* pSrc, size_t numBytes );
void* Mem_Malloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT );
void* Mem_MallocZero( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT );
void* Mem_Realloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT );
void Mem_Free( void* pOriginal );

// If possible determine the size of the memory allocated at the given address
size_t Mem_AllocSize( void* pOriginal );

// C style memory allocator that fall back to C runtime
void* Mem_MallocSystem( size_t numBytes );
void* Mem_ReallocSystem( void* pOriginal, size_t numBytes );
void Mem_FreeSystem( void* pOriginal );


//-----------------------------------------------------------------------------
// Inherit from CUseSystemMemAllocForNew if you want your objects to be placed in memory
// allocated by the system memory allocator routines, bypassing engine's global the one
//-----------------------------------------------------------------------------
class CUseSystemMemAllocForNew
{
public:
	void* operator new( size_t numBytes );
	void operator delete( void* pPtr );
	void* operator new[]( size_t numBytes );
	void operator delete[]( void* pPtr );
};

#include "core/memory.inl"
#if PLATFORM_WINDOWS
	#include "core/platforms/windows/win_memory.inl"
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS

// Override the global memory allocator
#include "core/memoverride.h"

#endif // !MEMORY_H