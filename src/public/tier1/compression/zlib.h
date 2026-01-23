#pragma once
#include "tier1/defines.h"
#include "tier1/types.h"
#include "tier1/istreamdata.h"

//-----------------------------------------------------------------------------
// Helper for zlib compress/decompress a data
//-----------------------------------------------------------------------------
class CZLib
{
public:
	static void Compress( IStreamDataWriter* pStreamWriter, byte* pSrcBuffer, uint64 srcSize );
	static void Uncompress( IStreamDataReader* pStreamReader, byte* pDestBuffer, uint64 destSize );
};
