#include "pch_stdlib.h"

#include "core/compression.h"
#include "stdlib/compression/zlib.h"

/**
 * @ingroup stdlib
 * @brief Size of chunk for uncompress compressed data
 */
#define UNCOMPRESS_CHUNK_SIZE			131072

/**
 * @ingroup stdlib
 * @brief Size of chunk for compress data
 */
#define COMPRESS_CHUNK_SIZE				UNCOMPRESS_CHUNK_SIZE

/**
 * @ingroup stdlib
 * @brief Header of compressed chunks
 */
struct compressedChunkSummary_t
{
	uint64		compressedSize;			/**< Full compressed size of data */
	uint64		uncompressedSize;		/**< Full uncompresses size of data */
};

/**
 * @ingroup stdlib
 * @brief Helper structure for compression support, containing information on compressed
 * and uncompressed size of a chunk of data
 */
struct compressedChunkInfo_t
{
	uint32		compressedSize;			/**< Compressed size of data */
	uint32		uncompressedSize;		/**< Uncompresses size of data */
};


/*
==================
CZLib::Compress
==================
*/
void CZLib::Compress( IStreamDataWriter* pStreamWriter, byte* pSrcBuffer, uint64 srcSize )
{
	PROFILE_SCOPE();

	// Do nothing if source size is zero
	if ( srcSize <= 0 )
	{
		return;
	}

	// Figure out how many chunks there are going to be based on uncompressed size and compression chunk size
	Assert( pStreamWriter && pSrcBuffer );
	uint64	totalChunkCount = ( srcSize + COMPRESS_CHUNK_SIZE - 1 ) / COMPRESS_CHUNK_SIZE;

	// Keep track of current position so we can later seek back and overwrite stub compression chunk infos
	uint64	startPosition = pStreamWriter->Tell();

	// Leave some place for header of compressed chunks, we overwrite it later
	compressedChunkSummary_t		summary;
	pStreamWriter->Write( &summary, sizeof( compressedChunkSummary_t ) );

	// Allocate compression chunk infos and serialize them so we can later overwrite the data
	compressedChunkInfo_t*	pCompressionChunks = new compressedChunkInfo_t[totalChunkCount];
	for ( uint64 chunkIndex = 0; chunkIndex < totalChunkCount; ++chunkIndex )
	{
		pStreamWriter->Write( &pCompressionChunks[chunkIndex], sizeof( compressedChunkInfo_t ) );
	}

	summary.uncompressedSize	= srcSize;		// The uncompressd size is equal to the passed in length
	summary.compressedSize		= 0;			// Zero initialize compressed size so we can update it during chunk compression

	// Set up source pointer amount of data to copy (in bytes)
	byte*	pSrc = pSrcBuffer;

	int64	bytesRemaining			= srcSize;
	uint64	currentChunkIndex		= 0;								// Start at index 1 as first chunk info is summary.
	uint32	compressedBufferSize	= COMPRESS_CHUNK_SIZE * 2;			// 2 times the uncompressed size should be more than enough; the compressed data shouldn't be that much larger
	void*	pCompressedBuffer		= Mem_Malloc( compressedBufferSize );
	while ( bytesRemaining > 0 )
	{
		uint32		bytesToCompress = Min<uint32>( ( uint32 )bytesRemaining, COMPRESS_CHUNK_SIZE );
		uint32		compressedSize = compressedBufferSize;
		bool		bResult = Sys_CompressMemory( COMPRESSION_ZLIB, pCompressedBuffer, compressedSize, pSrc, bytesToCompress );
		Assert( bResult );

		// Move to next chunk
		pSrc += bytesToCompress;
		pStreamWriter->Write( pCompressedBuffer, compressedSize );

		// Keep track of total compressed size, stored in first chunk
		summary.compressedSize += compressedSize;

		// Update current chunk
		Assert( currentChunkIndex < totalChunkCount );
		pCompressionChunks[currentChunkIndex].compressedSize	= compressedSize;
		pCompressionChunks[currentChunkIndex].uncompressedSize	= bytesToCompress;
		++currentChunkIndex;

		bytesRemaining -= COMPRESS_CHUNK_SIZE;
	}

	// Free allocated memory
	Mem_Free( pCompressedBuffer );

	// Overrwrite chunk infos by seeking to the beginning, serializing the data and then
	// seeking back to the end
	uint64	endPosition = pStreamWriter->Tell();

	// Seek to the beginning
	pStreamWriter->Seek( startPosition );

	// Serialize chunk summary
	pStreamWriter->Write( &summary, sizeof( compressedChunkSummary_t ) );

	// Serialize chunk infos
	for ( uint64 chunkIndex = 0; chunkIndex < totalChunkCount; chunkIndex++ )
	{
		pStreamWriter->Write( &pCompressionChunks[chunkIndex], sizeof( compressedChunkInfo_t ) );
	}

	// Seek back to end
	pStreamWriter->Seek( endPosition );

	// Free intermediate data
	delete[] pCompressionChunks;
}

/*
==================
CZLib::Uncompress
==================
*/
void CZLib::Uncompress( IStreamDataReader* pStreamReader, byte* pDestBuffer, uint64 destSize )
{
	PROFILE_SCOPE();

	// Do nothing if destination size is zero
	if ( destSize <= 0 )
	{
		return;
	}

	// Read the header compressed chunks	
	Assert( pStreamReader && pDestBuffer );
	compressedChunkSummary_t		summary;
	pStreamReader->Read( &summary, sizeof( compressedChunkSummary_t ) );

	// Handle change in compression chunk size in backward compatible way
	uint32	loadingCompressionChunkSize = UNCOMPRESS_CHUNK_SIZE;

	// Figure out how many chunks there are going to be based on uncompressed size and compression chunk size
	uint64	totalChunkCount = ( summary.uncompressedSize + loadingCompressionChunkSize - 1 ) / loadingCompressionChunkSize;

	// Allocate compression chunk infos and serialize them, keeping track of max size of compression chunks used
	compressedChunkInfo_t*	pCompressionChunks = new compressedChunkInfo_t[totalChunkCount];
	uint32					maxCompressedSize = 0;
	for ( uint64 chunkIndex = 0; chunkIndex < totalChunkCount; chunkIndex++ )
	{
		pStreamReader->Read( &pCompressionChunks[chunkIndex], sizeof( compressedChunkInfo_t ) );
		maxCompressedSize = Max( pCompressionChunks[chunkIndex].compressedSize, maxCompressedSize );
	}

	// Set up destination pointer and allocate memory for compressed chunks (one at a time)
	byte*	pDest = pDestBuffer;
	void*	pCompressedBuffer = Mem_Malloc( maxCompressedSize );

	// Iterate over all chunks, serialize them into memory and decompress them directly into the destination pointer
	for ( uint64 chunkIndex = 0; chunkIndex < totalChunkCount; chunkIndex++ )
	{
		const compressedChunkInfo_t&	chunk = pCompressionChunks[chunkIndex];

		// Read compressed data and decompress into pDest pointer directly
		pStreamReader->Read( pCompressedBuffer, chunk.compressedSize );
		bool	bResult = Sys_UncompressMemory( COMPRESSION_ZLIB, pDest, chunk.uncompressedSize, pCompressedBuffer, chunk.compressedSize );
		Assert( bResult );

		// And advance it by read amount
		pDest += chunk.uncompressedSize;
	}

	// Free up allocated memory
	Mem_Free( pCompressedBuffer );
	delete[] pCompressionChunks;
}