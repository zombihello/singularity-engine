#include "pch_game_shared.h"
#include "game/shared/game.h"
#include "game/shared/ecs/ecs_entity.h"
#include "game/shared/ecs/ecs_map.h"

/*
==================
CEcsEntity::CEcsEntity
==================
*/
CEcsEntity::CEcsEntity( ecsEntity_t ecsEntity, CEcsMap* pEcsMap )
	: ecsEntity( ecsEntity )
	, pEcsMap( pEcsMap )
	, pOnMapResetedDelegate( pEcsMap->OnMapReseted()->AddFunc( &CEcsEntity::OnMapResetedOrUnloaded, this ) )
	, pOnMapUnloadedDelegate( pEcsMap->OnMapUnloaded()->AddFunc( &CEcsEntity::OnMapResetedOrUnloaded, this ) )
{
	Assert( pEcsMap );
}

/*
==================
CEcsEntity::~CEcsEntity
==================
*/
CEcsEntity::~CEcsEntity()
{
	DestroyEcsEntity();
}

/*
==================
CEcsEntity::Destroy
==================
*/
void CEcsEntity::Destroy()
{
	DestroyEcsEntity();
}

/*
==================
CEcsEntity::IsValid
==================
*/
bool CEcsEntity::IsValid() const
{
	return !!pEcsMap;
}

/*
==================
CEcsEntity::OnMapResetedOrUnloaded
==================
*/
void CEcsEntity::OnMapResetedOrUnloaded( void* pUserData, IMap* pMap )
{
	CEcsEntity* pEcsEntity = (CEcsEntity*)pUserData;
	if ( pEcsEntity->pEcsMap == pMap )
	{
		pEcsEntity->DestroyEcsEntity( false );
	}
}

/*
==================
CEcsEntity::DestroyEcsEntity
==================
*/
void CEcsEntity::DestroyEcsEntity( bool bRemoveEcsMapDelegates /* = true */ )
{
	if ( pEcsMap )
	{
		CEcsWorld& ecsWorld = pEcsMap->GetEcsWorld();
		if ( ecsWorld.IsValidEntity( ecsEntity ) )
		{
			ecsWorld.DestroyEntity( ecsEntity );
		}

		if ( bRemoveEcsMapDelegates )
		{
			pEcsMap->OnMapReseted()->RemoveFunc( pOnMapResetedDelegate );
			pEcsMap->OnMapUnloaded()->RemoveFunc( pOnMapUnloadedDelegate );
		}

		pEcsMap				   = NULL;
		pOnMapResetedDelegate  = NULL;
		pOnMapUnloadedDelegate = NULL;
	}
}
