/**
 * @file
 * @addtogroup filesystem filesystem
 */

#ifndef FILESYSTEM_PLATFORMS_H
#define FILESYSTEM_PLATFORMS_H

#include <string>
#include <vector>

#include "core/types.h"
#include "core/platform.h"
#include "filesystem/ifilesystem.h"

/**
 * @ingroup filesystem
 * @brief Platform implementation to create file reader
 * @note Need implement on each platform
 * 
 * @param pPath         Path to file
 * @param flags         Combinations flags of fileRead_t for open mode
 * @return Return pointer to file reader, if a file isn't open returns NULL
 */
TRefPtr<IStreamDataReader> Plat_CreateFileReader( const achar* pPath, uint32 flags = FILE_READ_NONE );

/**
 * @ingroup filesystem
 * @brief Platform implementation to create file writer
 * @note Need implement on each platform
 *
 * @param pPath         Path to file
 * @param flags         Combinations flags of fileWrite_t for opening mode
 * @return Return pointer to file writer, if a file isn't open returns NULL
 */
TRefPtr<IStreamDataWriter> Plat_CreateFileWriter( const achar* pPath, uint32 flags = FILE_WRITE_NONE );

/**
 * @ingroup filesystem
 * @brief Platform implementation to make directory
 * @note Need implement on each platform
 * 
 * @param pPath     Path to directory
 * @return Return TRUE if directory is successfully made, otherwise returns FALSE
 */
bool Plat_MakeDirectory( const achar* pPath );

/**
 * @ingroup filesystem
 * @brief Platform implementation to delete directory
 * @note Need implement on each platform
 * 
 * @param pPath         Path to directory
 * @param bEvenReadOnly Delete a directory even if it is read-only or a file in it
 * @return Return TRUE if directory is successfully deleted, otherwise returns FALSE
 */
bool Plat_DeleteDirectory( const achar* pPath, bool bEvenReadOnly = false );

/**
 * @ingroup filesystem
 * @brief Platform implementation to find files and directories by the path
 * @note Need implement on each platform
 *
 * @param pPath         Path to directory
 * @param bFiles        Whether to search files
 * @param bDirectories  Whether to search directories
 * @param foundFiles    Output array of paths with all found files and directories. NOTE: This function don't clear the array before use!
 * @return Return TRUE if any files were found, otherwise returns FALSE
 */
bool Plat_FindFiles( const achar* pPath, bool bFiles, bool bDirectories, std::vector<std::string>& foundFiles );

/**
 * @ingroup filesystem
 * @brief Platform implementation to delete file
 * @note Need implement on each platform
 * 
 * @param pPath             Path to file
 * @param bEvenReadOnly     Delete file even it's read only
 * @return Return TRUE if file is successfully deleted, otherwise returns FALSE
 */
bool Plat_DeleteFile( const achar* pPath, bool bEvenReadOnly = false );

/**
 * @ingroup filesystem
 * @brief Platform implementation to copy file
 * @note Need implement on each platform
 *
 * @param pSrcPath          Source path
 * @param pDestPath         Destination path
 * @param bReplaceExisting  Is need replace existing file
 * @param bEvenReadOnly     Is even read only
 * @return Return copy result (see copyMoveResult_t)
 */
copyMoveResult_t Plat_CopyFile( const achar* pSrcPath, const achar* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false );

/**
 * @ingroup filesystem
 * @brief Platform implementation to check is exists file or directory
 * @note Need implement on each platform
 *
 * @param pPath   Path to directory or file
 * @return Return TRUE if file or directory is exist, otherwise FALSE
 */
bool Plat_IsFileExists( const achar* pPath );

/**
 * @ingroup filesystem
 * @brief Platform implementation to check is this file a directory
 * @note Need implement on each platform
 *
 * @param pPath   Path to file
 * @return Return TRUE if a file is directory, otherwise returns FALSE
 */
bool Plat_IsFileDirectory( const achar* pPath );

/**
 * @ingroup filesystem
 * @brief Platform implementation to move file
 * @note Need implement on each platform
 *
 * @param pSrcPath               Source path
 * @param pDestPath              Destination path
 * @param bReplaceExisting       Is need replace existing file
 * @param bEvenReadOnly          Is even read only
 * @return Return move result (see copyMoveResult_t)
 */
copyMoveResult_t Plat_MoveFile( const achar* pSrcPath, const achar* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false );

// Include implementation of platform specific inline functions
#if PLATFORM_WINDOWS
	#include "filesystem/platforms/windows/win_filesystem.inl"
#endif // PLATFORM_WINDOWS

#endif // FILESYSTEM_PLATFORMS_H