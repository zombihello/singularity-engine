#include "pch_gameframework.h"
#include "resourcesystem/iresourcesystem.h"
#include "gameframework/ientitydesc.h"
#include "gameframework/game.h"
#include "gameframework/ecs/ecs_map.h"

/*
==================
CEcsMap::CEcsMap
==================
*/
CEcsMap::CEcsMap()
{}

/*
==================
CEcsMap::CEcsMap
==================
*/
CEcsMap::CEcsMap( const CSMAPCompiledMapDoc& smapCompiledDoc )
{
	// Initialize the map by SMAP compiled document
	Init( smapCompiledDoc );
}

/*
==================
CEcsMap::~CEcsMap
==================
*/
CEcsMap::~CEcsMap()
{
	onMapUnloaded.Broadcast( this );
	onMapReseted.RemoveAll();
	Reset();
}

/*
==================
CEcsMap::Init
==================
*/
void CEcsMap::Init( const CSMAPCompiledMapDoc& smapCompiledDoc )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_SCENE )
	Reset();

	// Create entities
	const std::vector<CSMAPEntity>&		smapEntities = smapCompiledDoc.GetEntities();
	for ( uint32 entityIdx = 0, numEntities = ( uint32 )smapEntities.size(); entityIdx < numEntities; ++entityIdx )
	{
		const CSMAPEntity&			smapEntity = smapEntities[entityIdx];
		TResourcePtr<IEntityDesc>	pEntityDesc = g_pResourceSystem->FindOrLoadResource( smapEntity.GetEntityDesc(), RESOURCE_TYPE_ENTITY_DESC );
		if ( !pEntityDesc )
		{
			Warning( "Game: Entity descriptor '%s' not found for entity %i (name: '%s')", smapEntity.GetEntityDesc(), entityIdx, smapEntity.GetName() );
			continue;
		}

		pEntityDesc->Create( this, smapEntity.GetName() );
	}
}

/*
==================
CEcsMap::Reset
==================
*/
void CEcsMap::Reset()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_SCENE )
	onMapReseted.Broadcast( this );

	// TODO BS yehor.pohuliaka - Implement reload the map from the file system
	ecsWorld.Reset();
}

/*
==================
CEcsMap::Update
==================
*/
void CEcsMap::Update( float deltaTime )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_SCENE )
	ecsWorld.Update( deltaTime );
}

/*
==================
CEcsMap::OnMapReseted
==================
*/
IOnMapReseted* CEcsMap::OnMapReseted() const
{
	return ( IOnMapReseted* )&onMapReseted;
}

/*
==================
CEcsMap::OnMapUnloaded
==================
*/
IOnMapUnloaded* CEcsMap::OnMapUnloaded() const
{
	return ( IOnMapUnloaded* )&onMapUnloaded;
}