#include "pch_filesystem.h"
#include "filesystem/file_std.h"

/*
==================
Plat_CreateFileReader
==================
*/
TRefPtr<IStreamDataReader> Plat_CreateFileReader( const achar* pPath, uint32 flags /*= FILE_READ_NONE*/ )
{
	// Open a file
	FILE*	pFile = fopen( pPath, "rb" );
	if ( pFile )
	{
		// We are done!
		return new CStreamDataStdFileReader( pFile, pPath );
	}

	// Otherwise if we here it's mean what nothing found
	return nullptr;
}

/*
==================
Plat_CreateFileWriter
==================
*/
TRefPtr<IStreamDataWriter> Plat_CreateFileWriter( const achar* pPath, uint32 flags /*= FILE_WRITE_NONE*/ )
{
	// Translate flags into std flags
	achar	mode[] = "wb";
	if ( flags & FILE_WRITE_APPEND )
	{
		mode[0] = 'a';
	}

	// Create a file
	FILE*	pFile = fopen( pPath, mode );
	if ( pFile )
	{
		// We are done!
		return new CStreamDataStdFileWriter( pFile, pPath );
	}

	// Otherwise if we here it's mean what nothing found
	return nullptr;
}

/*
==================
Plat_FindFiles
==================
*/
bool Plat_FindFiles( const achar* pPath, bool bFiles, bool bDirectories, std::vector<std::string>& foundFiles )
{
    // Find files in the path
    bool                bResult = false;
    WIN32_FIND_DATAA	data;
	HANDLE              handle = FindFirstFileA( S_Sprintf( "%s/*", pPath ).c_str(), &data );
	if ( handle != INVALID_HANDLE_VALUE )
	{
		// Iterate over all files
		do
		{
			// Ignore '.', '..' and directories or files if it need
			if ( S_Strcmp( data.cFileName, "." ) && S_Strcmp( data.cFileName, ".." ) &&
				 ( ( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ? bDirectories : bFiles ) )
			{
                std::string&    filePath = foundFiles.emplace_back( pPath );
                S_AppendPathSeparator( filePath );
                filePath += data.cFileName;
                bResult = true;
			}
		}
		while ( FindNextFileA( handle, &data ) );
	}

	// Close handle if it need
	if ( handle != INVALID_HANDLE_VALUE )
	{
		FindClose( handle );
	}

    // We are done!
    return bResult;
}

/*
==================
Plat_DeleteFile
==================
*/
bool Plat_DeleteFile( const achar* pPath, bool bEvenReadOnly /*= false*/ )
{
	// Change file attributes if need to delete file even with read only
	if ( bEvenReadOnly )
	{
		SetFileAttributesA( pPath, FILE_ATTRIBUTE_NORMAL );
	}

	// Delete a file
	DWORD	error = 0;
	BOOL	bResult = DeleteFileA( pPath );
	if ( !bResult )
	{
		error = GetLastError();
	}

	// If didn't delete any file it's TRUE
	return bResult || error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
}

/*
==================
Plat_CopyFile
==================
*/
copyMoveResult_t Plat_CopyFile( const achar* pSrcPath, const achar* pDestPath, bool bReplaceExisting /*= false*/, bool bEvenReadOnly /*= false*/ )
{
	// Reset file attributes in the destination file if we must copy a file even read only
	if ( bEvenReadOnly )
	{
		SetFileAttributesA( pDestPath, 0 );
	}

	// Copy a file
	copyMoveResult_t		result;
	if ( CopyFileA( pSrcPath, pDestPath, !bReplaceExisting ) != 0 )
	{
		result = COPYMOVE_RESULT_OK;
	}
	else
	{
		result = COPYMOVE_RESULT_MISC_FAIL;
	}

	// Reset file attributes in the destination file
	if ( result == COPYMOVE_RESULT_OK )
	{
		SetFileAttributesA( pDestPath, 0 );
	}
	return result;
}