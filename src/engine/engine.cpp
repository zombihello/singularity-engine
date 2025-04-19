#include "pch_engine.h"
#include "appframework/iwindowmgr.h"
#include "gameinfo/gameinfo.h"
#include "core/icommandline.h"
#include "core/profile.h"
#include "core/crashdump.h"
#include "inputsystem/iinputsystem.h"
#include "filesystem/ifilesystem.h"
#include "studiorender/istudiorender.h"
#include "studiorender/studioapi/istudioapi.h"
#include "gameframework/igame.h"
#include "engine/engineapi.h"
#include "engine/engine.h"

//-----------------------------------------------------------------------------
// Global values and cvars
//-----------------------------------------------------------------------------
CEngine		g_Engine;

CConVar		r_windowwidth( "r_windowwidth", "1280", "Window width", FCVAR_ARCHIVE );
CConVar		r_windowheight( "r_windowheight", "720", "Window height", FCVAR_ARCHIVE );
CConVar		r_fullscreen( "r_fullscreen", "0", "Is need open the window in fullscreen mode", FCVAR_ARCHIVE );
CConVar		r_vsync( "r_vsync", "0", "Should use vertical synchronization (VSync)", FCVAR_ARCHIVE );


/*
==================
CEngine::CEngine
==================
*/
CEngine::CEngine()
	: bInited( false )
	, bInFocus( true )
	, pProcessWindowEventDelegate( NULL )
{}

/*
==================
CEngine::Init
==================
*/
bool CEngine::Init()
{
	// gameinfo.txt must be loaded
	if ( !gameinfo.IsLoaded() )
	{
		Warning( "Engine: gameinfo.txt must be loaded!" );
		return false;
	}
	
	// Setup application information for the crash dump
	crashDumpAppInfo_t				crashDumpAppInfo = {};
	crashDumpAppInfo.pAppName		= gameinfo.GetGame().c_str();
	crashDumpAppInfo.pAppVersion	= gameinfo.GetVersion().c_str();
	crashDumpAppInfo.pSupportEmail	= gameinfo.GetSupportEmail().c_str();
	crashDumpAppInfo.pSupportURL	= gameinfo.GetSupportURL().c_str();
	CrashDump_SetAppInfo( crashDumpAppInfo );

	// Attach the input system to window
	g_pInputSystem->AttachToWindow( g_pWindowMgr );

	// Initialize the game
	if ( !g_pGame->Init( g_pAppSystemFactory ) )
	{
		return false;
	}

	// Read file configuration and override it from command line
	ReadConfiguration( "//GAME/cfg/config.cfg" );
	OverrideConfigurationFromCommandLine();

	// Update window size and show the one
	g_pWindowMgr->SetSize( r_windowwidth.GetInt(), r_windowheight.GetInt() );
	g_pWindowMgr->SetTitle( gameinfo.GetGame().c_str() );
	g_pWindowMgr->SetFullscreen( r_fullscreen.GetBool() );
	g_pWindowMgr->ShowWindow();

	// Create and initialize the viewport
	pStudioViewport = g_pStudioRender->CreateViewport();
	pStudioViewport->SetViewportClient( &gameViewportClient );
	pStudioViewport->Init( g_pWindowMgr->GetHandle(), r_windowwidth.GetInt(), r_windowheight.GetInt(), r_vsync.GetBool() );

	// Subscribe on window events
	pProcessWindowEventDelegate = g_pWindowMgr->OnProcessWindowEvent()->AddFunc( &CEngine::OnProcessWindowEvent, this );

	// We are done!
	bInited = true;
	return true;
}

/*
==================
CEngine::Shutdown
==================
*/
void CEngine::Shutdown()
{
	if ( bInited )
	{
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

		// Shutdown the game
		g_pGame->Shutdown();

		// Detach the input system from the window
		g_pInputSystem->DetachFromWindow();
		bInited = false;
	}
}

/*
==================
CEngine::FrameUpdate
==================
*/
void CEngine::FrameUpdate()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_GAMELOGIC );

	// Do nothing if we haven't the window focus
	if ( !bInFocus && !PROFILE_IS_RECORD() )
	{
		return;
	}

	// Update a game logic frame and draw it
	g_pGame->FrameUpdate();
	if ( pStudioViewport )
	{
		pStudioViewport->DrawFrame();
	}
}

/*
==================
CEngine::OnProcessWindowEvent
==================
*/
void CEngine::OnProcessWindowEvent( void* pUserData, const windowEvent_t& windowEvent )
{
	PROFILE_SCOPE();
	CEngine*		pEngine = ( CEngine* )pUserData;
	switch ( windowEvent.type )
	{
		// Focus gained
	case windowEvent_t::EVENT_WINDOW_RESTORED:
	case windowEvent_t::EVENT_WINDOW_FOCUS_GAINED:
		if ( g_pWindowMgr->GetID() == windowEvent.windowId )
		{
			pEngine->bInFocus = true;
		}
		break;

		// Focus lost
	case windowEvent_t::EVENT_WINDOW_MINIMIZED:
	case windowEvent_t::EVENT_WINDOW_FOCUS_LOST:
		if ( g_pWindowMgr->GetID() == windowEvent.windowId )
		{
			pEngine->bInFocus = false;
		}
		break;

		// Resize window
	case windowEvent_t::EVENT_WINDOW_RESIZE:
		if ( g_pWindowMgr->GetID() == windowEvent.windowId && pEngine->pStudioViewport )
		{
			pEngine->pStudioViewport->Resize( windowEvent.events.windowResize.width, windowEvent.events.windowResize.height );
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
CEngine::ReadConfiguration
==================
*/
void CEngine::ReadConfiguration( const achar* pPath, bool bWriteConfigIfNoExist /* = true */ )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// If pPath is exist execute it
	if ( g_pFileSystem->IsFileExists( pPath ) )
	{
		g_pCvar->Exec( S_Sprintf( "exec %s", pPath ).c_str() );
		return;
	}

	// Otherwise try execute <filename>_default if the one is exist
	std::string		defaultPath;
	std::string		baseFileName;
	S_GetFilePath( pPath, defaultPath, false );
	S_GetFileBaseName( pPath, baseFileName, false );
	defaultPath		+= S_Sprintf( "%s_default", baseFileName.c_str() );
	defaultPath		+= S_GetFileExtension( pPath, true );
	if ( g_pFileSystem->IsFileExists( defaultPath.c_str() ) )
	{
		g_pCvar->Exec( S_Sprintf( "exec %s", defaultPath.c_str() ).c_str() );
	}

	// Save a new config if it need
	if ( bWriteConfigIfNoExist )
	{
		WriteConfiguration( pPath );
	}
}

/*
==================
CEngine::WriteConfiguration
==================
*/
void CEngine::WriteConfiguration( const achar* pPath )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Open file to write
	TRefPtr<IStreamDataWriter>	file = g_pFileSystem->CreateFileWriter( pPath );
	if ( !file )
	{
		Warning( "CEngine::WriteConfiguration: Failed to create file configuration \"%s\"", pPath );
		return;
	}
	std::string		buffer;

	// Write bindings
	buffer += "unbindall\n";
	for ( uint32 index = 0; index < BUTTON_CODE_COUNT; ++index )
	{
		const achar*	pCommand = g_pInputSystem->GetBindingCommand( ( buttonCode_t )index );
		if ( !pCommand || !pCommand[0] )
		{
			continue;
		}

		buffer += S_Sprintf( "bind \"%s\" \"%s\"\n", g_pInputSystem->GetButtonName( ( buttonCode_t )index ), pCommand );
	}

	// Write cvars
	for ( IConCmdBase* pVar = g_pCvar->GetCommands(); pVar; pVar = pVar->GetNext() )
	{
		// Skip commands and cvars that not have FCVAR_ARCHIVE
		if ( pVar->IsCommand() || !pVar->IsFlagSet( FCVAR_ARCHIVE ) )
		{
			continue;;
		}

		buffer += S_Sprintf( "%s \"%s\"\n", pVar->GetName(), ( ( IConVar* )pVar )->GetString() );
	}

	// Write buffer into file
	file->Write( buffer.data(), buffer.size() * sizeof( achar ) );
}

/*
==================
CEngine::OverrideConfigurationFromCommandLine
==================
*/
void CEngine::OverrideConfigurationFromCommandLine()
{
	PROFILE_SCOPE();
	ICommandLine*	pCommandLine = CommandLine();
	
	// Check for windowed mode command line override
	if ( pCommandLine->HasParam( "windowed" ) || pCommandLine->HasParam( "window" ) )
	{
		r_fullscreen.SetInt( 0 );
	}
	// Check for fullscreen override
	else if ( pCommandLine->HasParam( "full" ) || pCommandLine->HasParam( "fullscreen" ) )
	{
		r_fullscreen.SetInt( 1 );
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
		r_windowwidth.SetString( pCommandLine->GetFirstValue( pWidthParam ) );
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
		r_windowheight.SetString( pCommandLine->GetFirstValue( pHeightParam ) );
	}
}