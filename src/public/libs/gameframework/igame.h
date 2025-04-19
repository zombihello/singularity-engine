/**
 * @file
 * @addtogroup gameframework gameframework
 */

#ifndef IGAME_H
#define IGAME_H

#include <vector>

#include "stdlib/interface.h"
#include "appframework/iappsystemgroup.h"

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
// Base class of the game
//-----------------------------------------------------------------------------
class CGame : public IGame
{
public:
	// Methods initialize and shutdown the game
	virtual bool Init( createInterfaceFn_t pFactory ) override;
	virtual void Shutdown() override;

	// Process one game frame
	virtual void FrameUpdate() override;
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


//-----------------------------------------------------------------------------
// Base class of the game IAppSystems
//-----------------------------------------------------------------------------
class CGameAppSystems : public IGameAppSystems
{
public:
	virtual uint32 GetNum() const override;
	virtual const achar* GetModuleName( uint32 index ) const override;
	virtual const achar* GetInterfaceName( uint32 index ) const override;

protected:
	FORCEINLINE void AddAppSystem( const achar* pModuleName, const achar* pInterfaceName )
	{
		appSystemInfo_t&				appSystemInfo = appSystems.emplace_back();
		appSystemInfo.pModuleName		= pModuleName;
		appSystemInfo.pInterfaceName	= pInterfaceName;
	}

private:
	std::vector<appSystemInfo_t>		appSystems;
};

#endif // !IGAME_H