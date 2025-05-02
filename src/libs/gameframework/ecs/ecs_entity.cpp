#include "pch_gameframework.h"
#include "gameframework/game.h"
#include "gameframework/ecs/ecs_entity.h"

/*
==================
CEcsEntity::CEcsEntity
==================
*/
CEcsEntity::CEcsEntity( ecsEntity_t ecsEntity )
	: ecsEntity( ecsEntity )
{}

/*
==================
CEcsEntity::~CEcsEntity
==================
*/
CEcsEntity::~CEcsEntity()
{
	CEcsWorld&		ecsWorld = Game()->GetEcsWorld();
	if ( ecsWorld.IsValidEntity( ecsEntity ) )
	{
		ecsWorld.DestroyEntity( ecsEntity );
	}
}