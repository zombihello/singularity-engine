#include "pch_game_shared.h"
#include "resourcesystem/iresourcesystem.h"
#include "resourcesystem/resourceptr.h"
#include "game/ientitydesc.h"
#include "game/shared/game.h"
#include "game/shared/ecs/ecs_entitydesc.h"
#include "game/shared/ecs/ecs_map.h"
#include "game/shared/ecs/ecs_render.gen.h"

/*
==================
CEcsMap::CEcsMap
==================
*/
CEcsMap::CEcsMap()
	: bInitialized( false )
{
}

/*
==================
CEcsMap::~CEcsMap
==================
*/
CEcsMap::~CEcsMap()
{
	Reset();
}

/*
==================
CEcsMap::SetupEcsResources
==================
*/
void CEcsMap::SetupEcsResources()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_SCENE );
	Assert( pStudioScene );
	ecsWorld.SetResource( ecsResourceStudioScene_t{ pStudioScene } );
}

/*
==================
CEcsMap::Init
==================
*/
void CEcsMap::Init()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_SCENE );
	AssertMsg( !bInitialized, "A map can't be initialized twice" );

	// Create a new studio scene and setup ECS resources
	pStudioScene = g_pStudioRender->CreateScene();
	SetupEcsResources();

	// We are done
	bInitialized = true;
}

/*
==================
CEcsMap::Init
==================
*/
void CEcsMap::Init( const CSMAPCompiledMapDoc& smapCompiledDoc )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_SCENE );
	AssertMsg( !bInitialized, "A map can't be initialized twice" );

	// Create entities
	const eastl::vector<CSMAPEntity>& smapEntities	 = smapCompiledDoc.GetEntities();
	IResourceTypeMgr*				  pEntityDescMgr = g_pResourceSystem->GetResourceManagerForType<IEntityDesc>();
	for ( uint32 entityIdx = 0, numEntities = (uint32)smapEntities.size(); entityIdx < numEntities; ++entityIdx )
	{
		const CSMAPEntity&		  smapEntity  = smapEntities[entityIdx];
		CResourcePtr<IEntityDesc> pEntityDesc = pEntityDescMgr->LoadResource( smapEntity.GetClassName() );
		if ( !pEntityDesc )
		{
			Warning( "Game: Entity descriptor '%s' not found for entity %i (name: '%s')", smapEntity.GetClassName(), entityIdx, smapEntity.GetName() );
			continue;
		}

		SpawnEntity( *pEntityDesc, smapEntity.GetName() );
	}

	// Create a new studio scene and setup ECS resources
	pStudioScene = g_pStudioRender->CreateScene();
	SetupEcsResources();

	// We are done
	bInitialized = true;
}

/*
==================
CEcsMap::SpawnEntity
==================
*/
IEntity* CEcsMap::SpawnEntity( IEntityDesc* pEntityDesc, const char* pName /* = "" */ )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_SCENE );
	CRefPtr<CEcsEntity> pNewEcsEntity;
	if ( pEntityDesc )
	{
		CEcsEntityDesc* pEcsEntityDesc = (CEcsEntityDesc*)pEntityDesc;
		pNewEcsEntity				   = new CEcsEntity( ecsWorld.CreateEntity( pName, pEcsEntityDesc->GetEcsPrefab( this ) ), this );
		ecsEntities.emplace_back( pNewEcsEntity );
	}
	else
	{
		Warning( "Game: Failed to spawn entity '%s', its descriptor isn't valid", pName );
	}

	return pNewEcsEntity;
}

/*
==================
CEcsMap::DestroyEntity
==================
*/
void CEcsMap::DestroyEntity( IEntity* pEntity )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_SCENE );
	if ( pEntity )
	{
		CEcsEntity* pEcsEntity = (CEcsEntity*)pEntity;
		for ( uint32 index = 0, count = (uint32)ecsEntities.size(); index < count; ++index )
		{
			if ( ecsEntities[index] == pEcsEntity )
			{
				pEcsEntity->Destroy();
				ecsEntities.erase( ecsEntities.begin() + index );
				break;
			}
		}
	}
}

/*
==================
CEcsMap::Reset
==================
*/
void CEcsMap::Reset()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_SCENE );
	onMapReseted.Invoke( this );

	// TODO BS yehor.pohuliaka - Implement reload the map from the file system
	ecsWorld.Reset();
	ecsEntities.clear();
	pStudioScene = NULL;
	bInitialized = false;

	// Initialize the map as empty
	Init();
}

/*
==================
CEcsMap::Update
==================
*/
void CEcsMap::Update( float deltaTime )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_SCENE );
	pStudioScene->Update( deltaTime );
	ecsWorld.Update( deltaTime );
}

/*
==================
CEcsMap::OnMapReseted
==================
*/
IOnMapReseted* CEcsMap::OnMapReseted() const
{
	return (IOnMapReseted*)&onMapReseted;
}

/*
==================
CEcsMap::OnMapUnloaded
==================
*/
IOnMapUnloaded* CEcsMap::OnMapUnloaded() const
{
	return (IOnMapUnloaded*)&onMapUnloaded;
}
