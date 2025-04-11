#include "pch_core.h"
#include <zlib.h>

#include "core/debug.h"
#include "core/compression.h"

/*
==================
CompressMemoryZLIB
==================
*/
static bool CompressMemoryZLIB( void* pCompressedBuffer, uint32& compressedSize, const void* pUncompressedBuffer, uint32 uncompressedSize )
{
	PROFILE_SCOPE()

	// Zlib wants to use unsigned long.
	unsigned long	zCompressedSize = compressedSize;
	unsigned long	zUncompressedSize = uncompressedSize;

	// Compress data
	bool bResult = compress( ( byte* )pCompressedBuffer, &zCompressedSize, ( const byte* )pUncompressedBuffer, zUncompressedSize ) == Z_OK ? TRUE : FALSE;

	// Propagate compressed size from intermediate variable back into out variable.
	compressedSize = zCompressedSize;
	return bResult;
}

/*
==================
UncompressMemoryZLIB
==================
*/
static bool UncompressMemoryZLIB( void* pUncompressedBuffer, uint32 uncompressedSize, const void* pCompressedBuffer, uint32 compressedSize )
{
	PROFILE_SCOPE()

	// Zlib wants to use unsigned long.
	unsigned long	zCompressedSize = compressedSize;
	unsigned long	zUncompressedSize = uncompressedSize;

	// Uncompress data.
	bool bResult = uncompress( ( byte* )pUncompressedBuffer, &zUncompressedSize, ( const byte* )pCompressedBuffer, zCompressedSize ) == Z_OK ? TRUE : FALSE;

	// Sanity check to make sure we uncompressed as much data as we expected to.
	Assert( uncompressedSize == zUncompressedSize );
	return bResult;
}

/*
==================
Sys_CompressMemory
==================
*/
bool Sys_CompressMemory( compressionType_t compressionType, void* pCompressedBuffer, uint32& compressedSize, const void* pUncompressedBuffer, uint32 uncompressedSize )
{
	PROFILE_SCOPE()

	// Make sure a valid compression scheme was provided
	Assert( compressionType != COMPRESSION_NONE );
	bool	bResult = false;

	switch ( compressionType )
	{
	case COMPRESSION_ZLIB:
		bResult = CompressMemoryZLIB( pCompressedBuffer, compressedSize, pUncompressedBuffer, uncompressedSize );
		break;

	default:
		Warning( "Sys_CompressMemory: Compression type (0x%X) is not supported", compressionType );
		bResult = false;
		break;
	}

	return bResult;
}

/*
==================
Sys_UncompressMemory
==================
*/
bool Sys_UncompressMemory( compressionType_t compressionType, void* pUncompressedBuffer, uint32 uncompressedSize, const void* pCompressedBuffer, uint32 compressedSize )
{
	PROFILE_SCOPE()

	// Make sure a valid compression scheme was provided
	Assert( compressionType != COMPRESSION_NONE );
	bool	bResult = false;

	switch ( compressionType )
	{
	case COMPRESSION_ZLIB:
		bResult = UncompressMemoryZLIB( pUncompressedBuffer, uncompressedSize, pCompressedBuffer, compressedSize );
		break;

	default:
		Warning( "Sys_UncompressMemory: Compression type (0x%X) is not supported", compressionType );
		bResult = false;
		break;
	}

	return bResult;
}