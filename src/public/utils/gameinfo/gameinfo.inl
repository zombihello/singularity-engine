#pragma once

/*
==================
gameInfoCrashDump_t::Clear
==================
*/
FORCEINLINE void gameInfoCrashDump_t::Clear()
{
	supportEmail.clear();
	supportURL.clear();
}

/*
==================
CGameInfoDoc::LoadFromFile
==================
*/
FORCEINLINE bool CGameInfoDoc::LoadFromFile( const char* pPath )
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
CGameInfoDoc::LoadFromBuffer
==================
*/
FORCEINLINE bool CGameInfoDoc::LoadFromBuffer( const char* pBuffer )
{
	PROFILER_SCOPE_FUNC();
	CStreamDataMemoryReader streamReader( (byte*)pBuffer, S_Strlen( pBuffer ) );
	return LoadFromStream( &streamReader );
}

/*
==================
CGameInfoDoc::Clear
==================
*/
FORCEINLINE void CGameInfoDoc::Clear()
{
	game.clear();
	version.clear();
	crashDump.Clear();
	searchPaths.clear();
	bLoaded = false;
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
CGameInfoDoc::GetCrashDump
==================
*/
FORCEINLINE const gameInfoCrashDump_t& CGameInfoDoc::GetCrashDump() const
{
	return crashDump;
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
