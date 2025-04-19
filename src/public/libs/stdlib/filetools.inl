#ifndef FILETOOLS_INL
#define FILETOOLS_INL

/*
==================
S_IsPathSeparator
==================
*/
FORCEINLINE bool S_IsPathSeparator( achar c )
{
	return c == '\\' || c == '/';
}

/*
==================
S_SetCurrentDirectory
==================
*/
FORCEINLINE bool S_SetCurrentDirectory( const std::string& dirName )
{
	return S_SetCurrentDirectory( dirName.c_str() );
}

/*
==================
S_IsAbsolutePath
==================
*/
FORCEINLINE bool S_IsAbsolutePath( const std::string& path )
{
	return S_IsAbsolutePath( path.c_str() );
}

/*
==================
S_FixPathSeparators
==================
*/
FORCEINLINE void S_FixPathSeparators( std::string& path )
{
	S_FixPathSeparators( path.data() );
}

/*
==================
S_GetFileExtension
==================
*/
FORCEINLINE void S_GetFileExtension( const std::string& path, std::string& extension, bool bIncludeDot /*= false*/ )
{
	extension = S_GetFileExtension( path.c_str(), bIncludeDot );
}

/*
==================
S_GetFileName
==================
*/
FORCEINLINE void S_GetFileName( const std::string& path, std::string& fileName )
{
	fileName = S_GetFileName( path.c_str() );
}

/*
==================
S_RemoveDotPathSeparators
==================
*/
FORCEINLINE void S_RemoveDotPathSeparators( std::string& path, bool bRemoveDoubleSeparators /*= true*/ )
{
	S_RemoveDotPathSeparators( path.data(), bRemoveDoubleSeparators );
	path.resize( S_Strlen( path.c_str() ) );
}


/*
==================
CFilename::GetExtension
==================
*/
FORCEINLINE std::string CFilename::GetExtension( bool bIncludeDot /*= false*/ ) const
{
    return S_GetFileExtension( path.c_str(), bIncludeDot );
}

/*
==================
CFilename::GetFullPath
==================
*/
FORCEINLINE const std::string& CFilename::GetFullPath() const
{
    return path;
}

/*
==================
CFilename::GetBaseName
==================
*/
FORCEINLINE std::string CFilename::GetBaseName() const
{
    std::string		result;
    S_GetFileBaseName( path, result );
    return result;
}

/*
==================
CFilename::GetFileName
==================
*/
FORCEINLINE std::string CFilename::GetFileName() const
{
    std::string		result;
    S_GetFileName( path, result );
    return result;
}

/*
==================
CFilename::S_RemoveDotPathSeparators
==================
*/
FORCEINLINE std::string CFilename::GetPath() const
{
    std::string		result;
    S_GetFilePath( path, result );
    return result;
}

/*
==================
CFilename::S_RemoveDotPathSeparators
==================
*/
FORCEINLINE bool CFilename::IsInDirectory( const std::string& dirPath ) const
{
    std::string		absoluteDirPath;
    std::string		absolutePath;
    S_MakeAbsolutePath( dirPath, absoluteDirPath );
    S_MakeAbsolutePath( CFilename::path, absolutePath );

    // Compare two string
    return !S_Strnicmp( absolutePath.c_str(), absoluteDirPath.c_str(), ( uint32 )absoluteDirPath.size() );
}

#endif // !FILETOOLS_INL
