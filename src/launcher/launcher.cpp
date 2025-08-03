#include <string>
#include <unordered_set>

#include "appframework/iappsystemgroup.h"
#include "appframework/iwindowmgr.h"
#include "appframework/appframework.h"
#include "stdlib/convar.h"
#include "core/icommandline.h"
#include "core/crashdump.h"
#include "inputsystem/iinputsystem.h"
#include "filesystem/ifilesystem.h"
#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/istudiorender.h"
#include "studiorender/istudio_viewport.h"
#include "materialsystem/imaterialsystem.h"
#include "resourcesystem/iresourcesystem.h"
#include "gameframework/igame.h"
#include "gameinfo/gameinfo.h"

CConVar		window_width( "window_width", "1280", "Window width", FCVAR_ARCHIVE );
CConVar		window_height( "window_height", "720", "Window height", FCVAR_ARCHIVE );
CConVar		fullscreen( "fullscreen", "0", "Is need open the window in fullscreen mode", FCVAR_ARCHIVE );


#if ENABLE_LOGGING
#include <filesystem>
static FILE*		s_pLogFile = NULL;		// Launcher's log file

/*
==================
Launcher_LogOutput
==================
*/
static void Launcher_LogOutput( const achar* pMsg )
{
	// Print message to OS console
	if ( Sys_IsInitedConsoleIO() )
	{
		printf( pMsg );
	}

	// Print message to the log file
	if ( s_pLogFile )
	{
		fprintf( s_pLogFile, pMsg );
		fflush( s_pLogFile );
	}

	// Print message to debug output
	if ( Sys_IsDebuggerPresent() )
	{
		Sys_DebugMessage( pMsg );
	}

	// Print message to the engine's console
	if ( g_pCvar )
	{
		g_pCvar->ConsolePrintf( pMsg );
	}
}
#endif // ENABLE_LOGGING

/*
==================
Launcher_InitLogOutput
==================
*/
void Launcher_InitLogOutput()
{
#if ENABLE_LOGGING
	// Create directory for logs
	std::filesystem::create_directory( "../../logs" );
	
	// Open a log file and add it to the crash dump system
	s_pLogFile = fopen( "../../logs/launcher.log", "w" );
	if ( s_pLogFile )
	{
		CrashDump_AddLogFile( "../../logs/launcher.log" );
	}

	// Set our log output function
	Sys_SetLogOutputFunc( Launcher_LogOutput );
#endif // ENABLE_LOGGING
}


//-----------------------------------------------------------------------------
// IConVars overrider
//-----------------------------------------------------------------------------
class CConVarsOverrider : public IConVarsOverrider
{
public:
	virtual void OverrideFromCommandLine() override;
};
static CConVarsOverrider	s_conVarsOverrider;


//-----------------------------------------------------------------------------
// Game viewport client
//-----------------------------------------------------------------------------
class CGameViewportClient : public TBaseStudioViewportClient<IStudioViewportClient>
{
public:
};


//-----------------------------------------------------------------------------
// Singularity Engine app system group
//-----------------------------------------------------------------------------
class CSingularityAppSystemGroup : public CDefaultAppSystemGroup<CAppSystemGroup>
{
public:
	CSingularityAppSystemGroup( const achar* pDefaultGameDir, appInstanceHandle_t hInstance = NULL );

	// IAppSystemGroup interface
	// An installed application creation function, you should tell the group
	// the DLLs and the singleton interfaces you want to instantiate
	// Return FALSE if there's any problems and the app will abort
	virtual bool Create() override;

	// Allow the application to do some work after AppSystems are connected but
	// they aren't all Initialized
	// Return FALSE if there's any problems and the app will abort
	virtual bool PreInit() override;

	// Allow the application to do some work after AppSystems are initialized but
	// before main is run
	// Return FALSE if there's any problems and the app will abort
	virtual bool PostInit() override;

	// Main loop implemented by the application
	// Return exit code. If all ok returns zero
	virtual int32 Main() override;

	// Allow the application to do some work before all AppSystems are shut down
	virtual void PreShutdown() override;

	// Allow the application to do some work after all AppSystems are shut down
	virtual void PostShutdown() override;

	// Call an installed application destroy function, occurring after all modules are unloaded
	virtual void Destroy() override;

private:
	// Add engine and game systems
	bool AddEngineSystems();
	bool AddGameSystems();

	// Load and unload game dll
	bool GameDLL_Load();
	void GameDLL_Unload();

	static void OnProcessWindowEvent( void* pUserData, const windowEvent_t& windowEvent );

	bool									bInFocus;
	appInstanceHandle_t						hInstance;
	const achar*							pDefaultGameDir;
	CGameInfoDoc							gameInfo;
	CGameViewportClient						gameViewportClient;
	dllHandle_t								gameDLLHandle;
	createInterfaceFn_t						pGameFactory;
	TRefPtr<IStudioViewport>				pStudioViewport;
	IOnProcessWindowEvent::funcDelegate_t*	pProcessWindowEventDelegate;
};


/*
==================
CConVarsOverrider::OverrideFromCommandLine
==================
*/
void CConVarsOverrider::OverrideFromCommandLine()
{
	ICommandLine*	pCommandLine = CommandLine();

	// Check for windowed mode command line override
	if ( pCommandLine->HasParam( "windowed" ) || pCommandLine->HasParam( "window" ) )
	{
		fullscreen.SetInt( 0 );
	}
	// Check for fullscreen override
	else if ( pCommandLine->HasParam( "full" ) || pCommandLine->HasParam( "fullscreen" ) )
	{
		fullscreen.SetInt( 1 );
	}

	// Get width
	const achar*	pWidthParam = NULL;
	if ( pCommandLine->HasParam( "width" ) )
	{
		pWidthParam = "width";
	}
	else if ( pCommandLine->HasParam( "w" ) )
	{
		pWidthParam = "w";
	}

	// Override width
	if ( pWidthParam )
	{
		window_width.SetString( pCommandLine->GetFirstValue( pWidthParam ) );
	}

	// Get height
	const achar*	pHeightParam = NULL;
	if ( pCommandLine->HasParam( "height" ) )
	{
		pHeightParam = "height";
	}
	else if ( pCommandLine->HasParam( "h" ) )
	{
		pHeightParam = "h";
	}

	// Override height
	if ( pHeightParam )
	{
		window_height.SetString( pCommandLine->GetFirstValue( pHeightParam ) );
	}
}


/*
==================
CSingularityAppSystemGroup::CSingularityAppSystemGroup
==================
*/
CSingularityAppSystemGroup::CSingularityAppSystemGroup( const achar* pDefaultGameDir, appInstanceHandle_t hInstance /* = NULL */ )
	: bInFocus( true )
	, hInstance( hInstance )
	, pDefaultGameDir( pDefaultGameDir )
	, gameDLLHandle( NULL )
	, pGameFactory( NULL )
	, pProcessWindowEventDelegate( NULL )
{}

/*
==================
CSingularityAppSystemGroup::AddEngineSystems
==================
*/
bool CSingularityAppSystemGroup::AddEngineSystems()
{
	// Load engine application systems
	appSystemInfo_t		appSystemInfos[] =
	{
		{ "inputsystem"		DLL_EXT_STRING,		INPUTSYSTEM_INTERFACE_VERSION		},
		{ "studioapi_vk"	DLL_EXT_STRING,		STUDIOAPI_INTERFACE_VERSION			},
		{ "studiorender"	DLL_EXT_STRING,		STUDIORENDER_INTERFACE_VERSION		},
		{ "resourcesystem"	DLL_EXT_STRING,		RESOURCESYSTEM_INTERFACE_VERSION	},
		{ "materialsystem"	DLL_EXT_STRING,		MATERIALSYSTEM_INTERFACE_VERSION	},
		{ "", "" }																		// Required to terminate the list
	};

	// Add the window manager to app systems list
	g_pWindowMgr = CreateWindowMgr();
	AddSystem( g_pWindowMgr, WINDOWMGR_INTERFACE_VERSION );

	// Add all systems from the array
	return AddSystems( appSystemInfos );
}

/*
==================
CSingularityAppSystemGroup::AddGameSystems
==================
*/
bool CSingularityAppSystemGroup::AddGameSystems()
{
	// Add to the group the game's app systems if IGameAppSystems is exist
	Assert( pGameFactory );
	IGameAppSystems*	pGameAppSystems = ( IGameAppSystems* )pGameFactory( GAME_APPSYSTEMS_INTERFACE_VERSION );
	if ( pGameAppSystems )
	{
		std::vector<appSystemInfo_t>	appSystems_before;
		std::vector<appSystemInfo_t>	appSystems_after;
		for ( uint32 index = 0, count = pGameAppSystems->GetNum(); index < count; ++index )
		{
			gameAppSystemInfo_t			gameAppSystem = pGameAppSystems->GetInfo( index );
			appSystemInfo_t				appSystem;
			appSystem.pModuleName		= gameAppSystem.pModuleName;
			appSystem.pInterfaceName	= gameAppSystem.pInterfaceName;

			switch ( gameAppSystem.order )
			{
			case GAME_APPSYSTEM_ORDER_BEFORE_GAME:		appSystems_before.emplace_back( appSystem );	break;
			case GAME_APPSYSTEM_ORDER_AFTER_GAME:		appSystems_after.emplace_back( appSystem );		break;
			default:
				AssertMsg( false, "Unknown game app system order 0x%X", gameAppSystem.order );
				return false;
			}
		}

		// Terminate arrays
		appSystemInfo_t		nullTerminateInfo = { "", "" };
		appSystems_before.emplace_back( nullTerminateInfo );
		appSystems_after.emplace_back( nullTerminateInfo );

		// Add all the game's app systems into our group
		if ( !AddSystems( appSystems_before.data() ) )
		{
			return false;
		}
		AddSystem( g_pGame, GAME_INTERFACE_VERSION );
		if ( !AddSystems( appSystems_after.data() ) )
		{
			return false;
		}
	}

	return true;
}

/*
==================
CSingularityAppSystemGroup::GameDLL_Load
==================
*/
bool CSingularityAppSystemGroup::GameDLL_Load()
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

	// Load interface factory and any interfaces exported by the game dll
	pGameFactory = Sys_GetFactory( gameDLLHandle );
	if ( pGameFactory )
	{
		// Get game interface from dll
		g_pGame = ( IGame* )pGameFactory( GAME_INTERFACE_VERSION );
		if ( !g_pGame )
		{
			Warning( "Launcher: Could not get " GAME_INTERFACE_VERSION " from '//GAMEBIN/game" DLL_EXT_STRING "'" );
			GameDLL_Unload();
			return false;
		}
	}
	else
	{
		Warning( "Launcher: Could not find " CREATEINTERFACE_FUNCNAME " in '//GAMEBIN/game" DLL_EXT_STRING "'" );
		GameDLL_Unload();
		return false;
	}

	// We are done, all ok
	Msg( "Launcher: game" DLL_EXT_STRING " loaded for %s", g_pGame->GetGameDescription() );
	return true;
}

/*
==================
CSingularityAppSystemGroup::GameDLL_Unload
==================
*/
void CSingularityAppSystemGroup::GameDLL_Unload()
{
	if ( gameDLLHandle )
	{
		Msg( "Launcher: game" DLL_EXT_STRING " unloaded" );
		g_pFileSystem->UnloadModule( gameDLLHandle );

		gameDLLHandle	= NULL;
		pGameFactory	= NULL;
		g_pGame			= NULL;
	}
}

/*
==================
CSingularityAppSystemGroup::Create
==================
*/
bool CSingularityAppSystemGroup::Create()
{
	// Print some info about user
	Msg( "Launcher: User %s//%s", Sys_GetComputerName(), Sys_GetUserName() );

	// Load gameinfo.txt
	const achar*		pGameDir = CommandLine()->HasParam( "game" ) ? CommandLine()->GetFirstValue( "game" ) : pDefaultGameDir;
	if ( !gameInfo.LoadFromFile( S_Sprintf( "//BASE_PATH/%s/gameinfo.txt", pGameDir ).c_str() ) )
	{
		Sys_Error( "Setup file 'gameinfo.txt' doesn't exist in subdirectory '%s'", pGameDir );
		return false;
	}

	// Initialize the file system for the game
	const std::vector<gameInfoSearchPath_t>&	searchPaths = gameInfo.GetSearchPaths();
	for ( uint32 index = 0, count = ( uint32 )searchPaths.size(); index < count; ++index )
	{
		const gameInfoSearchPath_t&		searchPath = searchPaths[index];
		g_pFileSystem->AddSearchPath( searchPath.path.c_str(), searchPath.id.c_str() );
	}

	// Add engine application systems
	if ( !AddEngineSystems() )
	{
		// TODO BS yehor.pohuliaka - Add error message into appframework
		Sys_Error( "Failed to load engine systems" );
		return false;
	}

	// Load game dll
	if ( !GameDLL_Load() )
	{
		Sys_Error( "Failed to load game" DLL_EXT_STRING );
		return false;
	}

	// Add game application systems
	if ( !AddGameSystems() )
	{
		// TODO BS yehor.pohuliaka - Add error message into appframework
		Sys_Error( "Failed to load game systems" );
		return false;
	}

	g_pInputSystem	= ( IInputSystem* )FindSystem( INPUTSYSTEM_INTERFACE_VERSION );
	g_pStudioRender = ( IStudioRender* )FindSystem( STUDIORENDER_INTERFACE_VERSION );
	return true;
}

/*
==================
CSingularityAppSystemGroup::PreInit
==================
*/
bool CSingularityAppSystemGroup::PreInit()
{
	// Register cvars
	ConVar_Register( FCVAR_NONE, &s_conVarsOverrider );

	// Setup application information for the crash dump
	crashDumpAppInfo_t				crashDumpAppInfo = {};
	crashDumpAppInfo.pAppName		= gameInfo.GetGame().c_str();
	crashDumpAppInfo.pAppVersion	= gameInfo.GetVersion().c_str();
	crashDumpAppInfo.pSupportEmail	= gameInfo.GetSupportEmail().c_str();
	crashDumpAppInfo.pSupportURL	= gameInfo.GetSupportURL().c_str();
	CrashDump_SetAppInfo( crashDumpAppInfo );

	// Read file configuration and override it from the command line
	g_pCvar->ReadConfigFile( "//GAME/cfg" );
	g_pCvar->OverrideConVarsFromCommandLine();

	// Create a hidden window for we can init render context and other things links with the one
	if ( !g_pWindowMgr->Create( gameInfo.GetGame().c_str(), window_width.GetInt(), window_height.GetInt(), WINDOW_STYLE_DEFAULT | WINDOW_STYLE_HIDDEN ) )
	{
		Sys_Error( "Failed to create a window" );
		return false;
	}

	return true;
}

/*
==================
CSingularityAppSystemGroup::PostInit
==================
*/
bool CSingularityAppSystemGroup::PostInit()
{
	// Attach the input system to the window
	g_pInputSystem->AttachToWindow( g_pWindowMgr );

	// Subscribe on window events
	pProcessWindowEventDelegate = g_pWindowMgr->OnProcessWindowEvent()->AddFunc( &CSingularityAppSystemGroup::OnProcessWindowEvent, this );

	// Create and initialize a viewport
	pStudioViewport = g_pStudioRender->CreateViewport();
	if ( !pStudioViewport )
	{
		return false;
	}

	uint32		windowWidth = 0;
	uint32		windowHeight = 0;
	CConVarRef	r_vsyncRef( "r_vsync" );
	g_pWindowMgr->GetSize( windowWidth, windowHeight );
	g_pWindowMgr->SetFullscreen( fullscreen.GetBool() );
	pStudioViewport->SetViewportClient( &gameViewportClient );
	pStudioViewport->Init( g_pWindowMgr->GetHandle(), windowWidth, windowHeight, r_vsyncRef.IsValid() ? r_vsyncRef->GetBool() : false );
	g_pWindowMgr->ShowWindow();
	return true;
}

/*
==================
CSingularityAppSystemGroup::Main
==================
*/
int32 CSingularityAppSystemGroup::Main()
{
	// Initialize the profiler
	PROFILE_INIT();

	// Main game loop
	while ( !Sys_IsRequestingExit() )
	{
		PROFILE_FRAME( "Main Thread" );
		PROFILE_SCOPE( PROFILE_SCOPE_GROUP_GAMELOGIC );

		// Process window events
		g_pWindowMgr->ProcessEvents();
		if ( !bInFocus && !PROFILE_IS_RECORD() )
		{
			continue;
		}

		// Update a game logic frame and draw it
		g_pGame->FrameUpdate();
		pStudioViewport->DrawFrame();
	}

	// Flush render thread commands before shutdown the application
	Studio_FlushRenderCommands();

	// Shutdown the profiler
	PROFILE_SHUTDOWN();
	return 0;
}

/*
==================
CSingularityAppSystemGroup::PreShutdown
==================
*/
void CSingularityAppSystemGroup::PreShutdown()
{
	// Detach the input system from the window
	g_pInputSystem->DetachFromWindow();

	// Describe from window events
	if ( pProcessWindowEventDelegate )
	{
		g_pWindowMgr->OnProcessWindowEvent()->RemoveFunc( pProcessWindowEventDelegate );
		pProcessWindowEventDelegate = NULL;
	}

	// Shutdown the viewport
	if ( pStudioViewport )
	{
		pStudioViewport->Shutdown();
		pStudioViewport = NULL;
	}

	// Close the window
	g_pWindowMgr->Close();

	// Unregister cvars
	ConVar_Unregister();
}

/*
==================
CSingularityAppSystemGroup::PostShutdown
==================
*/
void CSingularityAppSystemGroup::PostShutdown()
{
	// Remove only paths "GAME" and "GAMEBIN" if gameinfo.txt not loaded
	if ( !gameInfo.IsLoaded() )
	{
		Warning( "Launcher: gameinfo.txt not laoded, will be remove only search paths \"GAME\" and \"GAMEBIN\"" );
		g_pFileSystem->RemoveSearchPath( "GAME" );
		g_pFileSystem->RemoveSearchPath( "GAMEBIN" );
		return;
	}

	// Otherwise we look for search paths in gameinfo.txt and
	// remove they from the file system
	std::unordered_set<std::string>				pathIDSet;
	const std::vector<gameInfoSearchPath_t>&	searchPaths = gameInfo.GetSearchPaths();
	for ( uint32 index = 0, count = ( uint32 )searchPaths.size(); index < count; ++index )
	{
		const gameInfoSearchPath_t&				searchPath = searchPaths[index];
		pathIDSet.insert( searchPath.id );
	}

	for ( auto it = pathIDSet.begin(), itEnd = pathIDSet.end(); it != itEnd; ++it )
	{
		g_pFileSystem->RemoveSearchPath( ( *it ).c_str() );
	}

	g_pWindowMgr	= NULL;
	g_pInputSystem	= NULL;
	g_pStudioRender = NULL;
}

/*
==================
CSingularityAppSystemGroup::Destroy
==================
*/
void CSingularityAppSystemGroup::Destroy()
{
	// Unload the game dll
	GameDLL_Unload();
	CrashDump_SetAppInfo( crashDumpAppInfo_t{NULL, NULL, NULL, NULL} );
}

/*
==================
CSingularityAppSystemGroup::OnProcessWindowEvent
==================
*/
void CSingularityAppSystemGroup::OnProcessWindowEvent( void* pUserData, const windowEvent_t& windowEvent )
{
	PROFILE_SCOPE();
	CSingularityAppSystemGroup*		pSingularityAppGroup = ( CSingularityAppSystemGroup* )pUserData;
	switch ( windowEvent.type )
	{
		// Focus gained
	case windowEvent_t::EVENT_WINDOW_RESTORED:
	case windowEvent_t::EVENT_WINDOW_FOCUS_GAINED:
		if ( g_pWindowMgr->GetID() == windowEvent.windowId )
		{
			pSingularityAppGroup->bInFocus = true;
		}
		break;

		// Focus lost
	case windowEvent_t::EVENT_WINDOW_MINIMIZED:
	case windowEvent_t::EVENT_WINDOW_FOCUS_LOST:
		if ( g_pWindowMgr->GetID() == windowEvent.windowId )
		{
			pSingularityAppGroup->bInFocus = false;
		}
		break;

		// Resize window
	case windowEvent_t::EVENT_WINDOW_RESIZE:
		if ( g_pWindowMgr->GetID() == windowEvent.windowId && pSingularityAppGroup->pStudioViewport )
		{
			pSingularityAppGroup->pStudioViewport->Resize( windowEvent.events.windowResize.width, windowEvent.events.windowResize.height );
		}
		break;

		// Close window
	case windowEvent_t::EVENT_WINDOW_CLOSE:
		if ( g_pWindowMgr->GetID() == windowEvent.windowId )
		{
			Sys_RequestExit( false );
		}
		break;
	}
}


/*
==================
LauncherMain
==================
*/
extern "C" DLL_EXPORT uint32 LauncherMain( appInstanceHandle_t hInstance, const achar* pDefaultGameDir, const achar* pCommandLine )
{
	// Initialize the main thread
	Sys_InitMainThread();

	// Init of launcher's log output and command line
	Launcher_InitLogOutput();
	CommandLine()->Init( pCommandLine );

	// Initialize OS console if it need
	if ( CommandLine()->HasParam( "console" ) )
	{
		Sys_SetupConsoleIO();
	}

	// Disable ensures if it need
#if ENABLE_ENSURE
	if ( CommandLine()->HasParam( "noensure" ) )
	{
		Sys_SetEnsureAllow( false );
	}
#endif // ENABLE_ENSURE

	// Run application
	CSingularityAppSystemGroup	singularitySystems( pDefaultGameDir, hInstance );
	CApplication				application( &singularitySystems );
	return application.Run();
}