#include "pch_game_shared.h"
#include "appframework/iwindowmgr.h"
#include "studiorender/istudiorender.h"
#include "resourcesystem/iresourcesystem.h"
#include "game/shared/ecs/ecs_core.h"
#include "game/shared/ecs/ecs_common.gen.h"
#include "game/shared/ecs/ecs_movement.gen.h"
#include "game/shared/ecs/ecs_camera.gen.h"
#include "game/shared/ecs/ecs_entitydesc.h"
#include "game/shared/game.h"

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
	// Connect Tier1 and register cvars
	if ( !ConnectTier1( pFactory ) )
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
	DisconnectTier1();

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

	// Register all game-specific resource types
	IResourceTypeMgr* pEcsDescsMgr = g_pResourceSystem->InstallResourceManagerForType<CEcsEntityDesc>();
	pEcsDescsMgr->RegisterResourceFactory( &ecsEntityDescFactory );
	pEcsDescsMgr->RegisterResourceLoader( &ecsEntityDescLoader );

	// Set a default resource for entity descriptors
	CResourcePtr<CEcsEntityDesc> pDefaultEntityDesc = pEcsDescsMgr->LoadResource( "//core/entities/default" );
	if ( !pDefaultEntityDesc )
	{
		pDefaultEntityDesc = pEcsDescsMgr->CreateResource( "__default" );
	}

	pEcsDescsMgr->SetDefaultResource( pDefaultEntityDesc );
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

	// Unregister all game-specific resource types
	g_pResourceSystem->RemoveResourceManagerForType<CEcsEntityDesc>();
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
	eastl::string		mapPath = S_GetFileExtension( pPath ) ? pPath : S_Sprintf( "%s.smap_c", pPath );
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
