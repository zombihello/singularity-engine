#pragma once

/*
==================
Mem_Malloc
==================
*/
FORCEINLINE void* Mem_Malloc( size_t numBytes, uint32 alignment /*= DEFAULT_ALIGNMENT*/ )
{
	return MemAlloc()->Malloc(numBytes, alignment);
}

/*
==================
Mem_MallocZero
==================
*/
FORCEINLINE void* Mem_MallocZero( size_t numBytes, uint32 alignment /*= DEFAULT_ALIGNMENT*/ )
{
	void*	pData = Mem_Malloc( numBytes, alignment );
	Mem_Memzero( pData, numBytes );
	return pData;
}

/*
==================
Mem_Realloc
==================
*/
FORCEINLINE void* Mem_Realloc( void* pOriginal, size_t numBytes, uint32 alignment /*= DEFAULT_ALIGNMENT*/ )
{
	return MemAlloc()->Realloc( pOriginal, numBytes, alignment );
}

/*
==================
Mem_Free
==================
*/
FORCEINLINE void Mem_Free( void* pOriginal )
{
	return MemAlloc()->Free( pOriginal );
}

/*
==================
Mem_AllocSize
==================
*/
FORCEINLINE size_t Mem_AllocSize( void* pOriginal )
{
	size_t	numBytes = 0;
	return MemAlloc()->GetAllocationSize( pOriginal, numBytes ) ? numBytes : 0;
}

/*
==================
Mem_MallocSystem
==================
*/
FORCEINLINE void* Mem_MallocSystem( size_t numBytes )
{
	return malloc( numBytes );
}

/*
==================
Mem_ReallocSystem
==================
*/
FORCEINLINE void* Mem_ReallocSystem( void* pOriginal, size_t numBytes )
{
	return realloc( pOriginal, numBytes );
}

/*
==================
Mem_FreeSystem
==================
*/
FORCEINLINE void Mem_FreeSystem( void* pOriginal )
{
	free( pOriginal );
}


/*
==================
CUseSystemMemAllocForNew::operator new
==================
*/
FORCEINLINE void* CUseSystemMemAllocForNew::operator new( size_t numBytes )
{
	return Mem_MallocSystem( numBytes );
}

/*
==================
CUseSystemMemAllocForNew::operator delete
==================
*/
FORCEINLINE void CUseSystemMemAllocForNew::operator delete( void* pPtr )
{
	Mem_FreeSystem( pPtr );
}

/*
==================
CUseSystemMemAllocForNew::operator new[]
==================
*/
FORCEINLINE void* CUseSystemMemAllocForNew::operator new[]( size_t numBytes )
{
	return Mem_MallocSystem( numBytes );
}

/*
==================
CUseSystemMemAllocForNew::operator delete[]
==================
*/
FORCEINLINE void CUseSystemMemAllocForNew::operator delete[]( void* pPtr )
{
	Mem_FreeSystem( pPtr );
}
