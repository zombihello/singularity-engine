#include "pch_filesystem.h"
#include "tier0/icommandline.h"
#include "filesystem/filesystem.h"
#include "filesystem/patharrayresult.h"
#include "filesystem/filesystem_internal.h"

// File system singleton
EXPOSE_SINGLE_INTERFACE( CFileSystem, IFileSystem, FILESYSTEM_INTERFACE_VERSION );

/*
==================
CFileSystem::CFileSystem
==================
*/
CFileSystem::CFileSystem()
{
	g_pFileSystem = this;
}

/*
==================
CFileSystem::~CFileSystem
==================
*/
CFileSystem::~CFileSystem()
{
	g_pFileSystem = NULL;
}

/*
==================
CFileSystem::Connect
==================
*/
bool CFileSystem::Connect( createInterfaceFn_t pFactory )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	return ConnectTier1( pFactory );
}

/*
==================
CFileSystem::Disconnect
==================
*/
void CFileSystem::Disconnect()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	DisconnectTier1();
}

/*
==================
CFileSystem::Init
==================
*/
bool CFileSystem::Init()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );

	// Get base directory from command line if it set
	eastl::string baseDir;
	if ( CommandLine()->HasParam( "basedir" ) )
	{
		baseDir = CommandLine()->GetFirstValue( "basedir" );
	}
	// Otherwise if not set default base directory
	else
	{
		baseDir = "../../";
	}

	// Make absolute path to the base directory (if it need)
	if ( !S_IsAbsolutePath( baseDir ) )
	{
		eastl::string exePath;
		eastl::string oldBaseDir = eastl::move( baseDir );
		S_GetFilePath( Sys_GetExecutablePath(), exePath, false );
		S_AppendPathSeparator( exePath );
		S_MakeAbsolutePath( oldBaseDir, baseDir, exePath, false );
	}

	// Set our base directory
	SetCurrentDirectory( baseDir.c_str() );

	// Add a search path to base directory
	AddSearchPath( baseDir.c_str(), "base_path" );
	AddSearchPath( baseDir.c_str(), "default_write_path" );
	return true;
}

/*
==================
CFileSystem::Shutdown
==================
*/
void CFileSystem::Shutdown()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );

	// Reset default current directory
	eastl::string exePath;
	S_GetFilePath( Sys_GetExecutablePath(), exePath, false );
	SetCurrentDirectory( exePath.c_str() );
	searchPaths.clear();
}

/*
==================
CFileSystem::CreateFileReader
==================
*/
CRefPtr<IStreamDataReader> CFileSystem::CreateFileReader( const char* pPath, uint32 flags /* = FILE_READ_NONE */ )
{
	// Parse a path ID in pPath
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	const char* pFilePath	 = NULL;
	const char* pPathID		 = NULL;
	uint32		lengthPathID = 0;
	ParsePathID( pPath, pFilePath, pPathID, lengthPathID );

	// Try open a file
	eastl::string finalPath;
	for ( CSearchPathIterator it( pFilePath, false, pPathID, lengthPathID ); it; ++it )
	{
		// Compute a full path
		ComputeFullPath( pFilePath, *it, finalPath );

		// Create file reader
		CRefPtr<IStreamDataReader> pFileReader = Plat_CreateFileReader( finalPath.c_str(), flags );
		if ( pFileReader )
		{
			return pFileReader;
		}
	}

	// Otherwise if we here it's mean what nothing found
	if ( flags & FILE_READ_NO_FAIL )
	{
		Sys_Error( "FileSystem: Failed to open file '%s', flags 0x%X", pPath, flags );
	}

	Warning( "FileSystem: Failed to open file '%s', flags 0x%X", pPath, flags );
	return NULL;
}

/*
==================
CFileSystem::CreateFileWriter
==================
*/
CRefPtr<IStreamDataWriter> CFileSystem::CreateFileWriter( const char* pPath, uint32 flags /* = FILE_WRITE_NONE */ )
{
	// Parse a path ID in pPath
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	const char* pFilePath	 = NULL;
	const char* pPathID		 = NULL;
	uint32		lengthPathID = 0;
	ParsePathID( pPath, pFilePath, pPathID, lengthPathID );

	// Try create a file
	eastl::string finalPath;
	eastl::string pathToFile;
	for ( CSearchPathIterator it( pFilePath, true, pPathID, lengthPathID ); it; ++it )
	{
		// Compute a full write path
		ComputeFullPath( pFilePath, *it, finalPath );

		// Create a directory tree for the file
		S_GetFilePath( finalPath, pathToFile, false );
		if ( !pathToFile.empty() && !MakeDirectoryInternal( pathToFile.c_str() ) )
		{
			// We failed, let's try another a search path
			continue;
		}

		// Create file writer
		CRefPtr<IStreamDataWriter> pFileWriter = Plat_CreateFileWriter( finalPath.c_str(), flags );
		if ( pFileWriter )
		{
			return pFileWriter;
		}
	}

	// Otherwise if we here it's mean what nothing found
	if ( flags & FILE_WRITE_NO_FAIL )
	{
		Sys_Error( "FileSystem: Failed to create file '%s', flags 0x%X", pPath, flags );
	}

	Warning( "FileSystem: Failed to create file '%s', flags 0x%X", pPath, flags );
	return NULL;
}

/*
==================
CFileSystem::FindFiles
==================
*/
CRefPtr<IPathArrayResult> CFileSystem::FindFiles( const char* pPath, const char* pPattern, bool bFiles, bool bDirectories, bool bLookAllPathIDs /* = true */ )
{
	// Parse a path ID in pPath
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	const char* pFilePath	 = NULL;
	const char* pPathID		 = NULL;
	uint32		lengthPathID = 0;
	ParsePathID( pPath, pFilePath, pPathID, lengthPathID );

	// Find files in all search paths
	eastl::string				 finalPath;
	eastl::vector<eastl::string> result;
	for ( CSearchPathIterator it( pFilePath, false, pPathID, lengthPathID ); it; ++it )
	{
		// Compute a full path and find files in the full path
		ComputeFullPath( pFilePath, *it, finalPath );
		bool bResult = Plat_FindFiles( finalPath.c_str(), pPattern, bFiles, bDirectories, result );

		// It's the end if we need look for only in the first search path where found any files
		if ( !bLookAllPathIDs && bResult )
		{
			break;
		}
	}

	// We are done!
	return new CPathArrayResult( result );
}

/*
==================
CFileSystem::LoadModule
==================
*/
dllHandle_t CFileSystem::LoadModule( const char* pDLLName )
{
	// Parse a path ID in pPath
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	const char* pModulePath	 = NULL;
	const char* pPathID		 = NULL;
	uint32		lengthPathID = 0;
	ParsePathID( pDLLName, pModulePath, pPathID, lengthPathID );

	// Try open a file
	eastl::string finalPath;
	for ( CSearchPathIterator it( pModulePath, false, pPathID, lengthPathID ); it; ++it )
	{
		// Compute a full path
		ComputeFullPath( pModulePath, *it, finalPath );

		// Load module
		dllHandle_t dllHandle = Sys_DLL_LoadModule( finalPath.c_str() );
		if ( dllHandle )
		{
			return dllHandle;
		}
	}

	// Otherwise if we here it's mean what nothing found
	Warning( "FileSystem: Failed to load module '%s'", pDLLName );
	return NULL;
}

/*
==================
CFileSystem::UnloadModule
==================
*/
void CFileSystem::UnloadModule( dllHandle_t dllHandle )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	if ( dllHandle )
	{
		Sys_DLL_UnloadModule( dllHandle );
	}
}

/*
==================
CFileSystem::DeleteFile
==================
*/
bool CFileSystem::DeleteFile( const char* pPath, bool bDeleteAllPathIDs /* = false */, bool bEvenReadOnly /* = false */ )
{
	// Parse a path ID in pPath
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	const char* pFilePath	 = NULL;
	const char* pPathID		 = NULL;
	uint32		lengthPathID = 0;
	ParsePathID( pPath, pFilePath, pPathID, lengthPathID );

	// Try delete a file
	bool		  bResult = false;
	eastl::string finalPath;
	for ( CSearchPathIterator it( pFilePath, true, pPathID, lengthPathID ); it; ++it )
	{
		// Compute a full path
		ComputeFullPath( pFilePath, *it, finalPath );

		// Delete a file
		bResult |= Plat_DeleteFile( finalPath.c_str(), bEvenReadOnly );

		// We are done if need to delete only one file
		if ( bResult && !bDeleteAllPathIDs )
		{
			break;
		}
	}

	// Print warning if we didn't to delete any file
	if ( !bResult )
	{
		Warning( "FileSystem: Failed to delete file '%s'", pPath );
	}

	// We are done!
	return bResult;
}

/*
==================
CFileSystem::MakeDirectory
==================
*/
bool CFileSystem::MakeDirectory( const char* pPath )
{
	// Parse a path ID in pPath
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	const char* pFilePath	 = NULL;
	const char* pPathID		 = NULL;
	uint32		lengthPathID = 0;
	ParsePathID( pPath, pFilePath, pPathID, lengthPathID );

	// Try to make a directory
	eastl::string finalPath;
	for ( CSearchPathIterator it( pFilePath, true, pPathID, lengthPathID ); it; ++it )
	{
		// Compute a full write path
		ComputeFullPath( pFilePath, *it, finalPath );

		// Make a directory
		if ( MakeDirectoryInternal( finalPath.c_str() ) )
		{
			return true;
		}
	}

	// Otherwise it's fail
	Warning( "FileSystem: Failed to create directory '%s'", pPath );
	return false;
}

/*
==================
CFileSystem::DeleteDirectory
==================
*/
bool CFileSystem::DeleteDirectory( const char* pPath, bool bDeleteAllPathIDs /* = false */, bool bEvenReadOnly /* = false */ )
{
	// Parse a path ID in pPath
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	const char* pFilePath	 = NULL;
	const char* pPathID		 = NULL;
	uint32		lengthPathID = 0;
	ParsePathID( pPath, pFilePath, pPathID, lengthPathID );

	// Try to delete a directory
	eastl::string finalPath;
	bool		  bResult = false;
	for ( CSearchPathIterator it( pFilePath, true, pPathID, lengthPathID ); it; ++it )
	{
		// Compute a full write path
		ComputeFullPath( pFilePath, *it, finalPath );

		// Delete a directory
		bResult |= DeleteDirectoryInternal( finalPath.c_str(), bEvenReadOnly );

		// We are done if need to delete only one directory
		if ( bResult && !bDeleteAllPathIDs )
		{
			break;
		}
	}

	// Print warning if we didn't to delete any directory
	if ( !bResult )
	{
		Warning( "FileSystem: Failed to delete directory '%s'", pPath );
	}

	// We are done!
	return bResult;
}

/*
==================
CFileSystem::CopyFile
==================
*/
copyMoveResult_t CFileSystem::CopyFile( const char* pSrcPath, const char* pDestPath, bool bReplaceExisting /* = false */, bool bEvenReadOnly /* = false */ )
{
	// Parse a path ID in pSrcPath
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	const char* pSrcFilePath	= NULL;
	const char* pSrcPathID		= NULL;
	uint32		lengthSrcPathID = 0;
	ParsePathID( pSrcPath, pSrcFilePath, pSrcPathID, lengthSrcPathID );

	// Parse a path ID in pDestPath
	const char* pDestFilePath	 = NULL;
	const char* pDestPathID		 = NULL;
	uint32		lengthDestPathID = 0;
	ParsePathID( pDestPath, pDestFilePath, pDestPathID, lengthDestPathID );

	// Try find a file and copy it
	eastl::string	 finalSrcPath;
	copyMoveResult_t result = COPYMOVE_RESULT_MISC_FAIL;
	for ( CSearchPathIterator srcIt( pSrcFilePath, false, pSrcPathID, lengthSrcPathID ); srcIt; ++srcIt )
	{
		// Compute a full source path
		ComputeFullPath( pSrcFilePath, *srcIt, finalSrcPath );

		// If file found let's try copy it
		if ( Plat_IsFileExists( finalSrcPath.c_str() ) && !Plat_IsFileDirectory( finalSrcPath.c_str() ) )
		{
			// Try copy the file
			eastl::string finalDestPath;
			eastl::string pathToFile;
			for ( CSearchPathIterator destIt( pDestFilePath, true, pDestPathID, lengthDestPathID ); destIt && result != COPYMOVE_RESULT_OK && result != COPYMOVE_RESULT_CANCELED; ++destIt )
			{
				// Compute a full destination path
				ComputeFullPath( pDestFilePath, *destIt, finalDestPath );

				// Make directory for a new file
				S_GetFilePath( finalDestPath, pathToFile, false );
				if ( !MakeDirectoryInternal( pathToFile.c_str() ) )
				{
					continue;
				}

				// Copy the file
				result = Plat_CopyFile( finalSrcPath.c_str(), finalDestPath.c_str(), bReplaceExisting, bEvenReadOnly );
			}

			// We're done, we won't try again
			break;
		}
	}

	// Print warning if we failed
	if ( result != COPYMOVE_RESULT_OK && result != COPYMOVE_RESULT_CANCELED )
	{
		Warning( "FileSystem: Failed to copy file '%s' to '%s'", pSrcPath, pDestPath );
	}

	// We are done!
	return result;
}

/*
==================
CFileSystem::CopyDirectory
==================
*/
copyMoveResult_t CFileSystem::CopyDirectory( const char* pSrcPath, const char* pDestPath, bool bCopyAllPathIDs /* = false */, bool bReplaceExisting /* = false */, bool bEvenReadOnly /* = false */ )
{
	// Parse a path ID in pSrcPath
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	const char* pSrcFilePath	= NULL;
	const char* pSrcPathID		= NULL;
	uint32		lengthSrcPathID = 0;
	ParsePathID( pSrcPath, pSrcFilePath, pSrcPathID, lengthSrcPathID );

	// Grab all directories to copy
	eastl::vector<eastl::string> srcPaths;
	{
		eastl::string finalSrcPath;
		for ( CSearchPathIterator srcIt( pSrcFilePath, false, pSrcPathID, lengthSrcPathID ); srcIt; ++srcIt )
		{
			// Compute a full source path
			ComputeFullPath( pSrcFilePath, *srcIt, finalSrcPath );

			// If file was found and it is directory then add the one to paths
			if ( Plat_IsFileExists( finalSrcPath.c_str() ) && Plat_IsFileDirectory( finalSrcPath.c_str() ) )
			{
				srcPaths.emplace_back( finalSrcPath );
			}

			// We stop if need copy only from the first path ID
			if ( !bCopyAllPathIDs && !srcPaths.empty() )
			{
				break;
			}
		}
	}

	// Parse a path ID in pDestPath
	const char* pDestFilePath	 = NULL;
	const char* pDestPathID		 = NULL;
	uint32		lengthDestPathID = 0;
	ParsePathID( pDestPath, pDestFilePath, pDestPathID, lengthDestPathID );

	// Copy directories
	eastl::string	 finalDestPath;
	copyMoveResult_t result = COPYMOVE_RESULT_MISC_FAIL;
	for ( CSearchPathIterator destIt( pDestFilePath, true, pDestPathID, lengthDestPathID ); destIt && result != COPYMOVE_RESULT_OK && result != COPYMOVE_RESULT_CANCELED; ++destIt )
	{
		// Compute a full destination path
		ComputeFullPath( pDestFilePath, *destIt, finalDestPath );

		// Copy directories
		for ( uint32 index = 0, count = (uint32)srcPaths.size(); index < count; ++index )
		{
			result = CopyDirectoryInternal( srcPaths[index].c_str(), finalDestPath.c_str(), bReplaceExisting, bEvenReadOnly );

			// If failed we try another search path
			if ( result != COPYMOVE_RESULT_OK && result != COPYMOVE_RESULT_CANCELED )
			{
				break;
			}
		}
	}

	// Print warning if we failed
	if ( result != COPYMOVE_RESULT_OK && result != COPYMOVE_RESULT_CANCELED )
	{
		Warning( "FileSystem: Failed to copy directory '%s' to '%s'", pSrcPath, pDestPath );
	}

	// We are done!
	return result;
}

/*
==================
CFileSystem::MoveFile
==================
*/
copyMoveResult_t CFileSystem::MoveFile( const char* pSrcPath, const char* pDestPath, bool bReplaceExisting /* = false */, bool bEvenReadOnly /* = false */ )
{
	// Parse a path ID in pSrcPath
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	const char* pSrcFilePath	= NULL;
	const char* pSrcPathID		= NULL;
	uint32		lengthSrcPathID = 0;
	ParsePathID( pSrcPath, pSrcFilePath, pSrcPathID, lengthSrcPathID );

	// Parse a path ID in pDestPath
	const char* pDestFilePath	 = NULL;
	const char* pDestPathID		 = NULL;
	uint32		lengthDestPathID = 0;
	ParsePathID( pDestPath, pDestFilePath, pDestPathID, lengthDestPathID );

	// Try find a file and move it
	eastl::string	 finalSrcPath;
	copyMoveResult_t result = COPYMOVE_RESULT_MISC_FAIL;
	for ( CSearchPathIterator srcIt( pSrcFilePath, false, pSrcPathID, lengthSrcPathID ); srcIt; ++srcIt )
	{
		// Compute a full source path
		ComputeFullPath( pSrcFilePath, *srcIt, finalSrcPath );

		// If file found let's try move it
		if ( Plat_IsFileExists( finalSrcPath.c_str() ) && !Plat_IsFileDirectory( finalSrcPath.c_str() ) )
		{
			// Try move the file
			eastl::string finalDestPath;
			eastl::string pathToFile;
			for ( CSearchPathIterator destIt( pDestFilePath, true, pDestPathID, lengthDestPathID ); destIt && result != COPYMOVE_RESULT_OK && result != COPYMOVE_RESULT_CANCELED; ++destIt )
			{
				// Compute a full destination path
				ComputeFullPath( pDestFilePath, *destIt, finalDestPath );

				// Make directory for the file
				S_GetFilePath( finalDestPath, pathToFile, false );
				if ( !MakeDirectoryInternal( pathToFile.c_str() ) )
				{
					continue;
				}

				// Move the file
				result = Plat_MoveFile( finalSrcPath.c_str(), finalDestPath.c_str(), bReplaceExisting, bEvenReadOnly );
			}

			// We're done, we won't try again
			break;
		}
	}

	// Print warning if we failed
	if ( result != COPYMOVE_RESULT_OK && result != COPYMOVE_RESULT_CANCELED )
	{
		Warning( "FileSystem: Failed to move file '%s' to '%s'", pSrcPath, pDestPath );
	}

	// We are done!
	return result;
}

/*
==================
CFileSystem::MoveDirectory
==================
*/
copyMoveResult_t CFileSystem::MoveDirectory( const char* pSrcPath, const char* pDestPath, bool bMoveAllPathIDs /* = false */, bool bReplaceExisting /* = false */, bool bEvenReadOnly /* = false */ )
{
	// Parse a path ID in pSrcPath
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	const char* pSrcFilePath	= NULL;
	const char* pSrcPathID		= NULL;
	uint32		lengthSrcPathID = 0;
	ParsePathID( pSrcPath, pSrcFilePath, pSrcPathID, lengthSrcPathID );

	// Grab all directories to move
	eastl::vector<eastl::string> srcPaths;
	{
		eastl::string finalSrcPath;
		for ( CSearchPathIterator srcIt( pSrcFilePath, true, pSrcPathID, lengthSrcPathID ); srcIt; ++srcIt )
		{
			// Compute a full source path
			ComputeFullPath( pSrcFilePath, *srcIt, finalSrcPath );

			// If file was found and it is directory then add the one to paths
			if ( Plat_IsFileExists( finalSrcPath.c_str() ) && Plat_IsFileDirectory( finalSrcPath.c_str() ) )
			{
				srcPaths.emplace_back( finalSrcPath );
			}

			// We stop if need move only from the first path ID
			if ( !bMoveAllPathIDs && !srcPaths.empty() )
			{
				break;
			}
		}
	}

	// Parse a path ID in pDestPath
	const char* pDestFilePath	 = NULL;
	const char* pDestPathID		 = NULL;
	uint32		lengthDestPathID = 0;
	ParsePathID( pDestPath, pDestFilePath, pDestPathID, lengthDestPathID );

	// Move directories
	eastl::string	 finalDestPath;
	copyMoveResult_t result = COPYMOVE_RESULT_MISC_FAIL;
	for ( CSearchPathIterator destIt( pDestFilePath, true, pDestPathID, lengthDestPathID ); destIt && result != COPYMOVE_RESULT_OK && result != COPYMOVE_RESULT_CANCELED; ++destIt )
	{
		// Compute a full destination path
		ComputeFullPath( pDestFilePath, *destIt, finalDestPath );

		// Move directories
		for ( uint32 index = 0, count = (uint32)srcPaths.size(); index < count; ++index )
		{
			result = MoveDirectoryInternal( srcPaths[index].c_str(), finalDestPath.c_str(), bReplaceExisting, bEvenReadOnly );

			// If failed we try another search path
			if ( result != COPYMOVE_RESULT_OK && result != COPYMOVE_RESULT_CANCELED )
			{
				break;
			}
		}
	}

	// Print warning if we failed
	if ( result != COPYMOVE_RESULT_OK && result != COPYMOVE_RESULT_CANCELED )
	{
		Warning( "FileSystem: Failed to move directory '%s' to '%s'", pSrcPath, pDestPath );
	}

	// We are done!
	return result;
}

/*
==================
CFileSystem::SetCurrentDirectory
==================
*/
bool CFileSystem::SetCurrentDirectory( const char* pPath )
{
	// Parse a path ID in pPath
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	const char* pFilePath	 = NULL;
	const char* pPathID		 = NULL;
	uint32		lengthPathID = 0;
	ParsePathID( pPath, pFilePath, pPathID, lengthPathID );

	// Select the first as current directory
	eastl::string finalPath;
	for ( CSearchPathIterator it( pFilePath, false, pPathID, lengthPathID ); it; ++it )
	{
		// Compute a full path
		ComputeFullPath( pFilePath, *it, finalPath );

		// If the directory is exist try to set as the current directory
		if ( Plat_IsFileDirectory( finalPath.c_str() ) && Plat_SetCurrentDirectory( finalPath.c_str() ) )
		{
			return true;
		}
	}

	// Otherwise it's fail
	Warning( "FileSystem: Failed to change current directory to '%s'", pPath );
	return false;
}

/*
==================
CFileSystem::IsFileExists
==================
*/
bool CFileSystem::IsFileExists( const char* pPath ) const
{
	// Parse a path ID in pPath
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	const char* pFilePath	 = NULL;
	const char* pPathID		 = NULL;
	uint32		lengthPathID = 0;
	ParsePathID( pPath, pFilePath, pPathID, lengthPathID );

	// Check on existing a file
	eastl::string finalPath;
	for ( CSearchPathIterator it( pFilePath, false, pPathID, lengthPathID ); it; ++it )
	{
		// Compute a full path
		ComputeFullPath( pFilePath, *it, finalPath );

		// If file is exist we are done!
		if ( Plat_IsFileExists( finalPath.c_str() ) )
		{
			return true;
		}
	}

	// If we're here, the file doesn't exist
	return false;
}

/*
==================
CFileSystem::IsFileDirectory
==================
*/
bool CFileSystem::IsFileDirectory( const char* pPath ) const
{
	// Parse a path ID in pPath
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	const char* pFilePath	 = NULL;
	const char* pPathID		 = NULL;
	uint32		lengthPathID = 0;
	ParsePathID( pPath, pFilePath, pPathID, lengthPathID );

	// Check a directory
	eastl::string finalPath;
	for ( CSearchPathIterator it( pFilePath, false, pPathID, lengthPathID ); it; ++it )
	{
		// Compute a full path
		ComputeFullPath( pFilePath, *it, finalPath );

		// If file is directory we are done!
		if ( Plat_IsFileDirectory( finalPath.c_str() ) )
		{
			return true;
		}
	}

	// If we're here, that means the test failed
	return false;
}

/*
==================
CFileSystem::GetUserDirectory
==================
*/
const char* CFileSystem::GetUserDirectory() const
{
	return Plat_GetUserDirectory();
}

/*
==================
CFileSystem::GetCurrentDirectory
==================
*/
bool CFileSystem::GetCurrentDirectory( char* pDest, uint32 maxLen ) const
{
	return Plat_GetCurrentDirectory( pDest, maxLen );
}

/*
==================
CFileSystem::MakeDirectoryInternal
==================
*/
bool CFileSystem::MakeDirectoryInternal( const char* pPath )
{
	// Do nothing if the path is empty
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	if ( !pPath || pPath[0] == '\0' )
	{
		return false;
	}

	// Preallocate memory for string
	eastl::string fullPath;
	fullPath.resize( S_Strlen( pPath ) );

	// Try make directory tree
	for ( uint32 index = 0, count = (uint32)fullPath.size(); index < count; ++index )
	{
		fullPath[index] = pPath[index];
		if ( ( S_IsPathSeparator( pPath[index] ) || index + 1 == count ) && !Plat_IsFileExists( fullPath.c_str() ) )
		{
			if ( !Plat_MakeDirectory( fullPath.c_str() ) )
			{
				return false;
			}
		}
	}

	return true;
}

/*
==================
CFileSystem::DeleteDirectoryInternal
==================
*/
bool CFileSystem::DeleteDirectoryInternal( const char* pPath, bool bEvenReadOnly /* = false */ )
{
	// Do nothing if the path is empty
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	if ( !pPath || pPath[0] == '\0' )
	{
		return false;
	}

	// Delete all files
	eastl::vector<eastl::string> paths;
	Plat_FindFiles( pPath, "*", true, false, paths );
	for ( uint32 index = 0, count = (uint32)paths.size(); index < count; ++index )
	{
		if ( !Plat_DeleteFile( paths[index].c_str(), bEvenReadOnly ) )
		{
			return false;
		}
	}

	// Delete all directories
	paths.clear();
	Plat_FindFiles( pPath, "*", false, true, paths );
	for ( uint32 index = 0, count = (uint32)paths.size(); index < count; ++index )
	{
		if ( !DeleteDirectoryInternal( paths[index].c_str(), bEvenReadOnly ) )
		{
			return false;
		}
	}

	return Plat_DeleteDirectory( pPath, bEvenReadOnly );
}

/*
==================
CFileSystem::CopyDirectoryInternal
==================
*/
copyMoveResult_t CFileSystem::CopyDirectoryInternal( const char* pSrcPath, const char* pDestPath, bool bReplaceExisting /* = false */, bool bEvenReadOnly /* = false */ )
{
	// Do nothing if src or dest path is empty
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	if ( !pSrcPath || !pDestPath || pSrcPath[0] == '\0' || pDestPath[0] == '\0' )
	{
		return COPYMOVE_RESULT_MISC_FAIL;
	}

	// Make directory tree for a new one
	if ( !MakeDirectoryInternal( pDestPath ) )
	{
		return COPYMOVE_RESULT_WRITE_FAIL;
	}

	// Copy files
	eastl::vector<eastl::string> paths;
	Plat_FindFiles( pSrcPath, "*", true, false, paths );
	for ( uint32 index = 0, count = (uint32)paths.size(); index < count; ++index )
	{
		// Get a full destination path
		eastl::string destPath = S_Sprintf( "%s/%s", pDestPath, S_GetFileName( paths[index].c_str() ) );
		S_FixPathSeparators( destPath );

		// Copy a file
		copyMoveResult_t result = Plat_CopyFile( paths[index].c_str(), destPath.c_str(), bReplaceExisting, bEvenReadOnly );
		if ( result != COPYMOVE_RESULT_OK && result != COPYMOVE_RESULT_CANCELED )
		{
			return result;
		}
	}

	// Copy sub directories
	paths.clear();
	Plat_FindFiles( pSrcPath, "*", false, true, paths );
	for ( uint32 index = 0, count = (uint32)paths.size(); index < count; ++index )
	{
		// Get a full destination path
		eastl::string destPath = S_Sprintf( "%s/%s", pDestPath, S_GetFileName( paths[index].c_str() ) );
		S_FixPathSeparators( destPath );

		// Copy a directory
		copyMoveResult_t result = CopyDirectoryInternal( paths[index].c_str(), destPath.c_str(), bReplaceExisting, bEvenReadOnly );
		if ( result != COPYMOVE_RESULT_OK && result != COPYMOVE_RESULT_CANCELED )
		{
			return result;
		}
	}

	return COPYMOVE_RESULT_OK;
}

/*
==================
CFileSystem::MoveDirectoryInternal
==================
*/
copyMoveResult_t CFileSystem::MoveDirectoryInternal( const char* pSrcPath, const char* pDestPath, bool bReplaceExisting /* = false */, bool bEvenReadOnly /* = false */ )
{
	// Do nothing if src or dest path is empty
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	if ( !pSrcPath || !pDestPath || pSrcPath[0] == '\0' || pDestPath[0] == '\0' )
	{
		return COPYMOVE_RESULT_MISC_FAIL;
	}

	// Make directory tree for a new one
	if ( !MakeDirectoryInternal( pDestPath ) )
	{
		return COPYMOVE_RESULT_WRITE_FAIL;
	}

	// Move files
	eastl::vector<eastl::string> paths;
	Plat_FindFiles( pSrcPath, "*", true, false, paths );
	for ( uint32 index = 0, count = (uint32)paths.size(); index < count; ++index )
	{
		// Get a full destination path
		eastl::string destPath = S_Sprintf( "%s/%s", pDestPath, S_GetFileName( paths[index].c_str() ) );
		S_FixPathSeparators( destPath );

		// Move a file
		copyMoveResult_t result = Plat_MoveFile( paths[index].c_str(), destPath.c_str(), bReplaceExisting, bEvenReadOnly );
		if ( result != COPYMOVE_RESULT_OK && result != COPYMOVE_RESULT_CANCELED )
		{
			return result;
		}
	}

	// Move sub directories
	paths.clear();
	Plat_FindFiles( pSrcPath, "*", false, true, paths );
	for ( uint32 index = 0, count = (uint32)paths.size(); index < count; ++index )
	{
		// Get a full destination path
		eastl::string destPath = S_Sprintf( "%s/%s", pDestPath, S_GetFileName( paths[index].c_str() ) );
		S_FixPathSeparators( destPath );

		// Move a directory
		copyMoveResult_t result = MoveDirectoryInternal( paths[index].c_str(), destPath.c_str(), bReplaceExisting, bEvenReadOnly );
		if ( result != COPYMOVE_RESULT_OK && result != COPYMOVE_RESULT_CANCELED )
		{
			return result;
		}
	}

	return COPYMOVE_RESULT_OK;
}

/*
==================
CFileSystem::AddSearchPath
==================
*/
void CFileSystem::AddSearchPath( const char* pSearchPath, const char* pPathID )
{
	const CSearchPath& searchPath = searchPaths.emplace_back( pPathID, pSearchPath );
	Msg( "FileSystem: Added search path '%s' with ID '%s'", searchPath.GetPath().c_str(), searchPath.GetPathID().c_str() );
}

/*
==================
CFileSystem::RemoveAllSearchPaths
==================
*/
void CFileSystem::RemoveAllSearchPaths()
{
	searchPaths.clear();
	Msg( "FileSystem: Removed all search paths" );
}

/*
==================
CFileSystem::RemoveSearchPath
==================
*/
void CFileSystem::RemoveSearchPath( const char* pPathID )
{
	for ( uint32 index = 0; index < searchPaths.size(); )
	{
		const CSearchPath& searchPath = searchPaths[index];
		if ( searchPath.GetPathID() == pPathID )
		{
			Msg( "FileSystem: Removed search path '%s' with ID '%s'", searchPath.GetPath().c_str(), pPathID );
			searchPaths.erase( searchPaths.begin() + index );
		}
		else
		{
			++index;
		}
	}
}

/*
==================
CFileSystem::GetSearchPath
==================
*/
CRefPtr<IPathArrayResult> CFileSystem::GetSearchPath( const char* pPathID ) const
{
	eastl::vector<eastl::string> result;
	for ( uint32 index = 0, count = (uint32)searchPaths.size(); index < count; ++index )
	{
		const CSearchPath& searchPath = searchPaths[index];
		if ( searchPath.GetPathID() == pPathID )
		{
			result.push_back( searchPath.GetPath() );
		}
	}

	return new CPathArrayResult( result );
}

/*
==================
CFileSystem::GetNumSearchPaths
==================
*/
uint32 CFileSystem::GetNumSearchPaths() const
{
	return (uint32)searchPaths.size();
}

/*
==================
CFileSystem::ParsePathID
==================
*/
void CFileSystem::ParsePathID( const char* pPath, const char*& pFilePath, const char*& pPathID, uint32& lengthPathID ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );

	// By default we think what in pPath no path ID
	pFilePath	 = pPath;
	pPathID		 = NULL;
	lengthPathID = 0;

	// Path must have at begin two forward slashes (//), otherwise no here path ID
	uint32 lengthPath = S_Strlen( pPath );
	if ( lengthPath > 2 )
	{
		// Make sure what we have two forward slashes (//)
		bool bForwardSlash = pPath[0] == '/' && pPath[1] == '/';
		if ( !bForwardSlash )
		{
			return;
		}

		// Find a first path separator after '//'
		const char* pFirstForwardSlash = pPath + 2;
		while ( pFirstForwardSlash && !S_IsPathSeparator( *pFirstForwardSlash ) )
		{
			++pFirstForwardSlash;
		}

		// If we not found any path separators after '//' then it's wrong
		if ( !pFirstForwardSlash )
		{
			Warning( "FileSystem: Failed to found forward slash for separating path ID from path to file (%s)", pPath );
			return;
		}

		// Set pointer to the begin of path ID and file path
		pFilePath	 = pFirstForwardSlash + 1;
		pPathID		 = pPath + 2;
		lengthPathID = (uint32)( pFirstForwardSlash - pPathID );
	}
}

/*
==================
CFileSystem::ComputeFullPath
==================
*/
void CFileSystem::ComputeFullPath( const char* pFilePath, const CSearchPath* pSearchPath, eastl::string& destPath ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	destPath.clear();
	Assert( pSearchPath );
	if ( !pSearchPath )
	{
		return;
	}

	if ( !pSearchPath->GetPath().empty() )
	{
		destPath += pSearchPath->GetPath();
		S_AppendPathSeparator( destPath );
	}

	destPath += pFilePath;
	S_FixPathSeparators( destPath );
}
