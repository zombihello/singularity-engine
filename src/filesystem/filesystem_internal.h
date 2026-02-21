#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include "tier0/defines.h"
#include "tier0/types.h"
#include "filesystem/ifilesystem.h"

//-----------------------------------------------------------------------------
// Platform specific internal defines
//-----------------------------------------------------------------------------
#define PLATFORM_USE_STD_FILE 0

//-----------------------------------------------------------------------------
// Platform specific functions
//-----------------------------------------------------------------------------
TRefPtr<IStreamDataReader> Plat_CreateFileReader( const char* pPath, uint32 flags = FILE_READ_NONE );
TRefPtr<IStreamDataWriter> Plat_CreateFileWriter( const char* pPath, uint32 flags = FILE_WRITE_NONE );
bool					   Plat_MakeDirectory( const char* pPath );
bool					   Plat_DeleteDirectory( const char* pPath, bool bEvenReadOnly = false );
bool					   Plat_FindFiles( const char* pPath, const char* pPattern, bool bFiles, bool bDirectories, eastl::vector<eastl::string>& foundFiles );
bool					   Plat_DeleteFile( const char* pPath, bool bEvenReadOnly = false );
copyMoveResult_t		   Plat_CopyFile( const char* pSrcPath, const char* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false );
bool					   Plat_IsFileExists( const char* pPath );
bool					   Plat_IsFileDirectory( const char* pPath );
copyMoveResult_t		   Plat_MoveFile( const char* pSrcPath, const char* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false );
bool					   Plat_SetCurrentDirectory( const char* pPath );
const char*				   Plat_GetUserDirectory();
bool					   Plat_GetCurrentDirectory( char* pDest, uint32 maxLen );

#if PLATFORM_WINDOWS
	#include "filesystem/platforms/windows/win_filesystem.h"
#endif	// PLATFORM_WINDOWS
