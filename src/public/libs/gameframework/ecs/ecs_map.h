#ifndef ECS_MAP_H
#define ECS_MAP_H

#include "smapdoc/smap_compiled_doc.h"
#include "gameframework/ecs/ecs_core.h"
#include "gameframework/ecs/ecs_component_factory.h"
#include "gameframework/imap.h"

//-----------------------------------------------------------------------------
// ECS map delegates
//-----------------------------------------------------------------------------
DECLARE_MULTICAST_DELEGATE( COnMapReseted, IMap* /* pMap */ );
DECLARE_MULTICAST_DELEGATE( COnMapUnloaded, IMap* /* pMap */ );


//-----------------------------------------------------------------------------
// ECS map
//-----------------------------------------------------------------------------
class CEcsMap : public TRefCounted<IMap>
{
public:
	CEcsMap();
	CEcsMap( const CSMAPCompiledMapDoc& smapCompiledDoc );
	~CEcsMap();

	// IMap interface
	// Resets the map to initial state to before any updates on the map
	virtual void Reset() override;
	virtual void Update( float deltaTime ) override;

	virtual IOnMapReseted* OnMapReseted() const override;
	virtual IOnMapUnloaded* OnMapUnloaded() const override;

	CEcsWorld& GetEcsWorld();

private:
	void Init( const CSMAPCompiledMapDoc& smapCompiledDoc );

	CEcsWorld		ecsWorld;
	COnMapReseted	onMapReseted;
	COnMapUnloaded	onMapUnloaded;
};

#include "gameframework/ecs/ecs_map.inl"

#endif // !ECS_MAP_H
