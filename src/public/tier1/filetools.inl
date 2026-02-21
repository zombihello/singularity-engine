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
S_HasPathSeparator
==================
*/
FORCEINLINE bool S_HasPathSeparator( const char* pPath )
{
	// Do nothing if the buffer isn't valid
	if ( !pPath || !pPath[0] )
	{
		return false;
	}

	uint32 length = S_Strlen( pPath );
	Assert( length > 0 );
	return S_IsPathSeparator( pPath[length - 1] );
}

/*
==================
S_HasPathSeparator
==================
*/
FORCEINLINE bool S_HasPathSeparator( const eastl::string& path )
{
	return S_HasPathSeparator( path.c_str() );
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
S_LoadFileToArray
==================
*/
FORCEINLINE bool S_LoadFileToArray( const char* pPath, eastl::vector<byte>& result, uint32 flags /* = FILE_READ_NONE */ )
{
	Assert( g_pFileSystem );
	TRefPtr<IStreamDataReader> pFile = g_pFileSystem->CreateFileReader( pPath, flags );
	if ( !pFile )
	{
		Error( "Tier1: Failed to load file '%s'", pPath );
		return false;
	}

	result.resize( pFile->GetSize() );
	pFile->Read( result.data(), result.size() );
	return true;
}

/*
==================
S_LoadFileToString
==================
*/
FORCEINLINE bool S_LoadFileToString( const char* pPath, eastl::string& result, uint32 flags /* = FILE_READ_NONE */ )
{
	Assert( g_pFileSystem );
	TRefPtr<IStreamDataReader> pFile = g_pFileSystem->CreateFileReader( pPath, flags );
	if ( !pFile )
	{
		Error( "Tier1: Failed to load file '%s'", pPath );
		return false;
	}

	uint64 fileSize = pFile->GetSize();
	result.resize( fileSize / sizeof( eastl::string::value_type ) );
	pFile->Read( result.data(), fileSize );
	return true;
}

/*
==================
S_SaveArrayToFile
==================
*/
FORCEINLINE bool S_SaveArrayToFile( const char* pPath, const eastl::vector<byte>& data, uint32 flags /* = FILE_WRITE_NONE */ )
{
	Assert( g_pFileSystem );
	TRefPtr<IStreamDataWriter> pFile = g_pFileSystem->CreateFileWriter( pPath, flags );
	if ( !pFile )
	{
		Error( "Tier1: Failed to save file '%s'", pPath );
		return false;
	}

	pFile->Write( (byte*)data.data(), data.size() );
	return true;
}

/*
==================
S_SaveStringToFile
==================
*/
FORCEINLINE bool S_SaveStringToFile( const char* pPath, const eastl::string& data, uint32 flags /* = FILE_WRITE_NONE */ )
{
	Assert( g_pFileSystem );
	TRefPtr<IStreamDataWriter> pFile = g_pFileSystem->CreateFileWriter( pPath, flags );
	if ( !pFile )
	{
		Error( "Tier1: Failed to save file '%s'", pPath );
		return false;
	}

	pFile->Write( (byte*)data.data(), data.size() * sizeof( eastl::string::value_type ) );
	return true;
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
