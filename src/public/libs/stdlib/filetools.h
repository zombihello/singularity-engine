#pragma once
#include <string>

#include "stdlib/defines.h"
#include "stdlib/types.h"

//-----------------------------------------------------------------------------
// File tools
//-----------------------------------------------------------------------------
bool S_SetCurrentDirectory( const achar* pDirName );
bool S_SetCurrentDirectory( const std::string& dirName );
bool S_GetCurrentDirectory( achar* pDestStr, uint32 maxLen );
void S_GetCurrentDirectory( std::string& destStr, bool bShrinkToFit = true );

bool S_MakeAbsolutePath( const achar* pSrcPath, achar* pDestPath, uint32 maxLen, const achar* pStartingDir = NULL );
void S_MakeAbsolutePath( const std::string& srcPath, std::string& destPath, const std::string& startingDir = "", bool bShrinkToFit = true );
int32 S_MakeRelativePath( const achar* pFullPath, const achar* pDirPath, achar* pRelativePath, uint32 maxLen );
bool S_MakeRelativePath( const std::string& fullPath, const std::string& dirPath, std::string& relativePath, bool bShrinkToFit = true );
bool S_IsAbsolutePath( const achar* pPath );
bool S_IsAbsolutePath( const std::string& path );

bool S_IsPathSeparator( achar c );
bool S_AppendPathSeparator( achar* pStr, uint32 strSize );
void S_AppendPathSeparator( std::string& str );
void S_FixPathSeparators( achar* pPath );
void S_FixPathSeparators( std::string& path );
void S_RemoveDotPathSeparators( achar* pPath, bool bRemoveDoubleSeparators = true );
void S_RemoveDotPathSeparators( std::string& path, bool bRemoveDoubleSeparators = true );

const achar* S_GetFileExtension( const achar* pPath, bool bIncludeDot = false );
void S_GetFileExtension( const std::string& path, std::string& extension, bool bIncludeDot = false );
bool S_GetFileBaseName( const achar* pPath, achar* pFileName, uint32 maxLen );
void S_GetFileBaseName( const std::string& path, std::string& fileName, bool bShrinkToFit = true );
const achar* S_GetFileName( const achar* pPath );
void S_GetFileName( const std::string& path, std::string& fileName );
bool S_GetFilePath( const achar* pSrcPath, achar* pDestPath, uint32 maxLen );
void S_GetFilePath( const std::string& srcPath, std::string& destPath, bool bShrinkToFit = true );


//-----------------------------------------------------------------------------
// Utility class for quick inquiries against filenames
//-----------------------------------------------------------------------------
class CFilename
{
public:
    CFilename();
    CFilename( const std::string& path );

    bool IsInDirectory( const std::string& dirPath ) const;
    std::string GetExtension( bool bIncludeDot = false ) const;
    const std::string& GetFullPath() const;
    std::string GetBaseName() const;
    std::string GetFileName() const;
    std::string GetPath() const;

private:
    std::string     path;
};

#include "stdlib/filetools.inl"
#if PLATFORM_WINDOWS
    #include "stdlib/platforms/windows/win_filetools.inl"
#else
    #error Unknown platform
#endif // PLATFORM_WINDOWS