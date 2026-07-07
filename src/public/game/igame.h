#pragma once
#include "appframework/iappsystem.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IMap;
class IStudioViewport;

//-----------------------------------------------------------------------------
// Game interface
//-----------------------------------------------------------------------------
#define GAME_INTERFACE_VERSION "SGame001"
class IGame : public IAppSystem
{
public:
	// Initialize/shutdown a map and get the active map
	// NOTE: The path to the map in the file system can be without file extension
	virtual bool  MapInit( const char* pPath ) = 0;
	virtual void  MapShutdown()				   = 0;
	virtual bool  HasActiveMap() const		   = 0;
	virtual IMap* GetActiveMap() const		   = 0;

	// Process one game frame
	virtual void FrameUpdate( float deltaTime )				   = 0;
	virtual void FrameDraw( IStudioViewport* pStudioViewport ) = 0;

	// Return string describing current module. For example: Singularity Sandbox, Nuclear Frost, etc
	virtual const char* GetGameDescription() const = 0;
};

//-----------------------------------------------------------------------------
// Interface exposed from the game module back to the engine for specifying IAppSystems
//-----------------------------------------------------------------------------
enum gameAppSystemOrder_t
{
	GAME_APPSYSTEM_ORDER_BEFORE_GAME,
	GAME_APPSYSTEM_ORDER_AFTER_GAME
};

struct gameAppSystemInfo_t
{
	gameAppSystemOrder_t order;	 // Determines the order in which systems are initialized
	const char*			 pModuleName;
	const char*			 pInterfaceName;
};

#define GAME_APPSYSTEMS_INTERFACE_VERSION "SGameAppSystems001"
class IGameAppSystems
{
public:
	virtual uint32				GetNum() const				  = 0;
	virtual gameAppSystemInfo_t GetInfo( uint32 index ) const = 0;
};