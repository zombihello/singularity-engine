/**
 * @file
 * @addtogroup stdlib stdlib
 */

#ifndef ZLIB_H
#define ZLIB_H

#include <vector>

#include "core/platform.h"
#include "core/types.h"
#include "stdlib/istreamdata.h"

/**
 * @ingroup stdlib
 * @brief Helper for zlib compress/decompress a data
 */
class CZLib
{
public:
	/**
	 * @brief Compress data
	 * 
	 * @param pStreamWriter		Stream writer
	 * @param pSrcBuffer		Pointer to source buffer
	 * @param srcSize			The source buffer size
	 */
	static void Compress( IStreamDataWriter* pStreamWriter, byte* pSrcBuffer, uint64 srcSize );

	/**
	 * @brief Uncompress data
	 *
	 * @param pStreamReader		Stream reader
	 * @param pDestBuffer		Pointer to destination buffer
	 * @param destSize			The destination buffer size
	 */
	static void Uncompress( IStreamDataReader* pStreamReader, byte* pDestBuffer, uint64 destSize );
};

#endif // !ZLIB_H