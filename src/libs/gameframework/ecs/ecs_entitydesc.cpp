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
CEcsEntityDesc::RecreateEcsArchetypeEntity
==================
*/
void CEcsEntityDesc::RecreateEcsArchetypeEntity() const
{
	CEcsWorld&		ecsWorld = Game()->GetEcsWorld();
	if ( !ecsWorld.IsValidEntity( ecsArchetypeEntity ) )
	{
		ecsArchetypeEntity = CreateEcsEntity( "" );
	}
}

/*
==================
CEcsEntityDesc::CreateEcsEntity
==================
*/
ecsEntity_t CEcsEntityDesc::CreateEcsEntity( const achar* pName ) const
{
	CEcsWorld&		ecsWorld	= Game()->GetEcsWorld();
	ecsEntity_t		ecsEntity	= ecsWorld.CreateEntity( pName );
	for ( uint32 componentIdx = 0, numComponents = ( uint32 ) ecsComponentFactories.size(); componentIdx < numComponents; ++componentIdx )
	{
		IEcsComponentFactory* pEcsComponentFactory = ecsComponentFactories[componentIdx];
		pEcsComponentFactory->Create( ecsWorld, ecsEntity );
	}

	return ecsEntity;
}

/*
==================
CEcsEntityDesc::Clear
==================
*/
void CEcsEntityDesc::Clear()
{
	CEcsWorld&	ecsWorld = Game()->GetEcsWorld();
	if ( ecsWorld.IsValidEntity( ecsArchetypeEntity ) )
	{
		ecsWorld.DestroyEntity( ecsArchetypeEntity );
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
	CEcsWorld&		ecsWorld = Game()->GetEcsWorld();
	RecreateEcsArchetypeEntity();
	return new CEcsEntity( ecsWorld.CloneEntity( ecsArchetypeEntity, pName ) );
}