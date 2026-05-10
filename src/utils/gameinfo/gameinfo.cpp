#include "utils/interfaces/interfaces.h"
#include "tier1/template.h"
#include "filesystem/ifilesystem.h"
#include "utils/gameinfo/gameinfo.h"

/*
==================
CGameInfoDoc::CGameInfoDoc
==================
*/
CGameInfoDoc::CGameInfoDoc()
	: bLoaded( false )
{
}

/*
==================
CGameInfoDoc::LoadFromStream
==================
*/
bool CGameInfoDoc::LoadFromStream( IStreamDataReader* pStreamReader )
{
	// Clear a game info data
	PROFILER_SCOPE_FUNC();
	Clear();

	// Load key values
	CKeyValues keyValues( "gameinfo" );
	if ( !keyValues.LoadFromStream( pStreamReader ) )
	{
		return false;
	}

	// Get game and version from the key values
	game	= keyValues.GetString( "game", "Singularity Game" );
	version = keyValues.GetString( "version" );

	// Get some information for a crash dump
	CKeyValues* pCrashDump = keyValues.FindKey( "crash_dump" );
	if ( pCrashDump )
	{
		crashDump.supportEmail = pCrashDump->GetString( "support_email" );
		crashDump.supportURL   = pCrashDump->GetString( "support_url" );
	}

	// Get some information for the file system
	CKeyValues* pFileSystem = keyValues.FindKey( "file_system" );
	if ( pFileSystem )
	{
		// Get search paths
		CKeyValues* pSearchPaths = pFileSystem->FindKey( "search_paths" );
		if ( pSearchPaths )
		{
			for ( CKeyValuesSubKeysIterator it( pSearchPaths ); it; ++it )
			{
				gameInfoSearchPath_t& searchPath = searchPaths.emplace_back();
				searchPath.id					 = it->GetName();
				searchPath.path					 = it->GetString( NULL );
				S_FillPathPlaceholders( searchPath.path );
			}
		}
	}

	// We are done
	bLoaded = true;
	return bLoaded;
}
