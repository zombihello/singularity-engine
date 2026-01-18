#pragma once
#include <string>

#include "stdlib/defines.h"
#include "stdlib/types.h"

//-----------------------------------------------------------------------------
// File tools
//-----------------------------------------------------------------------------
bool S_SetCurrentDirectory( const char* pDirName );
bool S_SetCurrentDirectory( const std::string& dirName );
bool S_GetCurrentDirectory( char* pDestStr, uint32 maxLen );
void S_GetCurrentDirectory( std::string& destStr, bool bShrinkToFit = true );

bool  S_MakeAbsolutePath( const char* pSrcPath, char* pDestPath, uint32 maxLen, const char* pStartingDir = NULL );
void  S_MakeAbsolutePath( const std::string& srcPath, std::string& destPath, const std::string& startingDir = "", bool bShrinkToFit = true );
int32 S_MakeRelativePath( const char* pFullPath, const char* pDirPath, char* pRelativePath, uint32 maxLen );
bool  S_MakeRelativePath( const std::string& fullPath, const std::string& dirPath, std::string& relativePath, bool bShrinkToFit = true );
bool  S_IsAbsolutePath( const char* pPath );
bool  S_IsAbsolutePath( const std::string& path );

bool S_IsPathSeparator( char c );
bool S_AppendPathSeparator( char* pStr, uint32 strSize );
void S_AppendPathSeparator( std::string& str );
void S_FixPathSeparators( char* pPath );
void S_FixPathSeparators( std::string& path );
void S_RemoveDotPathSeparators( char* pPath, bool bRemoveDoubleSeparators = true );
void S_RemoveDotPathSeparators( std::string& path, bool bRemoveDoubleSeparators = true );

const char* S_GetFileExtension( const char* pPath, bool bIncludeDot = false );
void		 S_GetFileExtension( const std::string& path, std::string& extension, bool bIncludeDot = false );
bool		 S_GetFileBaseName( const char* pPath, char* pFileName, uint32 maxLen );
void		 S_GetFileBaseName( const std::string& path, std::string& fileName, bool bShrinkToFit = true );
const char* S_GetFileName( const char* pPath );
void		 S_GetFileName( const std::string& path, std::string& fileName );
bool		 S_GetFilePath( const char* pSrcPath, char* pDestPath, uint32 maxLen );
void		 S_GetFilePath( const std::string& srcPath, std::string& destPath, bool bShrinkToFit = true );

//-----------------------------------------------------------------------------
// Utility class for quick inquiries against filenames
//-----------------------------------------------------------------------------
class CFilename
{
public:
	CFilename();
	CFilename( const std::string& path );

	bool			   IsInDirectory( const std::string& dirPath ) const;
	std::string		   GetExtension( bool bIncludeDot = false ) const;
	const std::string& GetFullPath() const;
	std::string		   GetBaseName() const;
	std::string		   GetFileName() const;
	std::string		   GetPath() const;

private:
	std::string path;
};

#include "stdlib/filetools.inl"
#if PLATFORM_WINDOWS
	#include "stdlib/platforms/windows/win_filetools.inl"
#else
	#error Unknown platform
#endif	// PLATFORM_WINDOWS