#include <EASTL/unordered_set.h>

#include "tier0/icommandline.h"
#include "appframework/application.h"
#include "appframework/appsystemgroup_windowmgr.h"
#include "studiorender/istudio_viewport.h"
#include "studiorender/istudio_renderresource.h"
#include "resourcesystem/iresourcesystem.h"
#include "game/igame.h"
#include "launcher/convars.h"
#include "launcher/appsystemgroup_engine.h"
#include "launcher/appsystemgroup_game.h"
#include "utils/gameinfo/gameinfo.h"

//-----------------------------------------------------------------------------
// Game viewport client
//-----------------------------------------------------------------------------
class CGameViewportClient : public CBaseStudioViewportClient<IStudioViewportClient>
{
public:
};

//-----------------------------------------------------------------------------
// Game application
//-----------------------------------------------------------------------------
class CLauncherApp : public CApplication
{
	typedef CApplication BaseClass;

public:
	CLauncherApp( const char* pCommandLine, const char* pDefaultGameDir, appInstanceHandle_t hInstance = NULL );

protected:
	// CApplication interface
	virtual void  Init() override;
	virtual int32 Main() override;
	virtual void  Shutdown() override;

	virtual const appInfo_t&		  GetAppInfo() const override;
	virtual const crashDumpAppInfo_t& GetCrashDumpInfo() const override;

private:
	static void OnWindowEvent( void* pUserData, const windowEvent_t& windowEvent );
	static void OnChangedMainWindow( void* pUserData, windowId_t newMainWindowId );

	bool					 bInFocus;
	const char*				 pDefaultGameDir;
	CAppSystemGroupWindowMgr windowMgrSystemGroup;
	CAppSystemGroupEngine	 engineSystemGroup;
	CAppSystemGroupGame		 gameSystemGroup;
	CGameInfoDoc			 gameInfo;
	CGameViewportClient		 gameViewportClient;
	CRefPtr<IStudioViewport> pStudioViewport;

	IWindowMgr::IOnWindowEvent::handle_t	   onWindowEventHandle;
	IWindowMgr::IOnChangedMainWindow::handle_t onChangedMainWindowHandle;
};

/*
==================
CLauncherApp::CLauncherApp
==================
*/
CLauncherApp::CLauncherApp( const char* pCommandLine, const char* pDefaultGameDir, appInstanceHandle_t hInstance /*= NULL*/ )
	: CApplication( pCommandLine, hInstance )
	, bInFocus( false )
	, pDefaultGameDir( pDefaultGameDir )
	, onWindowEventHandle( INVALID_HANDLE )
	, onChangedMainWindowHandle( INVALID_HANDLE )
{
}

/*
==================
CLauncherApp::Init
==================
*/
void CLauncherApp::Init()
{
	BaseClass::Init();
	PROFILER_SCOPE_FUNC();

	// Load gameinfo.txt
	const char* pGameDir = CommandLine()->HasParam( "game" ) ? CommandLine()->GetFirstValue( "game" ) : pDefaultGameDir;
	if ( !gameInfo.LoadFromFile( S_Sprintf( "//base_path/%s/gameinfo.txt", pGameDir ).c_str() ) )
	{
		Sys_Error( "Setup file 'gameinfo.txt' doesn't exist in subdirectory '%s'", pGameDir );
		return;
	}

	// Initialize the file system for the game
	const eastl::vector<gameInfoSearchPath_t>& searchPaths = gameInfo.GetSearchPaths();
	for ( uint32 index = 0, count = (uint32)searchPaths.size(); index < count; ++index )
	{
		const gameInfoSearchPath_t& searchPath = searchPaths[index];
		g_pFileSystem->AddSearchPath( searchPath.path.c_str(), searchPath.id.c_str() );
	}

	// Setup application information for the crash dump
	crashDumpAppInfo_t crashDumpAppInfo = {};
	crashDumpAppInfo.pAppName			= gameInfo.GetGame().c_str();
	crashDumpAppInfo.pAppVersion		= gameInfo.GetVersion().c_str();
	crashDumpAppInfo.pSupportEmail		= gameInfo.GetSupportEmail().c_str();
	crashDumpAppInfo.pSupportURL		= gameInfo.GetSupportURL().c_str();
	CrashDumpHandler()->SetAppInfo( crashDumpAppInfo );

	// Connect WindowMgr, Engine and Game groups
	AddGroup( &windowMgrSystemGroup );
	AddGroup( &engineSystemGroup );
	AddGroup( &gameSystemGroup );
	windowMgrSystemGroup.ConnectSystems();
	engineSystemGroup.ConnectSystems();
	gameSystemGroup.ConnectSystems();

	// Read file configuration and override it from the command line
	g_pCvar->ReadConfigFile( "//game/cfg" );
	g_pCvar->OverrideConVarsFromCommandLine();

	// Get all systems
	g_pWindowMgr	  = (IWindowMgr*)FindSystem( WINDOWMGR_INTERFACE_VERSION );
	g_pInputSystem	  = (IInputSystem*)FindSystem( INPUTSYSTEM_INTERFACE_VERSION );
	g_pStudioRender	  = (IStudioRender*)FindSystem( STUDIORENDER_INTERFACE_VERSION );
	g_pResourceSystem = (IResourceSystem*)FindSystem( RESOURCESYSTEM_INTERFACE_VERSION );
	g_pGame			  = (IGame*)FindSystem( GAME_INTERFACE_VERSION );

	// Initialize WindowMgr group
	windowMgrSystemGroup.InitSystems();

	// Create a hidden window for we can init render context and other things links with the one
	windowCreateInfo_t windowCreateInfo = {};
	windowCreateInfo.pTitle				= gameInfo.GetGame().c_str();
	windowCreateInfo.mode				= WINDOW_MODE_HIDDEN;
	windowCreateInfo.displayHandle		= INVALID_DISPLAY_HANDLE;
	windowCreateInfo.width				= window_width.GetInt() >= 0 ? window_width.GetInt() : WINDOW_SIZE_FROM_DISPLAY;
	windowCreateInfo.height				= window_height.GetInt() >= 0 ? window_height.GetInt() : WINDOW_SIZE_FROM_DISPLAY;
	windowCreateInfo.refreshRate		= window_refreshRate.GetFloat();

	// Select the display on which the window should be opened
	display_t display;
	int32	  displayId = window_displayId.GetInt();
	if ( ( displayId >= 0 && g_pWindowMgr->GetDisplayById( displayId, display ) ) || g_pWindowMgr->GetPrimaryDisplay( display ) )
	{
		windowCreateInfo.displayHandle = display.handle;
	}

	IWindow* pMainWindow = g_pWindowMgr->GetOrCreateMainWindow();
	if ( !pMainWindow->Create( windowCreateInfo ) )
	{
		Sys_Error( "Failed to create a window" );
		return;
	}

	// Initialize Engine and Game groups
	engineSystemGroup.InitSystems();
	gameSystemGroup.InitSystems();

	// After initializing the engine and game groups, we can the change window mode to default
	pMainWindow->SetMode( (windowMode_t)window_mode.GetInt() );

	// Attach the input system to the window
	g_pInputSystem->AttachToWindow( pMainWindow->GetId() );

	// Create a studio viewport
	pStudioViewport = g_pStudioRender->CreateViewport();
	pStudioViewport->SetViewportClient( &gameViewportClient );

	// Trigger the delegate to initialize a studio viewport
	OnChangedMainWindow( this, g_pWindowMgr->GetMainWindowId() );

	// Subscribe on window events
	onWindowEventHandle		  = g_pWindowMgr->OnWindowEvent()->Subscribe( &CLauncherApp::OnWindowEvent, this );
	onChangedMainWindowHandle = g_pWindowMgr->OnChangedMainWindow()->Subscribe( &CLauncherApp::OnChangedMainWindow, this );
}

/*
==================
CLauncherApp::Main
==================
*/
int32 CLauncherApp::Main()
{
#if ENABLE_LOGGING
	IProfiler* pProfiler = Profiler();
#endif	// ENABLE_LOGGING
	while ( !Sys_IsRequestingExit() )
	{
		// Process window events
		PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_GAMELOGIC );
		g_pWindowMgr->ProcessEvents();

		// Skip the frame if it need
		bool bSkipFrame = !bInFocus;
#if ENABLE_PROFILING
		bSkipFrame &= !pProfiler->IsConnected();
#endif	// ENABLE_PROFILING
		if ( bSkipFrame )
		{
			continue;
		}

		// Update the profiler
#if ENABLE_PROFILING
		pProfiler->Update();
#endif	// ENABLE_PROFILING

		// Update the resource system and the game
		g_pResourceSystem->FrameUpdate();
		g_pGame->FrameUpdate();

		// Flush render commands and draw the frame
		Studio_FlushRenderCommands();
		pStudioViewport->DrawFrame();

		// Go to next profiler frame
#if ENABLE_PROFILING
		pProfiler->NextFrame();
#endif	// ENABLE_PROFILING
	}

	// Flush render thread commands before shutdown the application
	Studio_FlushRenderCommands();
	return 0;
}

/*
==================
CLauncherApp::Shutdown
==================
*/
void CLauncherApp::Shutdown()
{
	PROFILER_SCOPE_FUNC();

	// Detach the input system from the window
	g_pInputSystem->DetachFromWindow();

	// Describe from window events
	if ( onWindowEventHandle != INVALID_HANDLE )
	{
		g_pWindowMgr->OnWindowEvent()->Unsubscribe( onWindowEventHandle );
		onWindowEventHandle = INVALID_HANDLE;
	}
	if ( onChangedMainWindowHandle != INVALID_HANDLE )
	{
		g_pWindowMgr->OnChangedMainWindow()->Unsubscribe( onChangedMainWindowHandle );
		onChangedMainWindowHandle = INVALID_HANDLE;
	}

	// Destroy the viewport and the main window
	if ( pStudioViewport )
	{
		pStudioViewport->Destroy();
		pStudioViewport = NULL;
	}
	g_pWindowMgr->DestroyWindow( g_pWindowMgr->GetMainWindowId() );

	// Remove all our groups
	RemoveGroup( &gameSystemGroup );
	RemoveGroup( &engineSystemGroup );
	RemoveGroup( &windowMgrSystemGroup );

	// Remove only paths "game" and "gamebin" if gameinfo.txt not loaded
	if ( !gameInfo.IsLoaded() )
	{
		Warning( "Launcher: gameinfo.txt not loaded, will be remove only search paths \"game\" and \"gamebin\"" );
		g_pFileSystem->RemoveSearchPath( "game" );
		g_pFileSystem->RemoveSearchPath( "gamebin" );
		return;
	}

	// Otherwise we look for search paths in gameinfo.txt and remove they from the file system
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
	g_pGame			= NULL;
	BaseClass::Shutdown();
}

/*
==================
CLauncherApp::OnWindowEvent
==================
*/
void CLauncherApp::OnWindowEvent( void* pUserData, const windowEvent_t& windowEvent )
{
	PROFILER_SCOPE_FUNC();
	if ( windowEvent.windowId != g_pWindowMgr->GetMainWindowId() )
	{
		return;
	}

	CLauncherApp* pApp = (CLauncherApp*)pUserData;
	switch ( windowEvent.type )
	{
		// Focus gained
	case WINDOW_EVENT_TYPE_RESTORED:
	case WINDOW_EVENT_TYPE_FOCUS_GAINED:
		pApp->bInFocus = true;
		break;

		// Focus lost
	case WINDOW_EVENT_TYPE_MINIMIZED:
	case WINDOW_EVENT_TYPE_FOCUS_LOST:
		pApp->bInFocus = false;
		break;

		// Resize window
	case WINDOW_EVENT_TYPE_RESIZE:
		if ( pApp->pStudioViewport )
		{
			pApp->pStudioViewport->Resize( windowEvent.resize.width, windowEvent.resize.height );
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
CLauncherApp::OnChangedMainWindow
==================
*/
void CLauncherApp::OnChangedMainWindow( void* pUserData, windowId_t newMainWindowId )
{
	PROFILER_SCOPE_FUNC();
	CConVarRef	  r_vsyncRef( "r_vsync" );
	IWindow*	  pMainWindow = g_pWindowMgr->GetWindow( newMainWindowId );
	vector2i_t	  windowSize  = pMainWindow->GetSize();
	CLauncherApp* pApp		  = (CLauncherApp*)pUserData;

	Assert( pApp->pStudioViewport );
	pApp->pStudioViewport->Init( pMainWindow->GetHandle(), windowSize.x, windowSize.y, r_vsyncRef.IsValid() ? r_vsyncRef->GetBool() : false );
}

/*
==================
CLauncherApp::GetAppInfo
==================
*/
const appInfo_t& CLauncherApp::GetAppInfo() const
{
	static appInfo_t s_appInfo{ "launcher", APPLICATION_TYPE_WINDOW, FCVAR_NONE, &g_conVarsOverrider, NULL };
	return s_appInfo;
}

/*
==================
CLauncherApp::GetCrashDumpInfo
==================
*/
const crashDumpAppInfo_t& CLauncherApp::GetCrashDumpInfo() const
{
	static crashDumpAppInfo_t s_crashDumpAppInfo{ "Singularity Launcher", __DATE__ " " __TIME__, NULL, NULL };
	return s_crashDumpAppInfo;
}

/*
==================
LauncherMain
==================
*/
extern "C" DLL_EXPORT uint32 LauncherMain( appInstanceHandle_t hInstance, const char* pDefaultGameDir, const char* pCommandLine )
{
	return CLauncherApp( pCommandLine, pDefaultGameDir, hInstance ).Run();
}
