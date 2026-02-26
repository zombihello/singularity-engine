#include <EASTL/string.h>
#include <EASTL/unordered_set.h>

#include "appframework/iappsystemgroup.h"
#include "appframework/iwindowmgr.h"
#include "appframework/appframework.h"
#include "tier1/convar.h"
#include "tier0/icommandline.h"
#include "tier0/crashdump.h"
#include "inputsystem/iinputsystem.h"
#include "filesystem/ifilesystem.h"
#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/istudiorender.h"
#include "studiorender/istudio_viewport.h"
#include "materialsystem/imaterialsystem.h"
#include "resourcesystem/iresourcesystem.h"
#include "game/igame.h"
#include "utils/gameinfo/gameinfo.h"
#if ENABLE_LOGGING
	#include "tier0/consoleio.h"
#endif	// ENABLE_LOGGING

CConVar window_displayId( "window_displayId", "-1", "Window display id (-1: Not specified)", FCVAR_ARCHIVE );
CConVar window_width( "window_width", "1280", "Window width", FCVAR_ARCHIVE );
CConVar window_height( "window_height", "720", "Window height", FCVAR_ARCHIVE );
CConVar window_refreshRate( "window_refreshRate", "0", "Window refresh rate (0: Not specified)", FCVAR_ARCHIVE );
CConVar window_mode( "window_mode", "1", "Window mode (0: Hidden, 1: Windowed, 2: Borderless Fullscreen, 3: Exclusive Fullscreen)", FCVAR_ARCHIVE );

//-----------------------------------------------------------------------------
// IConVars overrider
//-----------------------------------------------------------------------------
class CConVarsOverrider : public IConVarsOverrider
{
public:
	virtual void OverrideFromCommandLine() override;
};
static CConVarsOverrider s_conVarsOverrider;

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
	CSingularityAppSystemGroup( const char* pDefaultGameDir, appInstanceHandle_t hInstance = NULL );

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

	static void OnWindowEvent( void* pUserData, const windowEvent_t& windowEvent );
	static void OnChangedMainWindow( void* pUserData, windowId_t newMainWindowId );

	bool											  bInFocus;
	appInstanceHandle_t								  hInstance;
	const char*										  pDefaultGameDir;
	CGameInfoDoc									  gameInfo;
	CGameViewportClient								  gameViewportClient;
	dllHandle_t										  gameDLLHandle;
	createInterfaceFn_t								  pGameFactory;
	TRefPtr<IStudioViewport>						  pStudioViewport;
	IWindowMgr::IOnWindowEvent::funcDelegate_t*		  pWindowEventDelegate;
	IWindowMgr::IOnChangedMainWindow::funcDelegate_t* pChangedMainWindowDelegate;
};

/*
==================
CConVarsOverrider::OverrideFromCommandLine
==================
*/
void CConVarsOverrider::OverrideFromCommandLine()
{
	ICommandLine* pCommandLine = CommandLine();

	// Check for windowed mode command line override
	if ( pCommandLine->HasParam( "windowed" ) || pCommandLine->HasParam( "window" ) )
	{
		window_mode.SetInt( WINDOW_MODE_WINDOWED );
	}
	// Check for fullscreen override
	else if ( pCommandLine->HasParam( "full" ) || pCommandLine->HasParam( "fullscreen" ) )
	{
		window_mode.SetInt( WINDOW_MODE_EXCLUSIVE_FULLSCREEN );
	}

	// Get width
	const char* pWidthParam = NULL;
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
	const char* pHeightParam = NULL;
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
CSingularityAppSystemGroup::CSingularityAppSystemGroup( const char* pDefaultGameDir, appInstanceHandle_t hInstance /* = NULL */ )
	: bInFocus( true )
	, hInstance( hInstance )
	, pDefaultGameDir( pDefaultGameDir )
	, gameDLLHandle( NULL )
	, pGameFactory( NULL )
	, pWindowEventDelegate( NULL )
	, pChangedMainWindowDelegate( NULL )
{
}

/*
==================
CSingularityAppSystemGroup::AddEngineSystems
==================
*/
bool CSingularityAppSystemGroup::AddEngineSystems()
{
	// Load engine application systems
	appSystemInfo_t appSystemInfos[] = {
		{ "inputsystem" DLL_EXT_STRING, INPUTSYSTEM_INTERFACE_VERSION },
		{ "studioapi_vk" DLL_EXT_STRING, STUDIOAPI_INTERFACE_VERSION },
		{ "studiorender" DLL_EXT_STRING, STUDIORENDER_INTERFACE_VERSION },
		{ "resourcesystem" DLL_EXT_STRING, RESOURCESYSTEM_INTERFACE_VERSION },
		{ "materialsystem" DLL_EXT_STRING, MATERIALSYSTEM_INTERFACE_VERSION },
		{ "", "" }	// Required to terminate the list
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
	IGameAppSystems* pGameAppSystems = (IGameAppSystems*)pGameFactory( GAME_APPSYSTEMS_INTERFACE_VERSION );
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
		g_pGame = (IGame*)pGameFactory( GAME_INTERFACE_VERSION );
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

		gameDLLHandle = NULL;
		pGameFactory  = NULL;
		g_pGame		  = NULL;
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
	const char* pGameDir = CommandLine()->HasParam( "game" ) ? CommandLine()->GetFirstValue( "game" ) : pDefaultGameDir;
	if ( !gameInfo.LoadFromFile( S_Sprintf( "//BASE_PATH/%s/gameinfo.txt", pGameDir ).c_str() ) )
	{
		Sys_Error( "Setup file 'gameinfo.txt' doesn't exist in subdirectory '%s'", pGameDir );
		return false;
	}

	// Initialize the file system for the game
	const eastl::vector<gameInfoSearchPath_t>& searchPaths = gameInfo.GetSearchPaths();
	for ( uint32 index = 0, count = (uint32)searchPaths.size(); index < count; ++index )
	{
		const gameInfoSearchPath_t& searchPath = searchPaths[index];
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

	g_pInputSystem	= (IInputSystem*)FindSystem( INPUTSYSTEM_INTERFACE_VERSION );
	g_pStudioRender = (IStudioRender*)FindSystem( STUDIORENDER_INTERFACE_VERSION );
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

	// Set true in cheats and developer cvars if we in debug configuration
#if DEBUG
	CConVarRef cheatsRef( "cheats" );
	CConVarRef developerRef( "developer" );
	if ( cheatsRef.IsValid() )
	{
		cheatsRef->SetBool( true );
	}
	if ( developerRef.IsValid() )
	{
		developerRef->SetBool( true );
	}
#endif	// DEBUG

	// Setup application information for the crash dump
	crashDumpAppInfo_t crashDumpAppInfo = {};
	crashDumpAppInfo.pAppName			= gameInfo.GetGame().c_str();
	crashDumpAppInfo.pAppVersion		= gameInfo.GetVersion().c_str();
	crashDumpAppInfo.pSupportEmail		= gameInfo.GetSupportEmail().c_str();
	crashDumpAppInfo.pSupportURL		= gameInfo.GetSupportURL().c_str();
	CrashDump_SetAppInfo( crashDumpAppInfo );

	// Read file configuration and override it from the command line
	g_pCvar->ReadConfigFile( "//game/cfg" );
	g_pCvar->OverrideConVarsFromCommandLine();

	// Create a hidden window for we can init render context and other things links with the one
	display_t		   display;
	windowCreateInfo_t windowCreateInfo = {};
	windowCreateInfo.pTitle				= gameInfo.GetGame().c_str();
	windowCreateInfo.width				= window_width.GetInt();
	windowCreateInfo.height				= window_height.GetInt();
	windowCreateInfo.refreshRate		= window_refreshRate.GetFloat();
	windowCreateInfo.mode				= WINDOW_MODE_HIDDEN;
	if ( g_pWindowMgr->GetPrimaryDisplay( display ) )
	{
		windowCreateInfo.pDisplay = &display;
	}

	// TODO BS yehor.pohuliaka -
	// 1. Refactor CApplication and separate it by some parts, because g_pWindowMgr must be initialized before use
	// 2. Set correct window size in mode Borderless/Exclusive Fullscreen
	// 3. The input system

	if ( !g_pWindowMgr->GetOrCreateMainWindow()->Create( windowCreateInfo ) )
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
#if 0
	// Attach the input system to the window
	g_pInputSystem->AttachToWindow( g_pWindowMgr );
#endif	// 0

	// Subscribe on window events
	pWindowEventDelegate	   = g_pWindowMgr->OnWindowEvent()->AddFunc( &CSingularityAppSystemGroup::OnWindowEvent, this );
	pChangedMainWindowDelegate = g_pWindowMgr->OnChangedMainWindow()->AddFunc( &CSingularityAppSystemGroup::OnChangedMainWindow, this );

	// Trigger the delegate to initialize a studio viewport
	OnChangedMainWindow( this, g_pWindowMgr->GetMainWindowId() );
	return true;
}

/*
==================
CSingularityAppSystemGroup::Main
==================
*/
int32 CSingularityAppSystemGroup::Main()
{
	// Main game loop
	PROFILE_INIT();
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
	if ( pWindowEventDelegate )
	{
		g_pWindowMgr->OnWindowEvent()->RemoveFunc( pWindowEventDelegate );
		pWindowEventDelegate = NULL;
	}
	if ( pChangedMainWindowDelegate )
	{
		g_pWindowMgr->OnChangedMainWindow()->RemoveFunc( pChangedMainWindowDelegate );
		pChangedMainWindowDelegate = NULL;
	}

	// Shutdown the viewport
	if ( pStudioViewport )
	{
		pStudioViewport->Shutdown();
		pStudioViewport = NULL;
	}

	// Destroy the main window
	g_pWindowMgr->DestroyWindow( g_pWindowMgr->GetMainWindowId() );

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
	// Remove only paths "game" and "gamebin" if gameinfo.txt not loaded
	if ( !gameInfo.IsLoaded() )
	{
		Warning( "Launcher: gameinfo.txt not loaded, will be remove only search paths \"game\" and \"gamebin\"" );
		g_pFileSystem->RemoveSearchPath( "game" );
		g_pFileSystem->RemoveSearchPath( "gamebin" );
		return;
	}

	// Otherwise we look for search paths in gameinfo.txt and
	// remove they from the file system
	eastl::unordered_set<eastl::string>		   pathIDSet;
	const eastl::vector<gameInfoSearchPath_t>& searchPaths = gameInfo.GetSearchPaths();
	for ( uint32 index = 0, count = (uint32)searchPaths.size(); index < count; ++index )
	{
		const gameInfoSearchPath_t& searchPath = searchPaths[index];
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
	CrashDump_SetAppInfo( crashDumpAppInfo_t{ NULL, NULL, NULL, NULL } );
}

/*
==================
CSingularityAppSystemGroup::OnWindowEvent
==================
*/
void CSingularityAppSystemGroup::OnWindowEvent( void* pUserData, const windowEvent_t& windowEvent )
{
	PROFILE_SCOPE();
	if ( windowEvent.windowId != g_pWindowMgr->GetMainWindowId() )
	{
		return;
	}

	CSingularityAppSystemGroup* pSingularityAppGroup = (CSingularityAppSystemGroup*)pUserData;
	switch ( windowEvent.type )
	{
		// Focus gained
	case WINDOW_EVENT_TYPE_RESTORED:
	case WINDOW_EVENT_TYPE_FOCUS_GAINED:
		pSingularityAppGroup->bInFocus = true;
		break;

		// Focus lost
	case WINDOW_EVENT_TYPE_MINIMIZED:
	case WINDOW_EVENT_TYPE_FOCUS_LOST:
		pSingularityAppGroup->bInFocus = false;
		break;

		// Resize window
	case WINDOW_EVENT_TYPE_RESIZE:
		if ( pSingularityAppGroup->pStudioViewport )
		{
			pSingularityAppGroup->pStudioViewport->Resize( windowEvent.resize.width, windowEvent.resize.height );
		}
		break;

		// Close window
	case WINDOW_EVENT_TYPE_CLOSE:
		Sys_RequestExit( false );
		break;
	}
}

/*
==================
CSingularityAppSystemGroup::OnChangedMainWindow
==================
*/
void CSingularityAppSystemGroup::OnChangedMainWindow( void* pUserData, windowId_t newMainWindowId )
{
	CConVarRef					r_vsyncRef( "r_vsync" );
	IWindow*					pMainWindow			 = g_pWindowMgr->GetWindow( newMainWindowId );
	ivec2_t						windowSize			 = pMainWindow->GetSize();
	CSingularityAppSystemGroup* pSingularityAppGroup = (CSingularityAppSystemGroup*)pUserData;
	if ( !pSingularityAppGroup->pStudioViewport )
	{
		pSingularityAppGroup->pStudioViewport = g_pStudioRender->CreateViewport();
	}
	Assert( pSingularityAppGroup->pStudioViewport );

	pSingularityAppGroup->pStudioViewport->SetViewportClient( &pSingularityAppGroup->gameViewportClient );
	pSingularityAppGroup->pStudioViewport->Init( pMainWindow->GetHandle(), windowSize.x, windowSize.y, r_vsyncRef.IsValid() ? r_vsyncRef->GetBool() : false );
	pMainWindow->SetMode( (windowMode_t)window_mode.GetInt() );
}

/*
==================
LauncherMain
==================
*/
extern "C" DLL_EXPORT uint32 LauncherMain( appInstanceHandle_t hInstance, const char* pDefaultGameDir, const char* pCommandLine )
{
	// Enable developer messages if we in debug configuration
#if DEBUG && ENABLE_LOGGING
	Logger()->SetGroupActivate( LOG_GROUP_DEVELOPER, true );
#endif	// DEBUG && ENABLE_LOGGING

	// Initialize the main thread and the command line
	Sys_InitMainThread();
	CommandLine()->Init( pCommandLine );

	// Attach a console for I/O if it need
#if ENABLE_LOGGING
	if ( CommandLine()->HasParam( "stdout" ) )
	{
		Sys_SetupConsoleIO();
		static CLogOutputStdOut s_logOutputStdOut;
		Logger()->AddOutput( &s_logOutputStdOut );
	}
#endif	// ENABLE_LOGGING

	// Disable ensures if it need
#if ENABLE_ENSURE
	if ( CommandLine()->HasParam( "noensure" ) )
	{
		Sys_SetEnsureAllow( false );
	}
#endif	// ENABLE_ENSURE

	// Run the application
	CSingularityAppSystemGroup singularitySystems( pDefaultGameDir, hInstance );
	CApplication			   application( &singularitySystems, "launcher" );
	return application.Run();
}
