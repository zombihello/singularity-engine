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
// Forward declarations
//-----------------------------------------------------------------------------
class CEcsEntityDesc;

//-----------------------------------------------------------------------------
// ECS map
//-----------------------------------------------------------------------------
class CEcsMap : public IMap
{
public:
	friend CEcsEntityDesc;
	CEcsMap();
	CEcsMap( const CSMAPCompiledMapDoc& smapCompiledDoc );
	~CEcsMap();

	// IMap interface
	// Spawn and destroy an entity
	virtual IEntity* SpawnEntity( IEntityDesc* pEntityDesc, const char* pName = "" ) override;
	virtual void	 DestroyEntity( IEntity* pEntity ) override;

	// Resets the map to initial state to before any updates on the map
	virtual void Reset() override;
	virtual void Update( float deltaTime ) override;

	virtual IOnMapReseted*	OnMapReseted() const override;
	virtual IOnMapUnloaded* OnMapUnloaded() const override;

	CEcsWorld& GetEcsWorld();

private:
	void Init( const CSMAPCompiledMapDoc& smapCompiledDoc );

	CEcsWorld						   ecsWorld;
	COnMapReseted					   onMapReseted;
	COnMapUnloaded					   onMapUnloaded;
	eastl::vector<CRefPtr<CEcsEntity>> ecsEntities;
};

#include "game/shared/ecs/ecs_map.inl"
