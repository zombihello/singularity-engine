#pragma once

//-----------------------------------------------------------------------------
// Macros to allocate memory at stack
// This can't be a function, even FORCEINLINE'd because there's no guarantee that
// the memory returned in a function will stick around for the caller to use
//-----------------------------------------------------------------------------
#define Mem_Alloca( size )					 alloca( size )
#define Mem_AllocaAligned( size, alignment ) alloca( S_Align( size, alignment ) )

/*
 ==================
 Mem_Memmove
 ==================
 */
FORCEINLINE void* Mem_Memmove( void* pDest, const void* pSrc, size numBytes )
{
	return memmove( pDest, pSrc, numBytes );
}

/*
 ==================
 Mem_Memcmp
 ==================
 */
FORCEINLINE int32 Mem_Memcmp( const void* pBuf1, const void* pBuf2, size numBytes )
{
	return memcmp( pBuf1, pBuf2, numBytes );
}

/*
 ==================
 Mem_Memset
 ==================
 */
FORCEINLINE void* Mem_Memset( void* pDest, uint8 c, size numBytes )
{
	return memset( pDest, c, numBytes );
}

/*
 ==================
 Mem_Memzero
 ==================
 */
FORCEINLINE void* Mem_Memzero( void* pDest, size numBytes )
{
	return memset( pDest, 0, numBytes );
}

/*
 ==================
 Mem_Memcpy
 ==================
 */
FORCEINLINE void* Mem_Memcpy( void* pDest, const void* pSrc, size numBytes )
{
	return memcpy( pDest, pSrc, numBytes );
}
