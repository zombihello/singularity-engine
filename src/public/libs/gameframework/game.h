#pragma once
#include "appframework/iappsystemgroup.h"
#include "gameframework/igame.h"
#include "gameframework/ecs/ecs_core.h"
#include "gameframework/ecs/ecs_component_factory.h"
#include "gameframework/ecs/ecs_component_serialize.h"
#include "gameframework/ecs/ecs_entitydesc_factory.h"
#include "gameframework/ecs/ecs_map.h"
#include "gameframework/ecs/ecs_map_factory.h"

//-----------------------------------------------------------------------------
// Base class of the game
//-----------------------------------------------------------------------------
class CGame : public CBaseAppSystem<IGame>
{
public:
	// IAppSystem interfaces
	// Here's where the app systems get to learn about each other
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// Initialize and shutdown
	virtual bool Init() override;
	virtual void Shutdown() override;

	// IGame interfaces
	// Process one game frame
	virtual void FrameUpdate() override;

	CGame();

	void SetActiveMap( const TResourcePtr<IMap>& pEcsMap );
	IMap* GetActiveMap() const;
	CEcsComponentTypes& GetEcsComponentTypes();

protected:
	CEcsComponentTypes			ecsComponentTypes;

private:
	TResourcePtr<IMap>			pActiveMap;
	CEcsEntityDescFactory		ecsEntityDescFactory;
	CEcsMapFactory				ecsMapFactory;
};

// NOTE: You must implement the function to return a singleton game class
CGame* Game();


//-----------------------------------------------------------------------------
// Base class of the game IAppSystems
//-----------------------------------------------------------------------------
class CGameAppSystems : public IGameAppSystems
{
public:
	// IGameAppSystems interfaces
	virtual uint32 GetNum() const override;
	virtual gameAppSystemInfo_t GetInfo( uint32 index ) const override;

protected:
	void AddAppSystem( const achar* pModuleName, const achar* pInterfaceName, gameAppSystemOrder_t order );

private:
	std::vector<gameAppSystemInfo_t>	appSystems;
};

#include "gameframework/game.inl"