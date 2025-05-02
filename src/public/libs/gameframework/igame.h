#ifndef IGAME_H
#define IGAME_H

#include "core/types.h"

//-----------------------------------------------------------------------------
// Game interface
//-----------------------------------------------------------------------------
#define GAME_INTERFACE_VERSION		"SGame001"
class IGame
{
public:
	// Methods initialize and shutdown the game
	virtual bool Init( createInterfaceFn_t pFactory ) = 0;
	virtual void Shutdown() = 0;

	// Process one game frame
	virtual void FrameUpdate() = 0;

	// Return string describing current module. For example: Singularity Sandbox, Nuclear Frost, etc
	virtual const achar* GetGameDescription() const = 0;
};


//-----------------------------------------------------------------------------
// Interface exposed from the game module back to the engine for specifying IAppSystems
//-----------------------------------------------------------------------------
#define GAME_APPSYSTEMS_INTERFACE_VERSION		"SGameAppSystems001"
class IGameAppSystems
{
public:
	virtual uint32 GetNum() const = 0;
	virtual const achar* GetModuleName( uint32 index ) const = 0;
	virtual const achar* GetInterfaceName( uint32 index ) const = 0;
};

#endif // !IGAME_H