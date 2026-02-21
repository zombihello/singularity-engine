#pragma once

/*
==================
Plat_MakeDirectory
==================
*/
FORCEINLINE bool Plat_MakeDirectory( const char* pPath )
{
	return CreateDirectoryW( UTF8_TO_WCHAR( pPath ), NULL ) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
}

/*
==================
Plat_DeleteDirectory
==================
*/
FORCEINLINE bool Plat_DeleteDirectory( const char* pPath, bool bEvenReadOnly /*= false*/ )
{
	return RemoveDirectoryW( UTF8_TO_WCHAR( pPath ) );
}

/*
==================
Plat_IsFileExists
==================
*/
FORCEINLINE bool Plat_IsFileExists( const char* pPath )
{
	// Get file attributes at the path
	DWORD fileAttributes = GetFileAttributesW( UTF8_TO_WCHAR( pPath ) );

	// If it was not possible to get the file attributes, then the file does not exist
	if ( fileAttributes == INVALID_FILE_ATTRIBUTES )
	{
		return false;
	}

	// Otherwise all ok
	return true;
}

/*
==================
Plat_IsFileDirectory
==================
*/
FORCEINLINE bool Plat_IsFileDirectory( const char* pPath )
{
	DWORD fileAttributes = GetFileAttributesW( UTF8_TO_WCHAR( pPath ) );
	return fileAttributes != INVALID_FILE_ATTRIBUTES && fileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

/*
==================
Plat_MoveFile
==================
*/
FORCEINLINE copyMoveResult_t Plat_MoveFile( const char* pSrcPath, const char* pDestPath, bool bReplaceExisting /*= false*/, bool bEvenReadOnly /*= false*/ )
{
	DWORD moveFlags = ( bReplaceExisting ? MOVEFILE_REPLACE_EXISTING : 0x0 ) | MOVEFILE_WRITE_THROUGH;
	return MoveFileExW( UTF8_TO_WCHAR( pSrcPath ), UTF8_TO_WCHAR( pDestPath ), moveFlags ) != 0 ? COPYMOVE_RESULT_OK : COPYMOVE_RESULT_MISC_FAIL;
}
