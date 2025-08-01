#pragma once
#include <string>
#include <vector>

#include "stdlib/defines.h"
#include "stdlib/types.h"
#include "filesystem/ifilesystem.h"

//-----------------------------------------------------------------------------
// Platform specific functions
//-----------------------------------------------------------------------------
TRefPtr<IStreamDataReader> Plat_CreateFileReader( const achar* pPath, uint32 flags = FILE_READ_NONE );
TRefPtr<IStreamDataWriter> Plat_CreateFileWriter( const achar* pPath, uint32 flags = FILE_WRITE_NONE );
bool Plat_MakeDirectory( const achar* pPath );
bool Plat_DeleteDirectory( const achar* pPath, bool bEvenReadOnly = false );
bool Plat_FindFiles( const achar* pPath, bool bFiles, bool bDirectories, std::vector<std::string>& foundFiles );
bool Plat_DeleteFile( const achar* pPath, bool bEvenReadOnly = false );
copyMoveResult_t Plat_CopyFile( const achar* pSrcPath, const achar* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false );
bool Plat_IsFileExists( const achar* pPath );
bool Plat_IsFileDirectory( const achar* pPath );
copyMoveResult_t Plat_MoveFile( const achar* pSrcPath, const achar* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false );

// Include implementation of platform specific inline functions
#if PLATFORM_WINDOWS
	#include "filesystem/platforms/windows/win_filesystem.inl"
#endif // PLATFORM_WINDOWS