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
{}

/*
==================
CEcsEntityDesc::Init
==================
*/
void CEcsEntityDesc::Init( const CSENTCompiledEntityDescDoc& sentCompiledDoc )
{
	PROFILE_SCOPE()
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
ecsEntity_t CEcsEntityDesc::CreateEcsPrefab( const achar* pName ) const
{
	PROFILE_SCOPE()
	CEcsWorld&	ecsWorld = Game()->GetEcsWorld();
	ecsEntity_t ecsPrefab = ecsWorld.CreatePrefab( pName );
	for ( uint32 componentIdx = 0, numComponents = ( uint32 )ecsComponentFactories.size(); componentIdx < numComponents; ++componentIdx )
	{
		IEcsComponentFactory*	pEcsComponentFactory = ecsComponentFactories[componentIdx];
		pEcsComponentFactory->Create( ecsWorld, ecsPrefab );
	}

	return ecsPrefab;
}

/*
==================
CEcsEntityDesc::Clear
==================
*/
void CEcsEntityDesc::Clear()
{
	PROFILE_SCOPE()
	CEcsWorld&	ecsWorld = Game()->GetEcsWorld();
	if ( ecsWorld.IsValidEntity( ecsPrefab ) )
	{
		ecsWorld.DestroyEntity( ecsPrefab );
	}
	ecsComponentFactories.clear();
}

/*
==================
CEcsEntityDesc::Create
==================
*/
IEntity* CEcsEntityDesc::Create( const achar* pName /* = "" */ ) const
{
	PROFILE_SCOPE()
	CEcsWorld&		ecsWorld = Game()->GetEcsWorld();
	if ( !ecsWorld.IsValidEntity( ecsPrefab ) )
	{
		ecsPrefab = CreateEcsPrefab( "" );
	}
	return new CEcsEntity( ecsWorld.CreateEntity( pName, ecsPrefab ) );
}