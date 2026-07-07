#pragma once
#include "resourcesystem/iresourcetype.h"
#include "game/igame.h"
#include "game/shared/ecs/ecs_core.h"
#include "game/shared/ecs/ecs_component_factory.h"
#include "game/shared/ecs/ecs_component_serialize.h"
#include "game/shared/ecs/ecs_entitydesc_loader.h"
#include "game/shared/ecs/ecs_entitydesc.h"
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
	virtual void FrameUpdate( float deltaTime ) override;
	virtual void FrameDraw( IStudioViewport* pStudioViewport ) override;

	CGame();
	CEcsMap*			GetActiveEcsMap() const;
	CEcsComponentTypes& GetEcsComponentTypes();

protected:
	CEcsComponentTypes ecsComponentTypes;
	CEcsMap*		   pActiveEcsMap;

private:
	CResourceTypeFactory<CEcsEntityDesc> ecsEntityDescFactory;
	CEcsEntityDescLoader				 ecsEntityDescLoader;
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
