#include "pch_game_shared.h"
#include "game/shared/game.h"
#include "game/shared/ecs/ecs_core.h"
#include "game/shared/ecs/ecs_common.gen.h"
#include "game/shared/ecs/ecs_movement.gen.h"
#include "game/shared/ecs/ecs_render.gen.h"
#include "game/shared/ecs/ecs_camera.gen.h"

/*
==================
CEcsWorld::Reset
==================
*/
void CEcsWorld::Reset( bool bAutoInit /* = true */ )
{
	flecsWorld.reset();
	if ( bAutoInit )
	{
		EcsInitWorld( *this );
	}
}

/*
==================
EcsInitModules_Gameframework
==================
*/
void EcsInitModules_Gameframework()
{
	EcsInitReflection_Common();
	EcsInitReflection_Render();
	EcsInitReflection_Movement();
	EcsInitReflection_Camera();
}

/*
==================
EcsInitWorld_GameframeworkOnly
==================
*/
void EcsInitWorld_GameframeworkOnly( CEcsWorld& ecsWorld )
{
	ecsWorld.RegisterModule<ecsModuleCommon_t>();
	ecsWorld.RegisterModule<ecsModuleRender_t>();
	ecsWorld.RegisterModule<ecsModuleMovement_t>();
	ecsWorld.RegisterModule<ecsModuleCamera_t>();

	ecsWorld.SetResource( ecsResourceWindowMgr_t{ g_pWindowMgr } );
	ecsWorld.SetResource( ecsResourceStudioRender_t{ g_pStudioRender } );
}
