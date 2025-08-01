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
class CGame : public IGame
{
public:
	// IGame interfaces
	// Methods initialize and shutdown the game
	virtual bool Init( createInterfaceFn_t pFactory ) override;
	virtual void Shutdown() override;

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
	virtual const achar* GetModuleName( uint32 index ) const override;
	virtual const achar* GetInterfaceName( uint32 index ) const override;

protected:
	void AddAppSystem( const achar* pModuleName, const achar* pInterfaceName );

private:
	std::vector<appSystemInfo_t>		appSystems;
};

#include "gameframework/game.inl"