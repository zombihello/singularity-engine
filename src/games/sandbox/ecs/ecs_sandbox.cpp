#include "pch_sandbox.h"
#include "games/sandbox/ecs/ecs_testdraw.gen.h"

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