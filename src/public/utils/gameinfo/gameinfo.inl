#pragma once

/*
==================
CGameInfoDoc::LoadFromFile
==================
*/
FORCEINLINE bool CGameInfoDoc::LoadFromFile( const char* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try to open the file
	TRefPtr<IStreamDataReader> pFile = g_pFileSystem->CreateFileReader( pPath );
	if ( !pFile )
	{
		return false;
	}

	// Load a gameinfo from the stream
	eastl::string gameinfoPath;
	S_GetFilePath( pFile->GetPath(), gameinfoPath, false );
	return LoadFromStream( pFile, gameinfoPath.c_str() );
}

/*
==================
CGameInfoDoc::LoadFromBuffer
==================
*/
FORCEINLINE bool CGameInfoDoc::LoadFromBuffer( const char* pBuffer, const char* pGameInfoPath )
{
	PROFILE_SCOPE();
	CStreamDataMemoryReader streamReader( (byte*)pBuffer, S_Strlen( pBuffer ) );
	return LoadFromStream( &streamReader, pGameInfoPath );
}

/*
==================
CGameInfoDoc::GetGame
==================
*/
FORCEINLINE const eastl::string& CGameInfoDoc::GetGame() const
{
	return game;
}

/*
==================
CGameInfoDoc::GetVersion
==================
*/
FORCEINLINE const eastl::string& CGameInfoDoc::GetVersion() const
{
	return version;
}

/*
==================
CGameInfoDoc::GetSupportEmail
==================
*/
FORCEINLINE const eastl::string& CGameInfoDoc::GetSupportEmail() const
{
	return supportEmail;
}

/*
==================
CGameInfoDoc::GetSupportURL
==================
*/
FORCEINLINE const eastl::string& CGameInfoDoc::GetSupportURL() const
{
	return supportURL;
}

/*
==================
CGameInfoDoc::GetSearchPaths
==================
*/
FORCEINLINE const eastl::vector<gameInfoSearchPath_t>& CGameInfoDoc::GetSearchPaths() const
{
	return searchPaths;
}

/*
==================
CGameInfoDoc::IsLoaded
==================
*/
FORCEINLINE bool CGameInfoDoc::IsLoaded() const
{
	return bLoaded;
}
