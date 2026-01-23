#pragma once
#include "tier0/tier0_api.h"
#include "tier0/types.h"
#include "tier1/istreamdata.h"

//-----------------------------------------------------------------------------
// Compression
//-----------------------------------------------------------------------------
enum compressionType_t
{
	COMPRESSION_NONE,
	COMPRESSION_ZLIB
};

// Functions for [un]compress from memory
bool CompressMemory( compressionType_t compressionType, void* pCompressedBuffer, uint32& compressedSize, const void* pUncompressedBuffer, uint32 uncompressedSize );
bool UncompressMemory( compressionType_t compressionType, void* pUncompressedBuffer, uint32 uncompressedSize, const void* pCompressedBuffer, uint32 compressedSize );

// Functions for [un]compress from IStreamData
void CompressStreamData( compressionType_t compressionType, IStreamDataWriter* pStreamWriter, byte* pSrcBuffer, uint64 srcSize );
void UncompressStreamData( compressionType_t compressionType, IStreamDataReader* pStreamReader, byte* pDestBuffer, uint64 destSize );
