#include "pch_gameframework.h"
#include "gameframework/game.h"
#include "gameframework/ecs/ecs_entity.h"
#include "gameframework/ecs/ecs_entitydesc.h"

/*
==================
CEcsEntityDesc::CEcsEntityDesc
==================
*/
CEcsEntityDesc::CEcsEntityDesc()
	: lastUsedEcsPrefabIdx( INVALID_INDEX )
{}

/*
==================
CEcsEntityDesc::CEcsEntityDesc
==================
*/
CEcsEntityDesc::CEcsEntityDesc( const CSENTCompiledEntityDescDoc& sentCompiledDoc )
{
	// Initialize the entity descriptor by SENT compiled document
	Init( sentCompiledDoc );
}

/*
==================
CEcsEntityDesc::~CEcsEntityDesc
==================
*/
CEcsEntityDesc::~CEcsEntityDesc()
{
	Clear();
}

/*
==================
CEcsEntityDesc::Init
==================
*/
void CEcsEntityDesc::Init( const CSENTCompiledEntityDescDoc& sentCompiledDoc )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_SCENE )
	Clear();

	// Initialize new ECS component factories
	CEcsComponentTypes&								ecsComponentTypes	= Game()->GetEcsComponentTypes();
	const std::vector<CSENTEntityDescComponent>&	sentComponents		= sentCompiledDoc.GetComponents();
	for ( uint32 componentIdx = 0, numComponents = sentCompiledDoc.GetNumComponents(); componentIdx < numComponents; ++componentIdx )
	{
		TRefPtr<IEcsComponentFactory>	pEcsComponentFactory = ecsComponentTypes.CreateFactory( sentComponents[componentIdx] );
		if ( pEcsComponentFactory )
		{
			ecsComponentFactories.emplace_back( pEcsComponentFactory );
		}
	}
}

/*
==================
CEcsEntityDesc::CreateEcsPrefab
==================
*/
uint32 CEcsEntityDesc::CreateEcsPrefab( CEcsMap* pEcsMap, const achar* pName, uint32 ecsPrefabIdx /* = INVALID_INDEX */ ) const
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_SCENE );
	Assert( pEcsMap );
	Assert( ecsPrefabIdx == INVALID_INDEX || ecsPrefabIdx < ecsPrefabs.size() );

	// Create a new ECS prefab
	CEcsWorld&		ecsWorld	= pEcsMap->GetEcsWorld();
	ecsEntity_t		ecsEntity	= ecsWorld.CreatePrefab( pName );
	for ( uint32 componentIdx = 0, numComponents = ( uint32 )ecsComponentFactories.size(); componentIdx < numComponents; ++componentIdx )
	{
		IEcsComponentFactory*	pEcsComponentFactory = ecsComponentFactories[componentIdx];
		pEcsComponentFactory->Create( ecsWorld, ecsEntity );
	}
	
	// Register it in our array
	if ( ecsPrefabIdx != INVALID_INDEX )
	{
		ecsPrefabs[ecsPrefabIdx].ecsEntity = ecsEntity;
		return ecsPrefabIdx;
	}

	ecsPrefab_t							ecsPrefab = {};
	ecsPrefab.pEcsMap					= pEcsMap;
	ecsPrefab.ecsEntity					= ecsEntity;
	ecsPrefab.pOnMapResetedDelegate		= pEcsMap->OnMapReseted()->AddFunc( &CEcsEntityDesc::OnMapResetedOrUnloaded, ( void* )this );
	ecsPrefab.pOnMapUnloadedDelegate	= pEcsMap->OnMapUnloaded()->AddFunc( &CEcsEntityDesc::OnMapResetedOrUnloaded, ( void* )this );
	ecsPrefabs.emplace_back( ecsPrefab );
	return ( uint32 )ecsPrefabs.size() - 1;
}

/*
==================
CEcsEntityDesc::OnMapResetedOrUnloaded
==================
*/
void CEcsEntityDesc::OnMapResetedOrUnloaded( void* pUserData, IMap* pMap )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_SCENE )
	CEcsEntityDesc*		pEcsEntityDesc	= ( CEcsEntityDesc* )pUserData;
	for ( uint32 ecsPrefabIdx = 0, numEcsPrefabs = ( uint32 )pEcsEntityDesc->ecsPrefabs.size(); ecsPrefabIdx < numEcsPrefabs; ++ecsPrefabIdx )
	{
		ecsPrefab_t&	ecsPrefab = pEcsEntityDesc->ecsPrefabs[ecsPrefabIdx];
		if ( ecsPrefab.pEcsMap != pMap )
		{
			continue;
		}

		CEcsWorld&	ecsWorld = ( ( CEcsMap* )pMap )->GetEcsWorld();
		if ( ecsWorld.IsValidEntity( ecsPrefab.ecsEntity ) )
		{
			ecsWorld.DestroyEntity( ecsPrefab.ecsEntity );
		}

		pEcsEntityDesc->ecsPrefabs.erase( pEcsEntityDesc->ecsPrefabs.begin() + ecsPrefabIdx );
		if ( ecsPrefabIdx < pEcsEntityDesc->lastUsedEcsPrefabIdx )
		{
			--pEcsEntityDesc->lastUsedEcsPrefabIdx;
		}
		else if ( ecsPrefabIdx == pEcsEntityDesc->lastUsedEcsPrefabIdx )
		{
			pEcsEntityDesc->lastUsedEcsPrefabIdx = INVALID_INDEX;
		}
		return;
	}
}

/*
==================
CEcsEntityDesc::Clear
==================
*/
void CEcsEntityDesc::Clear()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_SCENE )
	for ( uint32 ecsPrefabIdx = 0, numEcsPrefabs = ( uint32 )ecsPrefabs.size(); ecsPrefabIdx < numEcsPrefabs; ++ecsPrefabIdx )
	{
		ecsPrefab_t&	ecsPrefab	= ecsPrefabs[ecsPrefabIdx];
		CEcsWorld&		ecsWorld	= ecsPrefab.pEcsMap->GetEcsWorld();
		if ( ecsWorld.IsValidEntity( ecsPrefab.ecsEntity ) )
		{
			ecsWorld.DestroyEntity( ecsPrefab.ecsEntity );
		}

		ecsPrefab.pEcsMap->OnMapReseted()->RemoveFunc( ecsPrefab.pOnMapResetedDelegate );
		ecsPrefab.pEcsMap->OnMapUnloaded()->RemoveFunc( ecsPrefab.pOnMapUnloadedDelegate );
	}

	// Clear ECS component factories, prefabs and update GUID
	ecsComponentFactories.clear();
	ecsPrefabs.clear();
	lastUsedEcsPrefabIdx = INVALID_INDEX;
	Sys_InitGuid( guid );
}

/*
==================
CEcsEntityDesc::Create
==================
*/
IEntity* CEcsEntityDesc::Create( IMap* pMap, const achar* pName /* = "" */ ) const
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_SCENE )
	Assert( pMap );
	
	CEcsWorld&		ecsWorld = ( ( CEcsMap* )pMap )->GetEcsWorld();
	if ( lastUsedEcsPrefabIdx == INVALID_INDEX || ecsPrefabs[lastUsedEcsPrefabIdx].pEcsMap != pMap )
	{
		// Try to find an already created prefab
		uint32	foundEcsPrefabIdx = INVALID_INDEX;
		for ( uint32 ecsPrefabIdx = 0, numEcsPrefabs = ( uint32 )ecsPrefabs.size(); ecsPrefabIdx < numEcsPrefabs; ++ecsPrefabIdx )
		{
			const ecsPrefab_t&	ecsPrefab = ecsPrefabs[ecsPrefabIdx];
			if ( ecsPrefab.pEcsMap == pMap )
			{
				foundEcsPrefabIdx = ecsPrefabIdx;
				break;
			}
		}

		// Create a new ECS prefab if it wasn't found or isn't valid
		if ( foundEcsPrefabIdx == INVALID_INDEX || !ecsWorld.IsValidEntity( ecsPrefabs[foundEcsPrefabIdx].ecsEntity ) )
		{
			lastUsedEcsPrefabIdx = CreateEcsPrefab( ( CEcsMap* )pMap, S_Sprintf( "ecs_prefab_%s", guid.AsString().c_str() ).c_str(), foundEcsPrefabIdx );
		}
	}

	// Create an ECS entity
	const ecsPrefab_t&		ecsPrefab = ecsPrefabs[lastUsedEcsPrefabIdx];
	return new CEcsEntity( ecsWorld.CreateEntity( pName, ecsPrefab.ecsEntity ), ecsPrefab.pEcsMap );
}