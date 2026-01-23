#include "pch_citadel.h"
#include "game/citadel/ecs/ecs_testdraw.gen.h"

/*
==================
EcsInitModules_Sandbox
==================
*/
void EcsInitModules_Sandbox()
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
