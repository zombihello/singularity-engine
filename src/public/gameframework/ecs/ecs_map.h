#pragma once
#include "utils/smapdoc/smap_compiled_doc.h"
#include "gameframework/ecs/ecs_core.h"
#include "gameframework/ecs/ecs_component_factory.h"
#include "gameframework/ecs/ecs_entity.h"
#include "gameframework/imap.h"

//-----------------------------------------------------------------------------
// ECS map delegates
//-----------------------------------------------------------------------------
DECLARE_MULTICAST_DELEGATE( COnMapReseted, IMap* /* pMap */ );
DECLARE_MULTICAST_DELEGATE( COnMapUnloaded, IMap* /* pMap */ );

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
	eastl::vector<TRefPtr<CEcsEntity>> ecsEntities;
};

#include "gameframework/ecs/ecs_map.inl"
