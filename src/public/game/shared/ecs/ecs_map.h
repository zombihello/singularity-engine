#pragma once
#include "utils/smapdoc/smap_compiled_doc.h"
#include "game/shared/ecs/ecs_core.h"
#include "game/shared/ecs/ecs_component_factory.h"
#include "game/shared/ecs/ecs_entity.h"
#include "game/imap.h"

//-----------------------------------------------------------------------------
// ECS map delegates
//-----------------------------------------------------------------------------
DECLARE_EVENT( COnMapReseted, IMap* /* pMap */ );
DECLARE_EVENT( COnMapUnloaded, IMap* /* pMap */ );

//-----------------------------------------------------------------------------
// ECS map
//-----------------------------------------------------------------------------
class CEcsMap : public IMap
{
public:
	// IMap interface
	// Spawn and destroy an entity
	virtual IEntity* SpawnEntity( IEntityDesc* pEntityDesc, const char* pName = "" ) override;
	virtual void	 DestroyEntity( IEntity* pEntity ) override;

	// Resets the map to initial state to before any updates on the map
	virtual void Reset() override;
	virtual void Update( float deltaTime ) override;

	virtual IOnMapReseted*	OnMapReseted() const override;
	virtual IOnMapUnloaded* OnMapUnloaded() const override;

	CEcsMap();
	~CEcsMap();

	void Init();
	void Init( const CSMAPCompiledMapDoc& smapCompiledDoc );

	bool		  IsInitialized() const;
	CEcsWorld&	  GetEcsWorld();
	IStudioScene* GetStudioScene() const;

protected:
	virtual void SetupEcsResources();

private:
	bool							   bInitialized;
	CEcsWorld						   ecsWorld;
	COnMapReseted					   onMapReseted;
	COnMapUnloaded					   onMapUnloaded;
	CRefPtr<IStudioScene>			   pStudioScene;
	eastl::vector<CRefPtr<CEcsEntity>> ecsEntities;
};

#include "game/shared/ecs/ecs_map.inl"
