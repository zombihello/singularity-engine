#include "pch_citadel.h"
#include "game/citadel/ecs/ecs_testdraw.gen.h"

/*
==================
EcsInitModules_Citadel
==================
*/
void EcsInitModules_Citadel()
{
	EcsInitReflection_TestDraw();
}

/*
==================
EcsInitWorld_GameOnly
==================
*/
void EcsInitWorld_GameOnly( CEcsWorld& ecsWorld )
{
	ecsWorld.RegisterModule<ecsModuleTestDraw_t>();
}
