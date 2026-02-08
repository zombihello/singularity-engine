#include "pch_tier1.h"
#include "tier0/debug.h"
#include "tier1/filetools.h"

/*
==================
S_GetCurrentDirectory
==================
*/
void S_GetCurrentDirectory( eastl::string& destStr, bool bShrinkToFit /*= true*/ )
{
	// Pre-allocate memory for current directory
	destStr.resize( 1024 );

	// Try get the current directory
	while ( !S_GetCurrentDirectory( destStr.data(), (uint32)destStr.size() ) )
	{
		destStr.resize( destStr.size() * 2 );
	}

	// Remove all extra null terminator characters
	destStr = destStr.c_str();

	// Shrink to fit if it need
	if ( bShrinkToFit )
	{
		destStr.shrink_to_fit();
	}
}

/*
==================
S_MakeAbsolutePath
==================
*/
bool S_MakeAbsolutePath( const char* pSrcPath, char* pDestPath, uint32 maxLen, const char* pStartingDir /*= nullptr*/ )
{
	Assert( pDestPath );
	Assert( maxLen >= 1 );

	// Do nothing if pSrcPath isn't valid
	if ( !pSrcPath || !pSrcPath[0] )
	{
		*pDestPath = 0;
		return true;
	}

	// If pSrcPath isn't relative just copy it
	if ( S_IsAbsolutePath( pSrcPath ) )
	{
		// Calculate count of chars to copy
		uint32 length = S_Strlen( pSrcPath );

		// If length of source path too big do nothing
		if ( length > maxLen )
		{
			*pDestPath = 0;
			return false;
		}

		// Otherwise copy pSrcPath to pDestPath and fix path separators
		S_Strncpy( pDestPath, pSrcPath, length );
		S_RemoveDotPathSeparators( pDestPath );
		return true;
	}
	else
	{
		// Length of the final string
		uint32 finalLength = 0;

		// Make sure the starting directory is absolute
		if ( pStartingDir && S_IsAbsolutePath( pStartingDir ) )
		{
			// Calculate count of chars to copy
			uint32 length = S_Strlen( pStartingDir );

			// If length of pStartingDir too big do nothing
			if ( length > maxLen )
			{
				*pDestPath = 0;
				return false;
			}

			// Otherwise copy pStartingDir to pDestPath
			finalLength = length;
			S_Strncpy( pDestPath, pStartingDir, maxLen );
		}
		else
		{
			// Copy to pDestPath current directory. If S_GetCurrentDirectory return FALSE, it's mean what pDestPath too small
			if ( !S_GetCurrentDirectory( pDestPath, maxLen ) )
			{
				*pDestPath = 0;
				return false;
			}
			finalLength = S_Strlen( pDestPath );

			// If we have pStartingDir copy it
			if ( pStartingDir )
			{
				// If S_AppendPathSeparator return FALSE, it's mean what pDestPath too small
				if ( !S_AppendPathSeparator( pDestPath, maxLen ) )
				{
					*pDestPath = 0;
					return false;
				}
				finalLength += 1;

				// Make sure what size of pDestPath enough to append pStartingDir
				uint32 length = S_Strlen( pStartingDir );
				if ( length > maxLen - finalLength )
				{
					*pDestPath = 0;
					return false;
				}

				finalLength += length;
				S_Strncat( pDestPath, pStartingDir, maxLen, COPY_ALL_CHARACTERS );
			}
		}

		// If S_AppendPathSeparator return FALSE, it's mean what pDestPath too small
		if ( !S_AppendPathSeparator( pDestPath, maxLen ) )
		{
			*pDestPath = 0;
			return false;
		}
		finalLength += 1;

		// Make sure what size of pDestPath enough to append pSrcPath
		uint32 length = S_Strlen( pSrcPath );
		if ( length > maxLen - finalLength )
		{
			*pDestPath = 0;
			return false;
		}

		// Copy pSrcPath and fix path separators
		S_Strncat( pDestPath, pSrcPath, maxLen, COPY_ALL_CHARACTERS );
		S_RemoveDotPathSeparators( pDestPath );
		return true;
	}
}

/*
==================
S_MakeAbsolutePath
==================
*/
void S_MakeAbsolutePath( const eastl::string& srcPath, eastl::string& destPath, const eastl::string& startingDir /*= ""*/, bool bShrinkToFit /*= true*/ )
{
	// Pre-allocate memory for an absolute path
	destPath.resize( 1024 );

	// Try to make an absolute path
	while ( !S_MakeAbsolutePath( srcPath.c_str(), destPath.data(), (uint32)destPath.size(), startingDir.c_str() ) )
	{
		destPath.resize( destPath.size() * 2 );
	}

	// Remove all extra null terminator characters
	destPath = destPath.c_str();

	// Shrink to fit if it need
	if ( bShrinkToFit )
	{
		destPath.shrink_to_fit();
	}
}

/*
==================
S_MakeRelativePath
==================
*/
int32 S_MakeRelativePath( const char* pFullPath, const char* pDirPath, char* pRelativePath, uint32 maxLen )
{
	enum
	{
		RESULT_OK					  = 0,
		RESULT_ERR_DEST_TOO_SMALL	  = 1,
		RESULT_ERR_ON_SEPARATE_DRIVES = 2
	};

	Assert( pRelativePath );
	Assert( maxLen >= 1 );
	*pRelativePath = 0;

	// Strip out common parts of the path
	const char* pLastCommonPath = nullptr;
	const char* pLastCommonDir	= nullptr;
	{
		const char* pCurPath = pFullPath;
		const char* pCurDir	 = pDirPath;
		while ( *pCurPath && ( S_ToLower( *pCurPath ) == S_ToLower( *pCurDir ) || ( S_IsPathSeparator( *pCurPath ) && ( S_IsPathSeparator( *pCurDir ) || ( *pCurDir == 0 ) ) ) ) )
		{
			if ( S_IsPathSeparator( *pCurPath ) )
			{
				pLastCommonPath = pCurPath + 1;
				pLastCommonDir	= pCurDir + 1;
			}

			if ( *pCurDir == 0 )
			{
				--pLastCommonDir;
				break;
			}

			++pCurDir;
			++pCurPath;
		}
	}

	// Nothing in common
	if ( !pLastCommonPath )
	{
		return RESULT_ERR_ON_SEPARATE_DRIVES;
	}

	// For each path separator remaining in the directory, need a ../
	uint32 finalLength			 = 0;
	bool   bLastCharWasSeparator = true;
	for ( ; *pLastCommonDir; ++pLastCommonDir )
	{
		if ( S_IsPathSeparator( *pLastCommonDir ) )
		{
			// Make sure what size of pRelativePath enough to append ../
			if ( maxLen - finalLength < 3 )
			{
				*pRelativePath = 0;
				return RESULT_ERR_DEST_TOO_SMALL;
			}

			pRelativePath[finalLength++] = '.';
			pRelativePath[finalLength++] = '.';
			pRelativePath[finalLength++] = PATH_SEPARATOR;
			bLastCharWasSeparator		 = true;
		}
		else
		{
			bLastCharWasSeparator = false;
		}
	}

	// Deal with relative paths not specified with a trailing slash
	if ( !bLastCharWasSeparator )
	{
		// Make sure what size of pRelativePath enough to append ../
		if ( maxLen - finalLength < 3 )
		{
			*pRelativePath = 0;
			return RESULT_ERR_DEST_TOO_SMALL;
		}

		pRelativePath[finalLength++] = '.';
		pRelativePath[finalLength++] = '.';
		pRelativePath[finalLength++] = PATH_SEPARATOR;
	}

	// Copy the remaining part of the relative path over, fixing the path separators
	for ( ; *pLastCommonPath; ++pLastCommonPath )
	{
		// Make sure what size of pRelativePath enough to append a new char
		if ( maxLen - finalLength < 1 )
		{
			*pRelativePath = 0;
			return RESULT_ERR_DEST_TOO_SMALL;
		}

		if ( S_IsPathSeparator( *pLastCommonPath ) )
		{
			pRelativePath[finalLength++] = PATH_SEPARATOR;
		}
		else
		{
			pRelativePath[finalLength++] = *pLastCommonPath;
		}
	}

	// Make sure what size of pRelativePath enough to append a null terminator
	if ( maxLen - finalLength < 1 )
	{
		*pRelativePath = 0;
		return RESULT_ERR_DEST_TOO_SMALL;
	}

	pRelativePath[finalLength] = 0;
	return RESULT_OK;
}

/*
==================
S_MakeRelativePath
==================
*/
bool S_MakeRelativePath( const eastl::string& fullPath, const eastl::string& dirPath, eastl::string& relativePath, bool bShrinkToFit /*= true*/ )
{
	// Pre-allocate memory for an relative path
	relativePath.resize( 1024 );

	// Try to make an relative path
	int32 result = 1;
	while ( result == 1 )
	{
		result = S_MakeRelativePath( fullPath.c_str(), dirPath.c_str(), relativePath.data(), (uint32)relativePath.size() );
		if ( result == 1 )
		{
			relativePath.resize( relativePath.size() * 2 );
		}
	}

	// Remove all extra null terminator characters
	if ( result == 0 )
	{
		relativePath = relativePath.c_str();

		// Shrink to fit if it need
		if ( bShrinkToFit )
		{
			relativePath.shrink_to_fit();
		}
	}

	return result == 0;
}

/*
==================
S_AppendPathSeparator
==================
*/
bool S_AppendPathSeparator( char* pStr, uint32 strSize )
{
	// Do nothing if pStr isn't valid
	if ( !pStr || !pStr[0] )
	{
		return false;
	}

	// Append a path separator
	uint32 length = S_Strlen( pStr );
	Assert( length > 0 );
	if ( !S_IsPathSeparator( pStr[length - 1] ) )
	{
		if ( length + 1 >= strSize )
		{
			return false;
		}

		pStr[length]	 = PATH_SEPARATOR;
		pStr[length + 1] = 0;
	}

	return true;
}

/*
==================
S_AppendPathSeparator
==================
*/
void S_AppendPathSeparator( eastl::string& str )
{
	if ( str.empty() || !S_IsPathSeparator( str[str.size() - 1] ) )
	{
		str += PATH_SEPARATOR;
	}
}

/*
==================
S_FixPathSeparators
==================
*/
void S_FixPathSeparators( char* pPath )
{
	while ( *pPath )
	{
		if ( S_IsPathSeparator( *pPath ) )
		{
			*pPath = PATH_SEPARATOR;
		}
		++pPath;
	}
}

/*
==================
S_GetFileExtension
==================
*/
const char* S_GetFileExtension( const char* pPath, bool bIncludeDot /*= false*/ )
{
	// By default we at end of the path
	const char* pResult = pPath + S_Strlen( pPath ) - 1;

	// Back up until a . or the start of path
	while ( pResult != pPath && *( pResult - 1 ) != '.' )
	{
		--pResult;
	}

	// Check to see if the '.' is part of a path name
	if ( pResult == pPath || S_IsPathSeparator( *pResult ) )
	{
		// If true it's mean what we no have extension
		return nullptr;
	}

	// Step back if we need include a dot
	if ( bIncludeDot )
	{
		--pResult;
	}

	// Otherwise we return our extension
	return pResult;
}

/*
==================
S_GetFileBaseName
==================
*/
bool S_GetFileBaseName( const char* pPath, char* pFileName, uint32 maxLen )
{
	Assert( pFileName );
	Assert( maxLen >= 1 );

	// Do nothing if pPath isn't valid
	if ( !pPath || !pPath[0] )
	{
		*pFileName = 0;
		return true;
	}

	// Get string length
	uint32 length  = S_Strlen( pPath );
	int32  startId = 0;
	int32  endId   = 0;

	// Scan backward for '.'
	endId = length ? length - 1 : 0;
	while ( endId && pPath[endId] != '.' && !S_IsPathSeparator( pPath[endId] ) )
	{
		--endId;
	}

	// We no have '.' then copy to the end
	if ( pPath[endId] != '.' )
	{
		endId = length ? length - 1 : 0;
	}
	// Otherwise we found '.' then copy to left of '.'
	else
	{
		--endId;
	}

	// Scan backward for path separator
	startId = length ? length - 1 : 0;
	while ( startId >= 0 && !S_IsPathSeparator( pPath[startId] ) )
	{
		--startId;
	}

	// We no have a path separator then copy from the start
	if ( startId < 0 || !S_IsPathSeparator( pPath[startId] ) )
	{
		startId = 0;
	}
	// Otherwise we found a path separator then copy for right of the one
	else
	{
		++startId;
	}

	// Calculate length of a new string
	length = endId - startId + 1;

	// If length of a new string too big do nothing
	if ( length > maxLen )
	{
		*pFileName = 0;
		return false;
	}

	// Copy partial string
	uint32 maxCharsToCopy = Min<uint32>( length, maxLen );
	S_Strncpy( pFileName, &pPath[startId], maxCharsToCopy );
	return true;
}

/*
==================
S_GetFileBaseName
==================
*/
void S_GetFileBaseName( const eastl::string& path, eastl::string& fileName, bool bShrinkToFit /*= true*/ )
{
	// Pre-allocate memory for filename
	fileName.resize( 1024 );

	// Try copy the base file name
	while ( !S_GetFileBaseName( path.c_str(), fileName.data(), (uint32)fileName.size() ) )
	{
		fileName.resize( fileName.size() * 2 );
	}

	// Remove all extra null terminator characters
	fileName = fileName.c_str();

	// Shrink to fit if it need
	if ( bShrinkToFit )
	{
		fileName.shrink_to_fit();
	}
}

/*
==================
S_GetFileName
==================
*/
const char* S_GetFileName( const char* pPath )
{
	// Back up until the character after the first path separator we find,
	// or the beginning of the string
	const char* pResult = pPath + S_Strlen( pPath ) - 1;
	while ( pResult > pPath && !S_IsPathSeparator( *( pResult - 1 ) ) )
	{
		--pResult;
	}
	return pResult;
}

/*
==================
S_GetFilePath
==================
*/
bool S_GetFilePath( const char* pSrcPath, char* pDestPath, uint32 maxLen )
{
	Assert( pDestPath );
	Assert( maxLen >= 1 );

	// Do nothing if pSrcPath isn't valid
	if ( !pSrcPath || !pSrcPath[0] )
	{
		*pDestPath = 0;
		return true;
	}

	// Get string length
	uint32		length = S_Strlen( pSrcPath );
	const char* pSrc   = pSrcPath + ( length ? length - 1 : 0 );

	// Back up until a path separator or the start
	while ( pSrc != pSrcPath && !S_IsPathSeparator( *( pSrc - 1 ) ) )
	{
		--pSrc;
	}

	// Calculate length of a new string
	length = (uint32)( pSrc - pSrcPath );

	// If length of a new string too big do nothing
	if ( length > maxLen )
	{
		*pDestPath = 0;
		return false;
	}

	// Copy partial string
	uint32 maxCharsToCopy = Min<uint32>( length, maxLen );
	S_Strncpy( pDestPath, pSrcPath, maxCharsToCopy );
	return true;
}

/*
==================
S_GetFilePath
==================
*/
void S_GetFilePath( const eastl::string& srcPath, eastl::string& destPath, bool bShrinkToFit /*= true*/ )
{
	// Pre-allocate memory for a new string
	destPath.resize( 1024 );

	// Try copy the path
	while ( !S_GetFilePath( srcPath.c_str(), destPath.data(), (uint32)destPath.size() ) )
	{
		destPath.resize( destPath.size() * 2 );
	}

	// Remove all extra null terminator characters
	destPath = destPath.c_str();

	// Shrink to fit if it need
	if ( bShrinkToFit )
	{
		destPath.shrink_to_fit();
	}
}

/*
==================
S_RemoveDotPathSeparators
==================
*/
void S_RemoveDotPathSeparators( char* pPath, bool bRemoveDoubleSeparators /*= true*/ )
{
	// Read and write position in the string
	char* pRead		= pPath;
	char* pWrite	= pPath;
	bool  bBoundary = true;

	// Remove all dot path separators ("./", "../")
	while ( *pRead )
	{
		// Get rid of /../ or trailing /.. by backing pWrite up to previous separator
		if ( bBoundary && pRead[0] == '.' && pRead[1] == '.' && ( S_IsPathSeparator( pRead[2] ) || !pRead[2] ) )
		{
			// Eat the last separator (or repeated separators) we wrote out
			while ( pWrite != pPath && pWrite[-1] == PATH_SEPARATOR )
			{
				--pWrite;
			}

			while ( true )
			{
				if ( pWrite == pPath )
				{
					break;
				}
				--pWrite;
				if ( *pWrite == PATH_SEPARATOR )
				{
					break;
				}
			}

			// Skip the '..' but not the slash, next loop iteration will handle separator
			pRead += 2;
			bBoundary = ( pWrite == pPath );
		}
		// Handle "./" by simply skipping this sequence. bBoundary is unchanged
		else if ( bBoundary && pRead[0] == '.' && ( S_IsPathSeparator( pRead[1] ) || !pRead[1] ) )
		{
			if ( S_IsPathSeparator( pRead[1] ) )
			{
				pRead += 2;
			}
			else
			{
				// Special case: if trailing "." is preceded by separator, eg "path/.",
				// then the final separator should also be stripped. bBoundary may then
				// be in an incorrect state, but we are at the end of processing anyway
				// so we don't really care (the processing loop is about to terminate)
				if ( pWrite != pPath && pWrite[-1] == PATH_SEPARATOR )
				{
					--pWrite;
				}
				pRead += 1;
			}
		}
		// Handle "/"
		else if ( S_IsPathSeparator( pRead[0] ) )
		{
			*pWrite = PATH_SEPARATOR;
			pWrite += 1 - ( bBoundary & bRemoveDoubleSeparators & ( pWrite != pPath ) );
			pRead += 1;
			bBoundary = true;
		}
		// Otherwise iterate over the path
		else
		{
			if ( pWrite != pRead )
			{
				*pWrite = *pRead;
			}
			pWrite += 1;
			pRead += 1;
			bBoundary = false;
		}
	}

	// We are done!
	*pWrite = 0;
}

/*
==================
CFilename::CFilename
==================
*/
CFilename::CFilename()
{
}

/*
==================
CFilename::CFilename
==================
*/
CFilename::CFilename( const eastl::string& path )
	: path( path )
{
	S_FixPathSeparators( CFilename::path );
}
