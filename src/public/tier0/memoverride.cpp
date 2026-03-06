//
// Insert this file into all projects using the memory system
// It will cause that project to use the shared memory allocator
//
#include "tier0/memory.h"

/*
==================
operator new
==================
*/
void* operator new( size numBytes )
{
	return Mem_Malloc( numBytes );
}

/*
==================
operator delete
==================
*/
void operator delete( void* pPtr ) noexcept
{
	Mem_Free( pPtr );
}

/*
==================
operator new[]
==================
*/
void* operator new[]( size numBytes )
{
	return Mem_Malloc( numBytes );
}

/*
==================
operator delete[]
==================
*/
void operator delete[]( void* pPtr ) noexcept
{
	Mem_Free( pPtr );
}
