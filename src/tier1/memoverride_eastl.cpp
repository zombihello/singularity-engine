#include "pch_tier1.h"

// To use EASTL we need override the next global operators
// https://github.com/electronicarts/EASTL/blob/master/doc/FAQ.md#cont23-how-do-i-override-the-default-global-allocator

/*
==================
operator new[]
==================
*/
void* operator new[]( size numBytes, const char* pName, int flags, unsigned debugFlags, const char* pFile, int line )
{
	return Mem_Malloc( numBytes );
}

/*
==================
operator new[]
==================
*/
void* operator new[]( size numBytes, size alignment, size alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* pFile, int line )
{
	return Mem_Malloc( numBytes );
}
