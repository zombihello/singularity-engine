#include "pch_game_shared.h"
#include "game/shared/basegame.h"
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
	, onMapResetedHandle( pEcsMap->OnMapReseted()->Subscribe( &CEcsEntity::OnMapResetedOrUnloaded, this ) )
	, onMapUnloadedHandle( pEcsMap->OnMapUnloaded()->Subscribe( &CEcsEntity::OnMapResetedOrUnloaded, this ) )
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
		pEcsEntity->DestroyEcsEntity();
	}
}

/*
==================
CEcsEntity::DestroyEcsEntity
==================
*/
void CEcsEntity::DestroyEcsEntity()
{
	if ( pEcsMap )
	{
		CEcsWorld& ecsWorld = pEcsMap->GetEcsWorld();
		if ( ecsWorld.IsValidEntity( ecsEntity ) )
		{
			ecsWorld.DestroyEntity( ecsEntity );
		}

		if ( onMapResetedHandle != INVALID_HANDLE )
		{
			pEcsMap->OnMapReseted()->Unsubscribe( onMapResetedHandle );
			onMapResetedHandle = INVALID_HANDLE;
		}
		if ( onMapUnloadedHandle != INVALID_HANDLE )
		{
			pEcsMap->OnMapUnloaded()->Unsubscribe( onMapUnloadedHandle );
			onMapUnloadedHandle = INVALID_HANDLE;
		}

		pEcsMap = NULL;
	}
}
