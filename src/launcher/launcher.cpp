#include "appframework/iappsystemgroup.h"
#include "appframework/iwindowmgr.h"
#include "stdlib/convar.h"
#include "core/icommandline.h"
#include "core/crashdump.h"
#include "engine/iengineapi.h"
#include "inputsystem/iinputsystem.h"
#include "filesystem/ifilesystem.h"
#include "studiorender/istudiorender.h"
#include "materialsystem/imaterialsystem.h"
#include "resourcesystem/iresourcesystem.h"

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


/**
 * @ingroup launcher
 * @brief Singularity Engine application
 */
class CSingularityEngineApp : public CDefaultAppSystemGroup<CAppSystemGroup>
{
public:
	/**
	 * @brief Constructor
	 * @param pGameDir		Game directory
	 * @param pHInstance	Application instance handle
	 */
	CSingularityEngineApp( const achar* pGameDir, appInstanceHandle_t hInstance = NULL );

	/**
	 * @brief An installed application creation function, you should tell the group
	 * the DLLs and the singleton interfaces you want to instantiate.
	 *
	 * @return Return FALSE if there's any problems and the app will abort
	 */
	virtual bool Create() override;

	/**
	 * @brief Allow the application to do some work after AppSystems are connected
	 *
	 * Allow the application to do some work after AppSystems are connected but
	 * they aren't all Initialized
	 *
	 * @return Return FALSE if there's any problems and the app will abort
	 */
	virtual bool PreInit() override;

	/**
	 * @brief Main loop implemented by the application
	 * @return Return exit code. If all ok returns zero
	 */
	virtual int32 Main() override;

	/**
	 * @brief Allow the application to do some work after all AppSystems are shut down
	 */
	virtual void PostShutdown() override;

private:
	appInstanceHandle_t		hInstance;			/**< Application instance handle */
	const achar*			pGameDir;			/**< Game directory */
	IWindowMgr*				pWindowMgr;			/**< Window manager */
	IEngineAPI*				pEngineAPI;			/**< Engine API */
};


/*
==================
CSingularityEngineApp::CSingularityEngineApp
==================
*/
CSingularityEngineApp::CSingularityEngineApp( const achar* pGameDir, appInstanceHandle_t hInstance /* = NULL */ )
	: hInstance( hInstance )
	, pGameDir( pGameDir )
	, pWindowMgr( NULL )
	, pEngineAPI( NULL )
{}

/*
==================
CSingularityEngineApp::Create
==================
*/
bool CSingularityEngineApp::Create()
{
	// Print some info about user
	Msg( "User %s//%s", Sys_GetComputerName(), Sys_GetUserName() );

	// Load application systems
	appSystemInfo_t		appSystemInfos[] =
	{
		{ "engine"			DLL_EXT_STRING,		CVAR_QUERY_INTERFACE_VERSION		},	// This one must be first
		{ "filesystem"		DLL_EXT_STRING,		FILESYSTEM_INTERFACE_VERSION		},
		{ "engine"			DLL_EXT_STRING,		CVAR_INTERFACE_VERSION				},
		{ "inputsystem"		DLL_EXT_STRING,		INPUTSYSTEM_INTERFACE_VERSION		},
		{ "studiorender"	DLL_EXT_STRING,		STUDIORENDER_INTERFACE_VERSION		},
		{ "resourcesystem"	DLL_EXT_STRING,		RESOURCESYSTEM_INTERFACE_VERSION	},
		{ "materialsystem"	DLL_EXT_STRING,		MATERIALSYSTEM_INTERFACE_VERSION	},
		{ "engine"			DLL_EXT_STRING,		ENGINEAPI_INTERFACE_VERSION			},
		{ "", "" }																		// Required to terminate the list
	};

	// Add the window manager to app systems list
	pWindowMgr = CreateWindowMgr();
	AddSystem( pWindowMgr, WINDOWMGR_INTERFACE_VERSION );

	// Add all systems from array
	if ( !AddSystems( appSystemInfos ) )
	{
		return false;
	}

	// Load up the appropriate Studio API DLL
	// This has to be done before connection 
	IStudioRender*		pStudioRender = ( IStudioRender* )FindSystem( STUDIORENDER_INTERFACE_VERSION );
	pStudioRender->SetStudioAPI( "studioapi_vk" DLL_EXT_STRING );

	// Get engine API
	pEngineAPI = ( IEngineAPI* )FindSystem( ENGINEAPI_INTERFACE_VERSION );
	return true;
}

/*
==================
CSingularityEngineApp::PreInit
==================
*/
bool CSingularityEngineApp::PreInit()
{
	// Connect StdLib and register cvars
	if ( !ConnectStdLib( GetFactory() ) )
	{
		return false;
	}
	ConVar_Register();

	// Create a hidden window for we can init render context and other things links with the one
	if ( !pWindowMgr->Create( "Singularity Engine", 1, 1, WINDOW_STYLE_DEFAULT | WINDOW_STYLE_HIDDEN ) )
	{
		Sys_Error( "Launcher: Failed to create window" );
		return false;
	}

	// Set startup info
	startupInfo_t						startupInfo;
	startupInfo.pAppInstance			= hInstance;
	startupInfo.pGame					= CommandLine()->HasParam( "game" ) ? CommandLine()->GetFirstValue( "game" ) : pGameDir;
	startupInfo.pParentAppSystemGroup	= this;
	pEngineAPI->SetStartupInfo( startupInfo );
	return true;
}

/*
==================
CSingularityEngineApp::Main
==================
*/
int32 CSingularityEngineApp::Main()
{
	return pEngineAPI->Run();
}

/*
==================
CSingularityEngineApp::PostShutdown
==================
*/
void CSingularityEngineApp::PostShutdown()
{
	ConVar_Unregister();
	DisconnectStdLib();
	pWindowMgr->Close();

	pWindowMgr = NULL;
	pEngineAPI = NULL;
}

/*
==================
LauncherMain
==================
*/
extern "C" DLL_EXPORT uint32 LauncherMain( appInstanceHandle_t hInstance, const achar* pGameDir, const achar* pCommandLine )
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

	// Run application
	CSingularityEngineApp		application( pGameDir, hInstance );
	return application.Run();
}