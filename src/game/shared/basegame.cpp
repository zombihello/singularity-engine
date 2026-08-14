#include "pch_game_shared.h"
#include "tier1/math/math.h"
#include "appframework/iwindowmgr.h"
#include "inputsystem/iinputsystem.h"
#include "studiorender/istudiorender.h"
#include "studiorender/istudio_viewport.h"
#include "resourcesystem/iresourcesystem.h"
#include "game/shared/ecs/ecs_core.h"
#include "game/shared/ecs/ecs_common.gen.h"
#include "game/shared/ecs/ecs_entitydesc.h"
#include "game/shared/baseplayer.h"
#include "game/shared/basegame.h"

/*
==================
CBaseGame::CBaseGame
==================
*/
CBaseGame::CBaseGame()
	: pActiveEcsMap( NULL )
	, pPlayer( NULL )
{
}

/*
==================
CBaseGame::Connect
==================
*/
bool CBaseGame::Connect( createInterfaceFn_t pFactory )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_GAMELOGIC );

	// Connect Tier1 and register cvars
	if ( !ConnectTier1( pFactory ) )
	{
		return false;
	}
	LinkCmds();
	LinkCVars();

	// Get the window manager
	g_pWindowMgr = (IWindowMgr*)pFactory( WINDOWMGR_INTERFACE_VERSION );
	if ( !g_pWindowMgr )
	{
		return false;
	}

	// Get the input system
	g_pInputSystem = (IInputSystem*)pFactory( INPUTSYSTEM_INTERFACE_VERSION );
	if ( !g_pInputSystem )
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
CBaseGame::Disconnect
==================
*/
void CBaseGame::Disconnect()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_GAMELOGIC );
	UnlinkCVars();
	UnlinkCmds();
	DisconnectTier1();

	g_pWindowMgr	  = NULL;
	g_pInputSystem	  = NULL;
	g_pStudioRender	  = NULL;
	g_pResourceSystem = NULL;
}

/*
==================
CBaseGame::Init
==================
*/
bool CBaseGame::Init()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_GAMELOGIC );

	// Register GameFramework ECS modules and initialize the world
	extern void EcsInitModules_Gameframework();
	EcsInitModules_Gameframework();

	// Register all game-specific resource types
	IResourceTypeMgr* pEcsDescsMgr = g_pResourceSystem->InstallResourceManagerForType<CEcsEntityDesc>();
	pEcsDescsMgr->RegisterResourceFactory( &ecsEntityDescFactory );
	pEcsDescsMgr->RegisterResourceLoader( &ecsEntityDescLoader );

	// Set a default resource for entity descriptors
	CResourcePtr<CEcsEntityDesc> pDefaultEntityDesc = pEcsDescsMgr->LoadResource( "__default", "//core/entities/default" );
	if ( !pDefaultEntityDesc )
	{
		pDefaultEntityDesc = pEcsDescsMgr->CreateResource( "__default" );
	}

	pEcsDescsMgr->SetDefaultResource( pDefaultEntityDesc );
	return true;
}

/*
==================
CBaseGame::Shutdown
==================
*/
void CBaseGame::Shutdown()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_GAMELOGIC );

	// Shutdown the active map
	MapShutdown();

	// Unregister all game-specific resource types
	g_pResourceSystem->RemoveResourceManagerForType<CEcsEntityDesc>();
}

/*
==================
CBaseGame::MapInit
==================
*/
bool CBaseGame::MapInit( const char* pPath )
{
	// Shutdown the old map
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_GAMELOGIC );
	MapShutdown();
	Assert( !pActiveEcsMap && !pPlayer );

	// Load a new map
	CSMAPCompiledMapDoc smapCompiledMapDoc;
	eastl::string		mapPath = S_GetFileExtension( pPath ) ? pPath : S_Sprintf( "%s.smap_c", pPath );
	if ( !smapCompiledMapDoc.LoadFromFile( mapPath.c_str() ) )
	{
		Warning( "Game: Failed to load map '%s'", mapPath.c_str() );
		return false;
	}

	pActiveEcsMap = new CEcsMap();
	pActiveEcsMap->Init( smapCompiledMapDoc );

	// Create a new player
	pPlayer = CreatePlayer();
	Msg( "Game: Map '%s' loaded", mapPath.c_str() );
	return true;
}

/*
==================
CBaseGame::MapShutdown
==================
*/
void CBaseGame::MapShutdown()
{
	// Destroy the player
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_GAMELOGIC );
	if ( pPlayer )
	{
		delete pPlayer;
		pPlayer = NULL;
	}

	// Reset the active map
	if ( pActiveEcsMap )
	{
		delete pActiveEcsMap;
		pActiveEcsMap = NULL;
		Msg( "Game: Active map unloaded" );
	}

	// Uncache all resources
	g_pResourceSystem->UncacheAllResources();
}

/*
==================
CBaseGame::FrameUpdate
==================
*/
void CBaseGame::FrameUpdate( float deltaTime )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_GAMELOGIC );
	if ( pActiveEcsMap )
	{
		pActiveEcsMap->Update( deltaTime );
		pPlayer->Update( deltaTime );
	}
}

/*
==================
CBaseGame::FrameDraw
==================
*/
void CBaseGame::FrameDraw( IStudioViewport* pStudioViewport )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	if ( pActiveEcsMap )
	{
		vector2i_t			 viewportSize	  = pStudioViewport->GetSize();
		const playerState_t& playerState	  = pPlayer->GetState();
		studioCameraView_t	 studioCameraView = {};
		studioCameraView.location			  = playerState.location;
		studioCameraView.rotation			  = playerState.viewAngles.ToQuaternion();
		studioCameraView.fieldOfView		  = 90.f;
		studioCameraView.nearClipPlane		  = 1.f;
		studioCameraView.farClipPlane		  = 10000.f;
		studioCameraView.aspectRatio		  = (float)viewportSize.x / (float)viewportSize.y;
		g_pStudioRender->DrawScene( pStudioViewport, pActiveEcsMap->GetStudioScene(), studioCameraView );
	}
}

/*
==================
CBaseGame::CreatePlayer
==================
*/
CBasePlayer* CBaseGame::CreatePlayer() const
{
	return new CBasePlayer();
}

/*
==================
CBaseGameAppSystems::GetNum
==================
*/
uint32 CBaseGameAppSystems::GetNum() const
{
	return (uint32)appSystems.size();
}

/*
==================
CBaseGameAppSystems::GetModuleName
==================
*/
gameAppSystemInfo_t CBaseGameAppSystems::GetInfo( uint32 index ) const
{
	return appSystems[index];
}
