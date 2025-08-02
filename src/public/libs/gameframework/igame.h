#pragma once
#include "appframework/iappsystem.h"

//-----------------------------------------------------------------------------
// Game interface
//-----------------------------------------------------------------------------
#define GAME_INTERFACE_VERSION		"SGame001"
class IGame : public IAppSystem
{
public:
	// Process one game frame
	virtual void FrameUpdate() = 0;

	// Return string describing current module. For example: Singularity Sandbox, Nuclear Frost, etc
	virtual const achar* GetGameDescription() const = 0;
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
	gameAppSystemOrder_t	order;			// Determines the order in which systems are initialized
	const achar*			pModuleName;
	const achar*			pInterfaceName;
};


#define GAME_APPSYSTEMS_INTERFACE_VERSION		"SGameAppSystems001"
class IGameAppSystems
{
public:
	virtual uint32 GetNum() const = 0;
	virtual gameAppSystemInfo_t GetInfo( uint32 index ) const = 0;
};