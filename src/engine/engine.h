/**
 * @file
 * @addtogroup engine engine
 */

#ifndef ENGINE_H
#define ENGINE_H

#include "core/platform.h"
#include "gameinfo/gameinfo.h"
#include "studiorender/istudio_viewport.h"
#include "engine/viewportclient_game.h"
#include "appframework/iwindowmgr.h"

/**
 * @ingroup engine
 * @brief The game engine
 */
class CEngine
{
public:
	/**
	 * @brief Constructor
	 */
	CEngine();

	/**
	 * @brief Initialize the game engine
	 * @return Return TRUE if the game engine is successfully initialized, otherwise FALSE
	 */
	bool Init();

	/**
	 * @brief Shutdown the game engine
	 */
	void Shutdown();

	/**
	 * @brief Process one game frame
	 */
	void FrameUpdate();

	/**
	 * @brief Is initialized the game engine
	 * @return Return TRUE if the game engine is initialized, otherwise FALSE
	 */
	FORCEINLINE bool IsInited() const
	{
		return bInited;
	}

	/**
	 * @brief Read file configuration
	 * Reads key bindings and archived cvars from pPath.
	 * If this file isn't exist try read from <filename>_default
	 * 
	 * @param pPath						Path to file
	 * @param bWriteConfigIfNoExist		Is need write a file if the one isn't exist
	 */
	void ReadConfiguration( const achar* pPath, bool bWriteConfigIfNoExist = true );

	/**
	 * @brief Write file configuration
	 * Writes key bindings and archived cvars into file
	 * 
	 * @param pPath		Path to file
	 */
	void WriteConfiguration( const achar* pPath );

	/**
	 * @brief Load gameinfo.txt
	 * 
	 * @param pPath		Path to gameinfo.txt
	 * @return Return TRUE if gameinfo loaded, otherwise returns FALSE
	 */
	FORCEINLINE bool LoadGameInfo( const achar* pPath )
	{
		return gameinfo.LoadFromFile( pPath );
	}

	/**
	 * @brief Unload gameinfo.txt
	 */
	FORCEINLINE void UnloadGameInfo()
	{
		gameinfo.Clear();
	}

	/**
	 * @brief Get gameinfo
	 * @return Return gameinfo
	 */
	FORCEINLINE const CGameInfoDoc& GetGameInfo() const
	{
		return gameinfo;
	}

private:
	/**
	 * @brief Override configuration with command line params
	 */
	void OverrideConfigurationFromCommandLine();

	/**
	 * @brief Process window event
	 * @param pUserData		Pointer to CEngine
	 * @param windowEvent	Window event
	 */
	static void OnProcessWindowEvent( void* pUserData, const windowEvent_t& windowEvent );

	bool									bInited;						/**< Is initialized the game engine */
	bool									bInFocus;						/**< Is the window in focus */
	CGameInfoDoc							gameinfo;						/**< Game info */
	CGameViewportClient						gameViewportClient;				/**< Game viewport client */
	TRefPtr<IStudioViewport>				pStudioViewport;				/**< Studio viewport */
	IOnProcessWindowEvent::funcDelegate_t*	pProcessWindowEventDelegate;	/**< Delegate of OnProcessWindowEvent */
};

/**
 * @ingroup engine
 * @brief The game engine
 */
extern CEngine		g_Engine;

#endif // !ENGINE_H