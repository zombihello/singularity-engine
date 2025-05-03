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
CGame::Init
==================
*/
bool CGame::Init( createInterfaceFn_t pFactory )
{
	// Connect StdLib
	if ( !ConnectStdLib( pFactory ) )
	{
		return false;
	}

	// Get the window manager
	g_pWindowMgr = ( IWindowMgr* )pFactory( WINDOWMGR_INTERFACE_VERSION );
	if ( !g_pWindowMgr )
	{
		return false;
	}

	// Get StudioRender
	g_pStudioRender = ( IStudioRender* )pFactory( STUDIORENDER_INTERFACE_VERSION );
	if ( !g_pStudioRender )
	{
		return false;
	}

	// Get the resource system
	g_pResourceSystem = ( IResourceSystem* )pFactory( RESOURCESYSTEM_INTERFACE_VERSION );
	if ( !g_pResourceSystem )
	{
		return false;
	}

	// Register cvars in the system
	ConVar_Register( FCVAR_GAMEDLL );

	// Register GameFramework modules
	ecsWorld.RegisterModule<ecsModuleCommon_t>();
	ecsWorld.RegisterModule<ecsModuleRender_t>();
	ecsWorld.RegisterModule<ecsModuleMovement_t>();
	ecsWorld.RegisterModule<ecsModuleCamera_t>();

	ecsWorld.SetResource( ecsResourceWindowMgr_t{ g_pWindowMgr } );
	ecsWorld.SetResource( ecsResourceStudioRender_t{ g_pStudioRender } );

	// Initialize all resource factories
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

	// Shutdown all resource factories
	ecsEntityDescFactory.Shutdown();

	// Reset the ECS world
	ecsWorld.Reset();

	// Unregister cvars and disconnect StdLib
	ConVar_Unregister();
	DisconnectStdLib();

	g_pStudioRender = NULL;
}

/*
==================
CGame::FrameUpdate
==================
*/
void CGame::FrameUpdate()
{
	ecsWorld.Update( 0.f );
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
	return ( uint32 )appSystems.size();
}

/*
==================
CGameAppSystems::GetModuleName
==================
*/
const achar* CGameAppSystems::GetModuleName( uint32 index ) const
{
	return appSystems[index].pModuleName;
}

/*
==================
CGameAppSystems::GetInterfaceName
==================
*/
const achar* CGameAppSystems::GetInterfaceName( uint32 index ) const
{
	return appSystems[index].pInterfaceName;
}