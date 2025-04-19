#ifndef ZLIB_H
#define ZLIB_H

#include <vector>

#include "core/platform.h"
#include "core/types.h"
#include "stdlib/istreamdata.h"

//-----------------------------------------------------------------------------
// Helper for zlib compress/decompress a data
//-----------------------------------------------------------------------------
class CZLib
{
public:
	static void Compress( IStreamDataWriter* pStreamWriter, byte* pSrcBuffer, uint64 srcSize );
	static void Uncompress( IStreamDataReader* pStreamReader, byte* pDestBuffer, uint64 destSize );
};

#endif // !ZLIB_H