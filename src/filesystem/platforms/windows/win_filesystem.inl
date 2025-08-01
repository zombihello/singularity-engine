#pragma once

/*
==================
Plat_MakeDirectory
==================
*/
FORCEINLINE bool Plat_MakeDirectory( const achar* pPath )
{ 
	return CreateDirectoryA( pPath, NULL ) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
}

/*
==================
Plat_DeleteDirectory
==================
*/
FORCEINLINE bool Plat_DeleteDirectory( const achar* pPath, bool bEvenReadOnly /*= false*/ )
{
	return RemoveDirectoryA( pPath );
}

/*
==================
Plat_IsFileExists
==================
*/
FORCEINLINE bool Plat_IsFileExists( const achar* pPath )
{
	// Get file attributes at the path
	DWORD	fileAttributes = GetFileAttributesA( pPath );
	
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
FORCEINLINE bool Plat_IsFileDirectory( const achar* pPath )
{
	DWORD	fileAttributes = GetFileAttributesA( pPath );
	return fileAttributes != INVALID_FILE_ATTRIBUTES && fileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

/*
==================
Plat_MoveFile
==================
*/
FORCEINLINE copyMoveResult_t Plat_MoveFile( const achar* pSrcPath, const achar* pDestPath, bool bReplaceExisting /*= false*/, bool bEvenReadOnly /*= false*/ )
{
	DWORD	moveFlags = ( bReplaceExisting ? MOVEFILE_REPLACE_EXISTING : 0x0 ) | MOVEFILE_WRITE_THROUGH;
	return MoveFileExA( pSrcPath, pDestPath, moveFlags ) != 0 ? COPYMOVE_RESULT_OK : COPYMOVE_RESULT_MISC_FAIL;
}