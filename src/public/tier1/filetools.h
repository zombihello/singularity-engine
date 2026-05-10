#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier1/strtools.h"
#include "filesystem/ifilesystem.h"
#include "utils/interfaces/interfaces.h"

//-----------------------------------------------------------------------------
// File tools
//-----------------------------------------------------------------------------
bool  S_MakeAbsolutePath( const char* pSrcPath, char* pDestPath, uint32 maxLen, const char* pStartingDir = NULL );
void  S_MakeAbsolutePath( const eastl::string& srcPath, eastl::string& destPath, const eastl::string& startingDir = "", bool bShrinkToFit = true );
int32 S_MakeRelativePath( const char* pFullPath, const char* pDirPath, char* pRelativePath, uint32 maxLen );
bool  S_MakeRelativePath( const eastl::string& fullPath, const eastl::string& dirPath, eastl::string& relativePath, bool bShrinkToFit = true );

bool S_IsAbsolutePath( const char* pPath );
bool S_IsAbsolutePath( const eastl::string& path );
bool S_IsPathSeparator( char c );
bool S_HasPathSeparator( const char* pPath );
bool S_HasPathSeparator( const eastl::string& path );
bool S_AppendPathSeparator( char* pStr, uint32 strSize );
void S_AppendPathSeparator( eastl::string& str );
void S_FixPathSeparators( char* pPath );
void S_FixPathSeparators( eastl::string& path );
void S_RemoveDotPathSeparators( char* pPath, bool bRemoveDoubleSeparators = true );
void S_RemoveDotPathSeparators( eastl::string& path, bool bRemoveDoubleSeparators = true );

const char* S_GetFileExtension( const char* pPath, bool bIncludeDot = false );
void		S_GetFileExtension( const eastl::string& path, eastl::string& extension, bool bIncludeDot = false );
bool		S_GetFileBaseName( const char* pPath, char* pFileName, uint32 maxLen );
void		S_GetFileBaseName( const eastl::string& path, eastl::string& fileName, bool bShrinkToFit = true );
const char* S_GetFileName( const char* pPath );
void		S_GetFileName( const eastl::string& path, eastl::string& fileName );
bool		S_GetFilePath( const char* pSrcPath, char* pDestPath, uint32 maxLen );
void		S_GetFilePath( const eastl::string& srcPath, eastl::string& destPath, bool bShrinkToFit = true );

bool S_LoadFileToArray( const char* pPath, eastl::vector<byte>& result, uint32 flags = FILE_READ_NONE );
bool S_LoadFileToString( const char* pPath, eastl::string& result, uint32 flags = FILE_READ_NONE );
bool S_SaveArrayToFile( const char* pPath, const eastl::vector<byte>& data, uint32 flags = FILE_WRITE_NONE );
bool S_SaveStringToFile( const char* pPath, const eastl::string& data, uint32 flags = FILE_WRITE_NONE );

// Supported placeholders: |platform_dir|, |platform_bin_dir|
void		  S_FillPathPlaceholders( eastl::string& path );
void		  S_FillPathPlaceholders( const char* pSrcPath, eastl::string& destPath );
eastl::string S_FillPathPlaceholders( const char* pPath );

//-----------------------------------------------------------------------------
// Utility class for quick inquiries against filenames
//-----------------------------------------------------------------------------
class CFilename
{
public:
	CFilename();
	CFilename( const eastl::string& path );

	bool				 IsInDirectory( const eastl::string& dirPath ) const;
	eastl::string		 GetExtension( bool bIncludeDot = false ) const;
	const eastl::string& GetFullPath() const;
	eastl::string		 GetBaseName() const;
	eastl::string		 GetFileName() const;
	eastl::string		 GetPath() const;

private:
	eastl::string path;
};

#include "tier1/filetools.inl"
#if PLATFORM_WINDOWS
	#include "tier1/platforms/windows/win_filetools.inl"
#else
	#error Unknown platform
#endif	// PLATFORM_WINDOWS
