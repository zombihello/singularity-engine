#include "pch_engine.h"
#include "gameinfo/gameinfo.h"
#include "filesystem/ifilesystem.h"
#include "inputsystem/iinputsystem.h"
#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/istudiorender.h"
#include "engine/engine.h"
#include "engine/appsystemgroup_game.h"
#include "engine/version.h"
#include "engine/iengineapi.h"
#include "engine/engineapi.h"

createInterfaceFn_t		g_pAppSystemFactory = NULL;

//-----------------------------------------------------------------------------
// Main engine interface
//-----------------------------------------------------------------------------
class CEngineAPI : public CBaseAppSystem<IEngineAPI>
{
public:
	// IAppSystem interface
	// Here's where the app systems get to learn about each other
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void* QueryInterface( const achar* pInterfaceName ) override;

	// Initialize and shutdown
	virtual bool Init() override;
	virtual void Shutdown() override;

	// IEngineAPI interface
	// This function must be called before Init()
	virtual void SetStartupInfo( const startupInfo_t& info ) override;
	virtual int32 Run() override;

private:
	void Game_InitFileSystem();
	void Game_ShutdownFileSystem();

	startupInfo_t		startupInfo;
};

static CEngineAPI		s_EngineAPI;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CEngineAPI, IEngineAPI, ENGINEAPI_INTERFACE_VERSION, s_EngineAPI );


/*
==================
CEngineAPI::Connect
==================
*/
bool CEngineAPI::Connect( createInterfaceFn_t pFactory )
{
	// Connect StdLib
	if ( !ConnectStdLib( pFactory ) )
	{
		return false;
	}

	// Print some info about engine version and build number
	Msg( "Engine: Singularity Engine " ENGINE_VERSION_STRING " build %i (" __DATE__ " " __TIME__ ")", Engine_BuildNumber() );

	// Get the window manager
	g_pWindowMgr = ( IWindowMgr* )pFactory( WINDOWMGR_INTERFACE_VERSION );
	if ( !g_pWindowMgr )
	{
		return false;
	}

	// Get the input system
	g_pInputSystem = ( IInputSystem* )pFactory( INPUTSYSTEM_INTERFACE_VERSION );
	if ( !g_pInputSystem )
	{
		return false;
	}

	// Get the Studio API
	g_pStudioAPI = ( IStudioAPI* )pFactory( STUDIOAPI_INTERFACE_VERSION );
	if ( !g_pStudioAPI )
	{
		return false;
	}

	// Get the studiorender
	g_pStudioRender = ( IStudioRender* )pFactory( STUDIORENDER_INTERFACE_VERSION );
	if ( !g_pStudioRender )
	{
		return false;
	}

	return true;
}

/*
==================
CEngineAPI::Disconnect
==================
*/
void CEngineAPI::Disconnect()
{
	g_pWindowMgr	= NULL;
	g_pInputSystem	= NULL;
	DisconnectStdLib();
}

/*
==================
CEngineAPI::QueryInterface
==================
*/
void* CEngineAPI::QueryInterface( const achar* pInterfaceName )
{
	createInterfaceFn_t		pFactory = Sys_GetFactoryThis();
	return pFactory( pInterfaceName );
}

/*
==================
CEngineAPI::Init
==================
*/
bool CEngineAPI::Init()
{
	// Register cvars
	ConVar_Register();
	return true;
}

/*
==================
CEngineAPI::Shutdown
==================
*/
void CEngineAPI::Shutdown()
{
	// Unregister cvars
	ConVar_Unregister();
}

/*
==================
CEngineAPI::SetStartupInfo
==================
*/
void CEngineAPI::SetStartupInfo( const startupInfo_t& info )
{
	// Copy off all the startup info
	startupInfo = info;
}

/*
==================
CEngineAPI::Run
==================
*/
int32 CEngineAPI::Run()
{
	//
	// NOTE: Systems set up here should depend on the game 
	// Systems which are game-independent should be set up in the launcher or Init()
	//

	// Load gameinfo.txt
	if ( !g_Engine.LoadGameInfo( S_Sprintf( "//BASE_PATH/%s/gameinfo.txt", startupInfo.pGame ).c_str() ) )
	{
		Sys_Error( "Engine: Setup file 'gameinfo.txt' doesn't exist in subdirectory '%s'", startupInfo.pGame );
		return 1;
	}

	// Initialize file system for the game
	Game_InitFileSystem();

	// Run game app system group
	CGameAppSystemGroup		gameAppSystemGroup( startupInfo.pParentAppSystemGroup );
	g_pAppSystemFactory	= gameAppSystemGroup.GetFactory();
	int32 result		= gameAppSystemGroup.Run();
	g_pAppSystemFactory = NULL;

	// We are done, shutdown the file system for the game
	Game_ShutdownFileSystem();

	// Unload gameinfo.txt and exit
	g_Engine.UnloadGameInfo();
	return result;
}

/*
==================
CEngineAPI::Game_InitFileSystem
==================
*/
void CEngineAPI::Game_InitFileSystem()
{
	// Setup search paths from gameinfo
	const std::vector<gameInfoSearchPath_t>&		searchPaths = g_Engine.GetGameInfo().GetSearchPaths();
	for ( uint32 index = 0, count = ( uint32 )searchPaths.size(); index < count; ++index )
	{
		const gameInfoSearchPath_t&		searchPath = searchPaths[index];
		g_pFileSystem->AddSearchPath( searchPath.path.c_str(), searchPath.id.c_str() );
	}
}

/*
==================
CEngineAPI::Game_ShutdownFileSystem
==================
*/
void CEngineAPI::Game_ShutdownFileSystem()
{
	// Remove only paths "GAME" and "GAMEBIN" if gameinfo.txt not loaded
	const CGameInfoDoc&		gameinfo = g_Engine.GetGameInfo();
	if ( !gameinfo.IsLoaded() )
	{
		Warning( "Engine: gameinfo.txt not laoded, will be remove only search paths \"GAME\" and \"GAMEBIN\"" );
		g_pFileSystem->RemoveSearchPath( "GAME" );
		g_pFileSystem->RemoveSearchPath( "GAMEBIN" );
		return;
	}

	// Otherwise we look for search paths in gameinfo.txt and
	// remove they from the file system
	std::unordered_set<std::string>				pathIDSet;
	const std::vector<gameInfoSearchPath_t>&	searchPaths = gameinfo.GetSearchPaths();
	for ( uint32 index = 0, count = ( uint32 )searchPaths.size(); index < count; ++index )
	{
		const gameInfoSearchPath_t&				searchPath = searchPaths[index];
		pathIDSet.insert( searchPath.id );
	}

	for ( auto it = pathIDSet.begin(), itEnd = pathIDSet.end(); it != itEnd; ++it )
	{
		g_pFileSystem->RemoveSearchPath( ( *it ).c_str() );
	}
}