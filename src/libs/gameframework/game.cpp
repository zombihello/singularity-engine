#include "pch_gameframework.h"
#include "appframework/iwindowmgr.h"
#include "studiorender/istudiorender.h"
#include "resourcesystem/iresourcesystem.h"
#include "gameframework/ecs/ecs_core.h"
#include "gameframework/ecs/ecs_common.gen.h"
#include "gameframework/ecs/ecs_movement.gen.h"
#include "gameframework/ecs/ecs_camera.gen.h"
#include "gameframework/game.h"

//-----------------------------------------------------------------------------
// Base game implementation
//-----------------------------------------------------------------------------
/*
==================
CGame::CGame
==================
*/
CGame::CGame()
{
}

/*
==================
CGame::Connect
==================
*/
bool CGame::Connect( createInterfaceFn_t pFactory )
{
	// Connect StdLib and register cvars
	if ( !ConnectStdLib( pFactory ) )
	{
		return false;
	}
	ConVar_Register();

	// Get the window manager
	g_pWindowMgr = (IWindowMgr*)pFactory( WINDOWMGR_INTERFACE_VERSION );
	if ( !g_pWindowMgr )
	{
		return false;
	}

	// Get StudioRender
	g_pStudioRender = (IStudioRender*)pFactory( STUDIORENDER_INTERFACE_VERSION );
	if ( !g_pStudioRender )
	{
		return false;
	}

	// Get the resource system
	g_pResourceSystem = (IResourceSystem*)pFactory( RESOURCESYSTEM_INTERFACE_VERSION );
	if ( !g_pResourceSystem )
	{
		return false;
	}

	return true;
}

/*
==================
CGame::Disconnect
==================
*/
void CGame::Disconnect()
{
	ConVar_Unregister();
	DisconnectStdLib();

	g_pWindowMgr	  = NULL;
	g_pStudioRender	  = NULL;
	g_pResourceSystem = NULL;
}

/*
==================
CGame::Init
==================
*/
bool CGame::Init()
{
	// Register GameFramework ECS modules and initialize the world
	extern void EcsInitModules_Gameframework();
	EcsInitModules_Gameframework();

	// Initialize all game-specific resource factories
	ecsEntityDescFactory.Init();
	return true;
}

/*
==================
CGame::Shutdown
==================
*/
void CGame::Shutdown()
{
	// Unregister all render objects
	g_pStudioRender->UnregisterAllObjects();

	// Shutdown the active map
	MapShutdown();

	// Shutdown all resource factories
	ecsEntityDescFactory.Shutdown();
}

/*
==================
CGame::MapInit
==================
*/
bool CGame::MapInit( const char* pPath )
{
	// Shutdown the old map
	MapShutdown();

	// Load a new map
	CSMAPCompiledMapDoc smapCompiledMapDoc;
	std::string			mapPath = S_GetFileExtension( pPath ) ? pPath : S_Sprintf( "%s.smap_c", pPath );
	if ( !smapCompiledMapDoc.LoadFromFile( mapPath.c_str() ) )
	{
		Warning( "Game: Failed to load map '%s'", mapPath.c_str() );
		return false;
	}

	pActiveEcsMap = new CEcsMap( smapCompiledMapDoc );
	Msg( "Game: Map '%s' loaded", mapPath.c_str() );
	return true;
}

/*
==================
CGame::MapShutdown
==================
*/
void CGame::MapShutdown()
{
	// Reset the active map
	if ( pActiveEcsMap )
	{
		delete pActiveEcsMap;
		pActiveEcsMap = NULL;
		Msg( "Game: Active map unloaded" );
	}
}

/*
==================
CGame::FrameUpdate
==================
*/
void CGame::FrameUpdate()
{
	if ( pActiveEcsMap )
	{
		pActiveEcsMap->Update( 0.f );
	}
}

//-----------------------------------------------------------------------------
// Game application systems implementation
//-----------------------------------------------------------------------------
/*
==================
CGameAppSystems::GetNum
==================
*/
uint32 CGameAppSystems::GetNum() const
{
	return (uint32)appSystems.size();
}

/*
==================
CGameAppSystems::GetModuleName
==================
*/
gameAppSystemInfo_t CGameAppSystems::GetInfo( uint32 index ) const
{
	return appSystems[index];
}