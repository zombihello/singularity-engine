#include "pch_gameframework.h"
#include "gameframework/game.h"
#include "gameframework/ecs/ecs_core.h"

/*
==================
CEcsWorld::Reset
==================
*/
void CEcsWorld::Reset()
{
	flecsWorld.reset();
	Game()->GetEcsComponentTypes().UnregisterAllTypes();
}