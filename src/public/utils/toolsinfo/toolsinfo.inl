#pragma once

/*
==================
CToolsInfoDoc::LoadFromFile
==================
*/
FORCEINLINE bool CToolsInfoDoc::LoadFromFile( const char* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try to open the file
	CRefPtr<IStreamDataReader> pFile = g_pFileSystem->CreateFileReader( pPath );
	if ( !pFile )
	{
		return false;
	}

	// Load a gameinfo from the stream
	return LoadFromStream( pFile );
}

/*
==================
CToolsInfoDoc::LoadFromBuffer
==================
*/
FORCEINLINE bool CToolsInfoDoc::LoadFromBuffer( const char* pBuffer )
{
	PROFILER_SCOPE_FUNC();
	CStreamDataMemoryReader streamReader( (byte*)pBuffer, S_Strlen( pBuffer ) );
	return LoadFromStream( &streamReader );
}

/*
==================
CToolsInfoDoc::Clear
==================
*/
FORCEINLINE void CToolsInfoDoc::Clear()
{
	searchPaths.clear();
	resourceCompilers.clear();
	bLoaded = false;
}

/*
==================
CToolsInfoDoc::GetSearchPaths
==================
*/
FORCEINLINE const eastl::vector<toolsInfoSearchPath_t>& CToolsInfoDoc::GetSearchPaths() const
{
	return searchPaths;
}

/*
==================
CToolsInfoDoc::GetResourceCompilers
==================
*/
FORCEINLINE const eastl::vector<eastl::string>& CToolsInfoDoc::GetResourceCompilers() const
{
	return resourceCompilers;
}

/*
==================
CToolsInfoDoc::IsLoaded
==================
*/
FORCEINLINE bool CToolsInfoDoc::IsLoaded() const
{
	return bLoaded;
}
