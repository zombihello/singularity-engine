#include "pch_tier1.h"

/*
==================
operator new[]

To use EASTL we need override the next global operators
https://github.com/electronicarts/EASTL/blob/master/doc/FAQ.md#cont23-how-do-i-override-the-default-global-allocator
==================
*/
void* operator new[]( size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line )
{
	return Mem_Malloc( size );
}

/*
==================
operator new[]
==================
*/
void* operator new[]( size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line )
{
	return Mem_Malloc( size );
}
