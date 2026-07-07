#include "pch_studiorender.h"
#include "studiorender/studio_framealloc.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
CFrameAlloc<STUDIO_FRAMEALLOC_BLOCK_SIZE, STUDIO_FRAMEALLOC_NUM_POOLS> g_studioFrameAlloc( "Studio FrameAlloc" );
