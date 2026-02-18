#include "utils/interfaces/interfaces.h"
#include "tier1/template.h"
#include "tier0/debug.h"
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
bool CGameInfoDoc::LoadFromStream( IStreamDataReader* pStreamReader, const char* pGameInfoPath /* = "" */ )
{
	// Clear a game info data
	PROFILE_SCOPE();
	Clear();

	// Load key values
	CKeyValues keyValues( "gameinfo" );
	if ( !keyValues.LoadFromStream( pStreamReader ) )
	{
		return false;
	}

	// Get values from the key value
	game		 = keyValues.GetString( "game", "Game" );
	version		 = keyValues.GetString( "version" );
	supportEmail = keyValues.GetString( "support_email" );
	supportURL	 = keyValues.GetString( "support_url" );

	// Initialize search paths
	CKeyValues* pSearchPaths = keyValues.FindKey( "search_paths" );
	if ( pSearchPaths )
	{
		for ( CKeyValuesSubKeysIterator it( pSearchPaths ); it; ++it )
		{
			gameInfoSearchPath_t& searchPath = searchPaths.emplace_back();
			searchPath.id					 = it->GetName();
			searchPath.path					 = it->GetString( NULL );
			ReplaceMacros( searchPath.path, pGameInfoPath );
		}
	}

	// We are done
	bLoaded = true;
	return bLoaded;
}

/*
==================
CGameInfoDoc::ReplaceMacros
==================
*/
void CGameInfoDoc::ReplaceMacros( eastl::string& string, const char* pGameInfoPath )
{
	PROFILE_SCOPE();
	enum gameInfoMacro_t
	{
		GAMEINFO_MACRO_PLATFORM_DIR,	  // |platform_dir|
		GAMEINFO_MACRO_PLATFORM_BIN_DIR,  // |platform_bin_dir|
		GAMEINFO_MACRO_GAMEINFO_PATH,	  // |gameinfo_path|
		GAMEINFO_NUM_MACROS				  // Number of macros in gameinfo.txt
	};

	// Table of all macros in gameinfo.txt
	static const char* s_pMacrosTable[] = {
		"|platform_dir|",	   // GAMEINFO_MACRO_PLATFORM_DIR
		"|platform_bin_dir|",  // GAMEINFO_MACRO_PLATFORM_BIN_DIR
		"|gameinfo_path|"	   // GAMEINFO_MACRO_GAMEINFO_PATH
	};

	// Table of macros value
	static const char* s_pValueMacroTable[] = {
		PLATFORM_DIR,	   // |platform_dir|
		PLATFORM_BIN_DIR,  // |platform_bin_dir|
		"",				   // |gameinfo_path|
	};
	static_assert( ARRAYSIZE( s_pMacrosTable ) == GAMEINFO_NUM_MACROS && ARRAYSIZE( s_pValueMacroTable ) == GAMEINFO_NUM_MACROS, "Need full init s_pMacrosTable and s_pValueMacroTable array" );

	// Update value for |gameinfo_path|
	s_pValueMacroTable[GAMEINFO_MACRO_GAMEINFO_PATH] = pGameInfoPath;

	// Replace all macros in the string
	for ( uint32 macroIndex = 0; macroIndex < GAMEINFO_NUM_MACROS; ++macroIndex )
	{
		const char* pCurMacro = s_pMacrosTable[macroIndex];
		const char* pCurValue = s_pValueMacroTable[macroIndex];

		size_t startPos = 0;
		while ( ( startPos = string.find( pCurMacro, startPos ) ) != eastl::string::npos )
		{
			string.replace( startPos, S_Strlen( pCurMacro ), pCurValue );
			startPos += S_Strlen( pCurValue );
		}
	}
}

/*
==================
CGameInfoDoc::Clear
==================
*/
void CGameInfoDoc::Clear()
{
	game.clear();
	searchPaths.clear();
	bLoaded = false;
}
