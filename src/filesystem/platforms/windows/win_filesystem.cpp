#include "pch_filesystem.h"
#include <shlobj.h>

#if PLATFORM_USE_STD_FILE
	#include "filesystem/platforms/generic/file_std.h"
#else
	#error Unknown file backend
#endif	// PLATFORM_USE_STD_FILE

/*
==================
Plat_CreateFileReader
==================
*/
TRefPtr<IStreamDataReader> Plat_CreateFileReader( const char* pPath, uint32 flags /*= FILE_READ_NONE*/ )
{
	// Open a file
#if PLATFORM_USE_STD_FILE
	FILE* pFile = _wfopen( UTF8_TO_WCHAR( pPath ), L"rb" );
	if ( pFile )
	{
		// We are done!

		return new CStreamDataStdFileReader( pFile, pPath );
	}
#else
	AssertUnimplemented();
#endif	// PLATFORM_USE_STD_FILE

	// Otherwise if we here it's mean what nothing found
	return NULL;
}

/*
==================
Plat_CreateFileWriter
==================
*/
TRefPtr<IStreamDataWriter> Plat_CreateFileWriter( const char* pPath, uint32 flags /*= FILE_WRITE_NONE*/ )
{
	// Translate flags into eastl flags
#if PLATFORM_USE_STD_FILE
	wchar_t mode[] = L"wb";
	if ( flags & FILE_WRITE_APPEND )
	{
		mode[0] = L'a';
	}

	// Create a file
	FILE* pFile = _wfopen( UTF8_TO_WCHAR( pPath ), mode );
	if ( pFile )
	{
		// We are done!
		return new CStreamDataStdFileWriter( pFile, pPath );
	}
#else
	AssertUnimplemented();
#endif	// PLATFORM_USE_STD_FILE

	// Otherwise if we here it's mean what nothing found
	return NULL;
}

/*
==================
Plat_FindFiles
==================
*/
bool Plat_FindFiles( const char* pPath, const char* pPattern, bool bFiles, bool bDirectories, eastl::vector<eastl::string>& foundFiles )
{
	// Find files in the path
	bool			 bResult = false;
	WIN32_FIND_DATAW data;
	HANDLE			 handle = FindFirstFileW( UTF8_TO_WCHAR( S_Sprintf( "%s%s%s", pPath, !S_HasPathSeparator( pPath ) ? "/" : "", pPattern ).c_str() ), &data );
	if ( handle != INVALID_HANDLE_VALUE )
	{
		// Iterate over all files
		do
		{
			// Ignore '.', '..' and directories or files if it need
			if ( S_Strcmp( data.cFileName, L"." ) && S_Strcmp( data.cFileName, L".." ) && ( ( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ? bDirectories : bFiles ) )
			{
				eastl::string& filePath = foundFiles.emplace_back( pPath );
				S_AppendPathSeparator( filePath );
				filePath += WCHAR_TO_UTF8( data.cFileName );
				bResult = true;
			}
		} while ( FindNextFileW( handle, &data ) );
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
bool Plat_DeleteFile( const char* pPath, bool bEvenReadOnly /*= false*/ )
{
	// Change file attributes if need to delete file even with read only
	eastl::wstring widePath = UTF8_TO_WCHAR( pPath );
	if ( bEvenReadOnly )
	{
		SetFileAttributesW( widePath.c_str(), FILE_ATTRIBUTE_NORMAL );
	}

	// Delete a file
	DWORD error	  = 0;
	BOOL  bResult = DeleteFileW( widePath.c_str() );
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
copyMoveResult_t Plat_CopyFile( const char* pSrcPath, const char* pDestPath, bool bReplaceExisting /*= false*/, bool bEvenReadOnly /*= false*/ )
{
	// Reset file attributes in the destination file if we must copy a file even read only
	eastl::wstring wideSrcPath	= UTF8_TO_WCHAR( pSrcPath );
	eastl::wstring wideDestPath = UTF8_TO_WCHAR( pDestPath );
	if ( bEvenReadOnly )
	{
		SetFileAttributesW( wideDestPath.c_str(), 0 );
	}

	// Copy a file
	copyMoveResult_t result;
	if ( CopyFileW( wideSrcPath.c_str(), wideDestPath.c_str(), !bReplaceExisting ) != 0 )
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
		SetFileAttributesW( wideDestPath.c_str(), 0 );
	}
	return result;
}

/*
==================
Plat_SetCurrentDirectory
==================
*/
bool Plat_SetCurrentDirectory( const char* pPath )
{
	return SetCurrentDirectoryW( UTF8_TO_WCHAR( pPath ) );
}

/*
==================
Plat_GetUserDirectory
==================
*/
const char* Plat_GetUserDirectory()
{
	static eastl::string s_userDir;
	if ( s_userDir.empty() )
	{
		// Get the My Documents directory
		wchar_t* pUserDir = NULL;
		HRESULT	 result	  = SHGetKnownFolderPath( FOLDERID_Documents, 0, NULL, &pUserDir );
		if ( SUCCEEDED( result ) )
		{
			// Make the base user dir path
			s_userDir = WCHAR_TO_UTF8( pUserDir );
			s_userDir += PATH_SEPARATOR_STRING;
			S_FixPathSeparators( s_userDir );
			CoTaskMemFree( pUserDir );
		}
	}

	return s_userDir.c_str();
}

/*
==================
Plat_GetCurrentDirectory
==================
*/
bool Plat_GetCurrentDirectory( char* pDest, uint32 maxLen )
{
	// Get length of the current directory path
	uint32 length = GetCurrentDirectoryW( 0, NULL );
	if ( length > maxLen )
	{
		return false;
	}

	// Get the current directory
	eastl::wstring currentDirectory;
	currentDirectory.resize( length );
	length = GetCurrentDirectoryW( (uint32)currentDirectory.size(), currentDirectory.data() );
	AssertMsg( length > 0, "Failed to get current directory (GetLastError 0x%X)", GetLastError() );

	// Convert WCHAR to UTF8 string and make sure that
	// a buffer has enough space for UTF8 string
	wcharToUtf8_t wcharToUtf8( currentDirectory.c_str() );
	length = S_Strlen( (char*)wcharToUtf8 );
	if ( length > maxLen )
	{
		return false;
	}

	// Copy the UTF8 string into the buffer
	Mem_Memcpy( pDest, (char*)wcharToUtf8, length );
	return true;
}
