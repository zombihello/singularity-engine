#include "interfaces/interfaces.h"
#include "stdlib/template.h"
#include "stdlib/filetools.h"
#include "core/debug.h"
#include "core/profile.h"
#include "filesystem/ifilesystem.h"
#include "gameinfo/gameinfo.h"

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
CGameInfoDoc::LoadFromFile
==================
*/
bool CGameInfoDoc::LoadFromFile( const char* pPath )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Do nothing if file system isn't valid
	Assert( g_pFileSystem );
	if ( !g_pFileSystem )
	{
		return false;
	}

	// Clear a game info data
	Clear();

	// Try to open file
	TRefPtr<IStreamDataReader> pFile = g_pFileSystem->CreateFileReader( pPath );
	if ( !pFile )
	{
		return false;
	}

	// Allocate memory for buffer
	uint64 fileSize = pFile->GetSize() + 1;
	byte*  pBuffer	= (byte*)Mem_MallocZero( fileSize );

	// Serialize data to string buffer
	pFile->Read( pBuffer, fileSize );

	// Load JSON file and free allocated memory for buffer
	CJsonDoc jsonGameInfo;
	bool	 bResult = jsonGameInfo.LoadFromBuffer( (const char*)pBuffer );
	Mem_Free( pBuffer );
	if ( !bResult )
	{
		return false;
	}

	// If all ok grab data from JSON
	std::string gameinfoPath;
	S_GetFilePath( pFile->GetPath(), gameinfoPath, false );
	bLoaded = GrabData( jsonGameInfo, gameinfoPath.c_str() );
	return bLoaded;
}

/*
==================
CGameInfoDoc::LoadFromBuffer
==================
*/
bool CGameInfoDoc::LoadFromBuffer( const char* pBuffer, const char* pGameInfoPath )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Clear a game info data
	Clear();

	// Load JSON from buffer
	CJsonDoc jsonGameInfo;
	if ( !jsonGameInfo.LoadFromBuffer( pBuffer ) )
	{
		return false;
	}

	// If all ok grab data from JSON
	bLoaded = GrabData( jsonGameInfo, pGameInfoPath );
	return bLoaded;
}

/*
==================
CGameInfoDoc::GrabData
==================
*/
bool CGameInfoDoc::GrabData( const CJsonDoc& jsonDoc, const char* pGameInfoPath )
{
	PROFILE_SCOPE();

	// Get game name
	{
		CJsonValue jsonGameVar = jsonDoc.GetValue( "game" );
		if ( jsonGameVar.IsValid() )
		{
			if ( jsonGameVar.IsA( JSONVALUE_TYPE_STRING ) )
			{
				game = jsonGameVar.GetString();
			}
			else
			{
				Warning( "GameInfo: Invalid 'game', must be string type" );
			}
		}
	}

	// Get game version
	{
		CJsonValue jsonVersionVar = jsonDoc.GetValue( "version" );
		if ( jsonVersionVar.IsValid() )
		{
			if ( jsonVersionVar.IsA( JSONVALUE_TYPE_STRING ) )
			{
				version = jsonVersionVar.GetString();
			}
			else
			{
				Warning( "GameInfo: Invalid 'version', must be string type" );
			}
		}
	}

	// Get support email
	{
		CJsonValue jsonSupportEmailVar = jsonDoc.GetValue( "support_email" );
		if ( jsonSupportEmailVar.IsValid() )
		{
			if ( jsonSupportEmailVar.IsA( JSONVALUE_TYPE_STRING ) )
			{
				supportEmail = jsonSupportEmailVar.GetString();
			}
			else
			{
				Warning( "GameInfo: Invalid 'support_email', must be string type" );
			}
		}
	}

	// Get support URL
	{
		CJsonValue jsonSupportURLVar = jsonDoc.GetValue( "support_url" );
		if ( jsonSupportURLVar.IsValid() )
		{
			if ( jsonSupportURLVar.IsA( JSONVALUE_TYPE_STRING ) )
			{
				supportURL = jsonSupportURLVar.GetString();
			}
			else
			{
				Warning( "GameInfo: Invalid 'support_url', must be string type" );
			}
		}
	}

	// Get search paths
	{
		CJsonValue jsonSearchPaths = jsonDoc.GetValue( "search_paths" );
		if ( jsonSearchPaths.IsValid() )
		{
			if ( jsonSearchPaths.IsA( JSONVALUE_TYPE_ARRAY ) )
			{
				std::vector<CJsonValue> jsonVSearchPaths = jsonSearchPaths.GetArray();
				for ( uint32 index = 0, count = (uint32)jsonVSearchPaths.size(); index < count; ++index )
				{
					bool			  bInvalidElement = true;
					const CJsonValue& jsonValue		  = jsonVSearchPaths[index];
					if ( jsonValue.IsValid() )
					{
						if ( jsonValue.IsA( JSONVALUE_TYPE_OBJECT ) )
						{
							CJsonObject jsonSearchPath = jsonValue.GetObject();
							CJsonValue	jsonID		   = jsonSearchPath.GetValue( "id" );
							CJsonValue	jsonPath	   = jsonSearchPath.GetValue( "path" );
							if ( jsonID.IsValid() && jsonID.IsA( JSONVALUE_TYPE_STRING ) && jsonPath.IsValid() && jsonPath.IsA( JSONVALUE_TYPE_STRING ) )
							{
								gameInfoSearchPath_t& searchPath = searchPaths.emplace_back();
								searchPath.id					 = jsonID.GetString();
								searchPath.path					 = jsonPath.GetString();
								ReplaceMacros( searchPath.path, pGameInfoPath );
								bInvalidElement = false;
							}
						}
					}

					if ( bInvalidElement )
					{
						Warning( "GameInfo: Invalid 'search_paths[%i]'. Must have 'id' and 'path' (string types)", index );
					}
				}
			}
			else
			{
				Warning( "GameInfo: Invalid 'search_paths', must be array type" );
			}
		}
	}

	return true;
}

/*
==================
CGameInfoDoc::ReplaceMacros
==================
*/
void CGameInfoDoc::ReplaceMacros( std::string& string, const char* pGameInfoPath )
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
		while ( ( startPos = string.find( pCurMacro, startPos ) ) != std::string::npos )
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