#pragma once

//-----------------------------------------------------------------------------
// Override the global memory allocator
// 
// For override new and delete just add the file memoverride.cpp 
// into your project and all this will automatically be used
//-----------------------------------------------------------------------------
#undef malloc
#define malloc( NumBytes )			Mem_Malloc( NumBytes )

#undef realloc
#define realloc( Ptr, NumBytes )	Mem_Realloc( Ptr, NumBytes )

#undef free
#define free( Ptr )					Mem_Free( Ptr )