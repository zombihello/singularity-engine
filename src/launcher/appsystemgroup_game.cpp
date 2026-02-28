#include "filesystem/ifilesystem.h"
#include "game/igame.h"
#include "utils/interfaces/interfaces.h"
#include "launcher/appsystemgroup_game.h"

/*
==================
CAppSystemGroupGame::CAppSystemGroupGame
==================
*/
CAppSystemGroupGame::CAppSystemGroupGame()
	: gameDLLHandle( INVALID_DLL_HANDLE )
	, pGameFactory( NULL )
{
}

/*
==================
CAppSystemGroupGame::Create
==================
*/
bool CAppSystemGroupGame::Create()
{
	// Load game dll
	if ( !GameDLL_Load() )
	{
		return false;
	}

	// Try get game systems
	Assert( pGameFactory );
	IGame*			 pGame			 = (IGame*)pGameFactory( GAME_INTERFACE_VERSION );
	IGameAppSystems* pGameAppSystems = (IGameAppSystems*)pGameFactory( GAME_APPSYSTEMS_INTERFACE_VERSION );
	if ( !pGame )
	{
		Warning( "Launcher: Could not get " GAME_INTERFACE_VERSION " from '//GAMEBIN/game" DLL_EXT_STRING "'" );
		GameDLL_Unload();
		return false;
	}

	// Add to the app game's app systems if IGameAppSystems is exist
	if ( pGameAppSystems )
	{
		eastl::vector<appSystemInfo_t> appSystems_before;
		eastl::vector<appSystemInfo_t> appSystems_after;
		for ( uint32 index = 0, count = pGameAppSystems->GetNum(); index < count; ++index )
		{
			gameAppSystemInfo_t gameAppSystem = pGameAppSystems->GetInfo( index );
			appSystemInfo_t		appSystem;
			appSystem.pModuleName	 = gameAppSystem.pModuleName;
			appSystem.pInterfaceName = gameAppSystem.pInterfaceName;

			switch ( gameAppSystem.order )
			{
			case GAME_APPSYSTEM_ORDER_BEFORE_GAME: appSystems_before.emplace_back( appSystem ); break;
			case GAME_APPSYSTEM_ORDER_AFTER_GAME: appSystems_after.emplace_back( appSystem ); break;
			default:
				AssertMsg( false, "Unknown game app system order 0x%X", gameAppSystem.order );
				return false;
			}
		}

		// Terminate arrays
		appSystemInfo_t nullTerminateInfo = { "", "" };
		appSystems_before.emplace_back( nullTerminateInfo );
		appSystems_after.emplace_back( nullTerminateInfo );

		// Add all the game's app systems into our group
		if ( !AddSystems( appSystems_before.data() ) )
		{
			return false;
		}
		AddSystem( pGame, GAME_INTERFACE_VERSION );
		if ( !AddSystems( appSystems_after.data() ) )
		{
			return false;
		}
	}
	// Otherwise just register the game
	else
	{
		AddSystem( pGame, GAME_INTERFACE_VERSION );
	}

	Msg( "Launcher: '//GAMEBIN/game" DLL_EXT_STRING "' is %s", pGame->GetGameDescription() );
	return true;
}

/*
==================
CAppSystemGroupGame::Destroy
==================
*/
void CAppSystemGroupGame::Destroy()
{
	GameDLL_Unload();
}

/*
==================
CAppSystemGroupGame::GameDLL_Load
==================
*/
bool CAppSystemGroupGame::GameDLL_Load()
{
	// Load a game dll
	AssertMsg( !gameDLLHandle, "Cannot load game module twice!" );
	gameDLLHandle = g_pFileSystem->LoadModule( "//GAMEBIN/game" DLL_EXT_STRING );
	if ( !gameDLLHandle )
	{
		Warning( "Launcher: Failed to load '//GAMEBIN/game" DLL_EXT_STRING "'" );
		GameDLL_Unload();
		return false;
	}

	// Get an interface factory of the game dll
	pGameFactory = Sys_GetFactory( gameDLLHandle );
	if ( !pGameFactory )
	{
		Warning( "Launcher: Could not find " CREATEINTERFACE_FUNCNAME " in '//GAMEBIN/game" DLL_EXT_STRING "'" );
		GameDLL_Unload();
		return false;
	}

	// We are done, all ok
	Msg( "Launcher: '//GAMEBIN/game" DLL_EXT_STRING "' loaded" );
	return true;
}

/*
==================
CAppSystemGroupGame::GameDLL_Unload
==================
*/
void CAppSystemGroupGame::GameDLL_Unload()
{
	if ( gameDLLHandle )
	{
		Msg( "Launcher: game" DLL_EXT_STRING " unloaded" );
		g_pFileSystem->UnloadModule( gameDLLHandle );
		gameDLLHandle = NULL;
		pGameFactory  = NULL;
	}
}

/*
==================
CAppSystemGroupGame::GetName
==================
*/
const char* CAppSystemGroupGame::GetName() const
{
	return "Game";
}
