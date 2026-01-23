#pragma once
#include "appframework/iappsystemgroup.h"
#include "game/igame.h"
#include "game/shared/ecs/ecs_core.h"
#include "game/shared/ecs/ecs_component_factory.h"
#include "game/shared/ecs/ecs_component_serialize.h"
#include "game/shared/ecs/ecs_entitydesc_factory.h"
#include "game/shared/ecs/ecs_map.h"

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
	// NOTE: The path to the map in the file system can be without file extension
	virtual bool  MapInit( const char* pPath ) override;
	virtual void  MapShutdown() override;
	virtual bool  HasActiveMap() const override;
	virtual IMap* GetActiveMap() const override;

	// Process one game frame
	virtual void FrameUpdate() override;

	CGame();
	CEcsMap*			GetActiveEcsMap() const;
	CEcsComponentTypes& GetEcsComponentTypes();

protected:
	CEcsComponentTypes ecsComponentTypes;
	CEcsMap*		   pActiveEcsMap;

private:
	CEcsEntityDescFactory ecsEntityDescFactory;
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
	virtual uint32				GetNum() const override;
	virtual gameAppSystemInfo_t GetInfo( uint32 index ) const override;

protected:
	void AddAppSystem( const char* pModuleName, const char* pInterfaceName, gameAppSystemOrder_t order );

private:
	eastl::vector<gameAppSystemInfo_t> appSystems;
};

#include "game/shared/game.inl"
