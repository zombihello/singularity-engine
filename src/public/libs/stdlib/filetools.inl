#pragma once

/*
==================
S_IsPathSeparator
==================
*/
FORCEINLINE bool S_IsPathSeparator( char c )
{
	return c == '\\' || c == '/';
}

/*
==================
S_SetCurrentDirectory
==================
*/
FORCEINLINE bool S_SetCurrentDirectory( const eastl::string& dirName )
{
	return S_SetCurrentDirectory( dirName.c_str() );
}

/*
==================
S_IsAbsolutePath
==================
*/
FORCEINLINE bool S_IsAbsolutePath( const eastl::string& path )
{
	return S_IsAbsolutePath( path.c_str() );
}

/*
==================
S_FixPathSeparators
==================
*/
FORCEINLINE void S_FixPathSeparators( eastl::string& path )
{
	S_FixPathSeparators( path.data() );
}

/*
==================
S_GetFileExtension
==================
*/
FORCEINLINE void S_GetFileExtension( const eastl::string& path, eastl::string& extension, bool bIncludeDot /*= false*/ )
{
	extension = S_GetFileExtension( path.c_str(), bIncludeDot );
}

/*
==================
S_GetFileName
==================
*/
FORCEINLINE void S_GetFileName( const eastl::string& path, eastl::string& fileName )
{
	fileName = S_GetFileName( path.c_str() );
}

/*
==================
S_RemoveDotPathSeparators
==================
*/
FORCEINLINE void S_RemoveDotPathSeparators( eastl::string& path, bool bRemoveDoubleSeparators /*= true*/ )
{
	S_RemoveDotPathSeparators( path.data(), bRemoveDoubleSeparators );
	path.resize( S_Strlen( path.c_str() ) );
}

/*
==================
CFilename::GetExtension
==================
*/
FORCEINLINE eastl::string CFilename::GetExtension( bool bIncludeDot /*= false*/ ) const
{
	return S_GetFileExtension( path.c_str(), bIncludeDot );
}

/*
==================
CFilename::GetFullPath
==================
*/
FORCEINLINE const eastl::string& CFilename::GetFullPath() const
{
	return path;
}

/*
==================
CFilename::GetBaseName
==================
*/
FORCEINLINE eastl::string CFilename::GetBaseName() const
{
	eastl::string result;
	S_GetFileBaseName( path, result );
	return result;
}

/*
==================
CFilename::GetFileName
==================
*/
FORCEINLINE eastl::string CFilename::GetFileName() const
{
	eastl::string result;
	S_GetFileName( path, result );
	return result;
}

/*
==================
CFilename::S_RemoveDotPathSeparators
==================
*/
FORCEINLINE eastl::string CFilename::GetPath() const
{
	eastl::string result;
	S_GetFilePath( path, result );
	return result;
}

/*
==================
CFilename::S_RemoveDotPathSeparators
==================
*/
FORCEINLINE bool CFilename::IsInDirectory( const eastl::string& dirPath ) const
{
	eastl::string absoluteDirPath;
	eastl::string absolutePath;
	S_MakeAbsolutePath( dirPath, absoluteDirPath );
	S_MakeAbsolutePath( CFilename::path, absolutePath );

	// Compare two string
	return !S_Strnicmp( absolutePath.c_str(), absoluteDirPath.c_str(), (uint32)absoluteDirPath.size() );
}
