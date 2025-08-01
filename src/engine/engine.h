#pragma once
#include "stdlib/defines.h"
#include "gameinfo/gameinfo.h"
#include "studiorender/istudio_viewport.h"
#include "engine/viewportclient_game.h"
#include "appframework/iwindowmgr.h"

//-----------------------------------------------------------------------------
// The game engine
//-----------------------------------------------------------------------------
class CEngine
{
public:
	CEngine();

	// Initialize and shutdown functions
	bool Init();
	void Shutdown();
	bool IsInited() const;

	void FrameUpdate();

	// Reads key bindings and archived cvars from pPath.
	// If this file isn't exist try read from <filename>_default 
	void ReadConfiguration( const achar* pPath, bool bWriteConfigIfNoExist = true );

	// Writes key bindings and archived cvars into file
	void WriteConfiguration( const achar* pPath );

	// Load and unload gameinfo.txt
	bool LoadGameInfo( const achar* pPath );
	void UnloadGameInfo();
	const CGameInfoDoc& GetGameInfo() const;

private:
	void OverrideConfigurationFromCommandLine();
	static void OnProcessWindowEvent( void* pUserData, const windowEvent_t& windowEvent );

	bool									bInited;
	bool									bInFocus;
	CGameInfoDoc							gameinfo;
	CGameViewportClient						gameViewportClient;
	TRefPtr<IStudioViewport>				pStudioViewport;
	IOnProcessWindowEvent::funcDelegate_t*	pProcessWindowEventDelegate;
};
extern CEngine		g_Engine;

#include "engine/engine.inl"