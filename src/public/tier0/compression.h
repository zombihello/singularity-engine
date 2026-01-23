#pragma once
#include "tier0/tier0_api.h"
#include "tier1/types.h"

//-----------------------------------------------------------------------------
// Compression
//-----------------------------------------------------------------------------
enum compressionType_t
{
	COMPRESSION_NONE,
	COMPRESSION_ZLIB
};

// Thread-safe abstract compression routine. Compresses memory from uncompressed buffer and writes it to compressed
// buffer. Updates CompressedSize with size of compressed data. Compression controlled by the passed in flags
TIER0_INTERFACE bool Sys_CompressMemory( compressionType_t compressionType, void* pCompressedBuffer, uint32& compressedSize, const void* pUncompressedBuffer, uint32 uncompressedSize );

// Thread-safe abstract decompression routine. Uncompresses memory from compressed buffer and writes it to uncompressed
// buffer. UncompressedSize is expected to be the exact size of the data after decompression
TIER0_INTERFACE bool Sys_UncompressMemory( compressionType_t compressionType, void* pUncompressedBuffer, uint32 uncompressedSize, const void* pCompressedBuffer, uint32 compressedSize );
