#pragma once

//-----------------------------------------------------------------------------
// Macroses to allocate memory at stack
// This can't be a function, even FORCEINLINE'd because there's no guarantee that the memory returned in a function will stick around for the caller to use
//-----------------------------------------------------------------------------
#define Mem_Alloca( size )									( size > 0 ? ( void* )( ( ( ptrint )alloca( size + 15 ) + 15 ) & ~15 ) : 0 )
#define Mem_AllocaAligned( size, alignment )				( size > 0 ? ( alignment <= 16 ? Mem_Alloca( size ) : ( void* )( ( ( ptrint )alloca( size + alignment-1 ) + alignment-1 ) & ~( alignment-1 ) ) ) : 0 )

/*
 ==================
 Mem_Memmove
 ==================
 */
FORCEINLINE void* Mem_Memmove( void* pDest, const void* pSrc, size_t numBytes )
{
	return memmove( pDest, pSrc, numBytes );
}

/*
 ==================
 Mem_Memcmp
 ==================
 */
FORCEINLINE int32 Mem_Memcmp( const void* pBuf1, const void* pBuf2, size_t numBytes )
{
	return memcmp( pBuf1, pBuf2, numBytes );
}

/*
 ==================
 Mem_Memset
 ==================
 */
FORCEINLINE void* Mem_Memset( void* pDest, uint8 c, size_t numBytes )
{
	return memset( pDest, c, numBytes );
}

/*
 ==================
 Mem_Memzero
 ==================
 */
FORCEINLINE void* Mem_Memzero( void* pDest, size_t numBytes )
{
	return memset( pDest, 0, numBytes );
}

/*
 ==================
 Mem_Memcpy
 ==================
 */
FORCEINLINE void* Mem_Memcpy( void* pDest, const void* pSrc, size_t numBytes )
{
	return memcpy( pDest, pSrc, numBytes );
}