#include "pch_engine.h"
#include "core/crashdump.h"
#include "gameframework/igame.h"
#include "filesystem/ifilesystem.h"
#include "engine/engine.h"
#include "engine/appsystemgroup_game.h"

/*
==================
CGameAppSystemGroup::CGameAppSystemGroup
==================
*/
CGameAppSystemGroup::CGameAppSystemGroup( CAppSystemGroup* pParentAppSystem /* = NULL */ )
	: CAppSystemGroup( pParentAppSystem )
	, gameDLLHandle( NULL )
	, pGameFactory( NULL )
{}

/*
==================
CGameAppSystemGroup::Create
==================
*/
bool CGameAppSystemGroup::Create()
{
	// Load game dll
	if ( !GameDLL_Load() )
	{
		Sys_Error( "Failed to load game" DLL_EXT_STRING );
		return false;
	}

	// Add to the group the game's app systems if IGameAppSystems is exist
	IGameAppSystems*				pGameAppSystems = ( IGameAppSystems* )pGameFactory( GAME_APPSYSTEMS_INTERFACE_VERSION );
	std::vector<appSystemInfo_t>	gameAppSystems;
	if ( pGameAppSystems )
	{
		// Add each app system into our array
		for ( uint32 index = 0, count = pGameAppSystems->GetNum(); index < count; ++index )
		{
			appSystemInfo_t&				appSystemInfo = gameAppSystems.emplace_back();
			appSystemInfo.pModuleName		= pGameAppSystems->GetModuleName( index );
			appSystemInfo.pInterfaceName	= pGameAppSystems->GetInterfaceName( index );
		}		
	}

	// Terminate of array
	appSystemInfo_t&					nullTerminateInfo = gameAppSystems.emplace_back();
	nullTerminateInfo.pModuleName		= "";
	nullTerminateInfo.pInterfaceName	= "";

	// Add all the game's app systems into our group
	if ( !AddSystems( gameAppSystems.data() ) )
	{
		return false;
	}

	// We are done!
	return true;
}

/*
==================
CGameAppSystemGroup::PreInit
==================
*/
bool CGameAppSystemGroup::PreInit()
{
	return true;
}

/*
==================
CGameAppSystemGroup::PostInit
==================
*/
bool CGameAppSystemGroup::PostInit()
{
	// Initialize the game engine
	if ( !g_Engine.Init() )
	{
		return false;
	}
	return true;
}

/*
==================
CGameAppSystemGroup::Main
==================
*/
int32 CGameAppSystemGroup::Main()
{
	// Initialize the profiler
	PROFILE_INIT()

	// Main game loop
	while ( !Sys_IsRequestingExit() )
	{
		// Process window events and update game frame
		PROFILE_FRAME( "Main Thread" );
		g_pWindowMgr->ProcessEvents();
		g_Engine.FrameUpdate();
	}

	// Flush render thread commands before shutdown the application
	Studio_FlushRenderCommands();

	// Shutdown the profiler
	PROFILE_SHUTDOWN()
	return 0;
}

/*
==================
CGameAppSystemGroup::PreShutdown
==================
*/
void CGameAppSystemGroup::PreShutdown()
{
	// Shutdown the game engine
	g_Engine.Shutdown();
}

/*
==================
CGameAppSystemGroup::PostShutdown
==================
*/
void CGameAppSystemGroup::PostShutdown()
{}

/*
==================
CGameAppSystemGroup::Destroy
==================
*/
void CGameAppSystemGroup::Destroy()
{
	// Unload game dll
	GameDLL_Unload();
}

/*
==================
CGameAppSystemGroup::GameDLL_Load
==================
*/
bool CGameAppSystemGroup::GameDLL_Load()
{
	// Do nothing if game dll already loaded
	if ( gameDLLHandle )
	{
		return true;
	}

	// Load game dll
	gameDLLHandle = g_pFileSystem->LoadModule( "//GAMEBIN/game" DLL_EXT_STRING );
	if ( !gameDLLHandle )
	{
		Warning( "Engine: Failed to load '//GAMEBIN/game" DLL_EXT_STRING "'" );
		GameDLL_Unload();
		return false;
	}

	// Load interface factory and any interfaces exported by the game dll
	pGameFactory = Sys_GetFactory( gameDLLHandle );
	if ( pGameFactory )
	{
		// Get game interface from dll
		g_pGame = ( IGame* )pGameFactory( GAME_INTERFACE_VERSION );
		if ( !g_pGame )
		{
			Warning( "Engine: Could not get IGame interface from '//GAMEBIN/game" DLL_EXT_STRING "'" );
			GameDLL_Unload();
			return false;
		}
	}
	else
	{
		Warning( "Engine: Could not find factory interface in '//GAMEBIN/game" DLL_EXT_STRING "'" );
		GameDLL_Unload();
		return false;
	}

	// We are done, all ok
	Msg( "Engine: game" DLL_EXT_STRING " loaded for %s", g_pGame->GetGameDescription() );
	return true;
}

/*
==================
CGameAppSystemGroup::GameDLL_Unload
==================
*/
void CGameAppSystemGroup::GameDLL_Unload()
{
	if ( gameDLLHandle )
	{
		Msg( "Engine: game" DLL_EXT_STRING " unloaded" );
		g_pFileSystem->UnloadModule( gameDLLHandle );
		CrashDump_SetAppInfo( crashDumpAppInfo_t{ NULL, NULL, NULL, NULL } );

		gameDLLHandle	= NULL;
		pGameFactory	= NULL;
		g_pGame			= NULL;
	}
}