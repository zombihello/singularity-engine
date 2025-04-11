/**
 * @file
 * @addtogroup core core
 */

#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "core/coreapi.h"
#include "core/types.h"

/**
 * @ingroup core
 * @brief Compression type
 */
enum compressionType_t
{
	COMPRESSION_NONE,	/**< No compression */
	COMPRESSION_ZLIB,	/**< Compress with ZLIB */
};

/**
 * @ingroup core
 * @brief Thread-safe abstract compression routine. Compresses memory from uncompressed buffer and writes it to compressed
 * buffer. Updates CompressedSize with size of compressed data. Compression controlled by the passed in flags
 *
 * @param compressionType		Compression type
 * @param pCompressedBuffer		Buffer compressed data is going to be written to
 * @param compressedSize		Size of pCompressedBuffer, at exit will be size of compressed data
 * @param pUncompressedBuffer	Buffer containing uncompressed data
 * @param uncompressedSize		Size of uncompressed data in bytes
 * @return Return TRUE if compression succeeds, FALSE if it fails because pCompressedBuffer was too small or other reasons
 */
CORE_INTERFACE bool Sys_CompressMemory( compressionType_t compressionType, void* pCompressedBuffer, uint32& compressedSize, const void* pUncompressedBuffer, uint32 uncompressedSize );

/**
 * @ingroup core
 * @brief Thread-safe abstract decompression routine. Uncompresses memory from compressed buffer and writes it to uncompressed
 * buffer. UncompressedSize is expected to be the exact size of the data after decompression
 *
 * @param compressionType		Compression type
 * @param pUncompressedBuffer	Buffer containing uncompressed data
 * @param uncompressedSize		Size of uncompressed data in bytes
 * @param pCompressedBuffer		Buffer compressed data is going to be read from
 * @param compressedSize		Size of pCompressedBuffer data in bytes
 * @return Return TRUE if compression succeeds, FALSE if it fails because pCompressedBuffer was too small or other reasons
 */
CORE_INTERFACE bool Sys_UncompressMemory( compressionType_t compressionType, void* pUncompressedBuffer, uint32 uncompressedSize, const void* pCompressedBuffer, uint32 compressedSize );

#endif // !COMPRESSION_H