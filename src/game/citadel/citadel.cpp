#include "pch_citadel.h"
#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/istudio_rendercmd.h"
#include "studiorender/studio_vertextypes.h"
#include "studiorender/istudiorender.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialvar.h"
#include "resourcesystem/iresourcesystem.h"
#include "game/shared/game.h"
#include "game/ientity.h"
#include "game/ientitydesc.h"
#include "game/imap.h"
#include "game/shared/ecs/ecs_core.h"
#include "game/shared/ecs/ecs_common.gen.h"
#include "game/shared/ecs/ecs_movement.gen.h"
#include "game/shared/ecs/ecs_camera.gen.h"
#include "game/citadel/ecs/ecs_testdraw.gen.h"

//-----------------------------------------------------------------------------
// Citadel game
//-----------------------------------------------------------------------------
class CCitadelGame : public CGame
{
public:
	// IAppSystem interfaces
	// Here's where the app systems get to learn about each other
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// Initialize and shutdown
	virtual bool Init() override;
	virtual void Shutdown() override;

	// IGame interfaces
	virtual const char* GetGameDescription() const override;

private:
	CRefPtr<IEntity> pQuadEntity;
	CRefPtr<IEntity> pPlayerEntity;
};

EXPOSE_INTERFACE_FN( Game, IGame, GAME_INTERFACE_VERSION );
EXPOSE_SINGLE_INTERFACE( CGameAppSystems, IGameAppSystems, GAME_APPSYSTEMS_INTERFACE_VERSION );

/*
==================
Game
==================
*/
CGame* Game()
{
	static CCitadelGame s_SandboxGame;
	return &s_SandboxGame;
}

/*
==================
CCitadelGame::Connect
==================
*/
bool CCitadelGame::Connect( createInterfaceFn_t pFactory )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_GAMELOGIC );
	if ( !CGame::Connect( pFactory ) )
	{
		return false;
	}

	// Get StudioAPI
	g_pStudioAPI = (IStudioAPI*)pFactory( STUDIOAPI_INTERFACE_VERSION );
	if ( !g_pStudioAPI )
	{
		return false;
	}

	return true;
}

/*
==================
CCitadelGame::Disconnect
==================
*/
void CCitadelGame::Disconnect()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_GAMELOGIC );
	g_pStudioAPI = NULL;
	CGame::Disconnect();
}

/*
==================
CCitadelGame::Init
==================
*/
bool CCitadelGame::Init()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_GAMELOGIC );
	if ( !CGame::Init() )
	{
		return false;
	}

	// Load a quad material
	class CInitQuadHelper
	{
	public:
		static void R_InitQuad( CCitadelGame* pGame )
		{
			studioSimpleElementVertex_t quadVerteces[] = { { { -0.5f, -0.5f, 0.f, 1.f }, { -1.f, 0.f }, { 255, 0, 0 } },
														   { { 0.5f, -0.5f, 0.f, 1.f }, { 0.f, 0.f }, { 0, 255, 0 } },
														   { { 0.5f, 0.5f, 0.f, 1.f }, { 0.f, 1.f }, { 0, 0, 255 } },
														   { { -0.5f, 0.5f, 0.f, 1.f }, { -1.f, 1.f }, { 255, 255, 255 } } };
			uint16						quadIndices[]  = { 0, 1, 2, 2, 3, 0 };

			IResourceTypeMgr*		  pMaterialsMgr			 = g_pResourceSystem->GetResourceManagerForType<IMaterial>();
			CResourcePtr<IMaterial>	  pMaterial				 = pMaterialsMgr->LoadResource( "materials/nelson" );
			CRefPtr<IStudioAPIBuffer> pStudioAPIVertexBuffer = g_pStudioAPI->CreateBuffer( (byte*)&quadVerteces[0], ARRAYSIZE( quadVerteces ) * sizeof( studioSimpleElementVertex_t ), sizeof( studioSimpleElementVertex_t ), STUDIOAPI_BUFFER_USAGE_FLAG_STATIC | STUDIOAPI_BUFFER_USAGE_FLAG_VERTEX_BUFFER | STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST );
			CRefPtr<IStudioAPIBuffer> pStudioAPIIndexBuffer	 = g_pStudioAPI->CreateBuffer( (byte*)&quadIndices[0], ARRAYSIZE( quadIndices ) * sizeof( uint16 ), sizeof( uint16 ), STUDIOAPI_BUFFER_USAGE_FLAG_STATIC | STUDIOAPI_BUFFER_USAGE_FLAG_INDEX_BUFFER | STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST );
			Quad().Init( pStudioAPIVertexBuffer, pStudioAPIIndexBuffer, pMaterial );
		}
	};

	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioRenderCmd_InitQuad,
										CCitadelGame*, pGame, this,
										{
											CInitQuadHelper::R_InitQuad( pGame );
										} );
	Studio_FlushRenderCommands();

	// Initialize the ECS world
	extern void EcsInitModules_Citadel();
	EcsInitModules_Citadel();

	// Load a map
	if ( !MapInit( "maps/test" ) )
	{
		Sys_Error( "Failed to load 'maps/test'" );
		return false;
	}
	return true;
}

/*
==================
CCitadelGame::Shutdown
==================
*/
void CCitadelGame::Shutdown()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_GAMELOGIC );
	Quad().Shutdown();
	pQuadEntity	  = NULL;
	pPlayerEntity = NULL;
	CGame::Shutdown();
}

/*
==================
CCitadelGame::GetGameDescription
==================
*/
const char* CCitadelGame::GetGameDescription() const
{
	return "Citadel";
}

/*
==================
CEcsSystemQuadDraw::OnUpdate
==================
*/
void CEcsSystemQuadInit::OnUpdate( CEcsWorld ecsWorld, ecsEntity_t entity, const ecsComponentQuad_t& quad, const ecsResourceStudioRender_t& studioRender )
{
	ecsComponentStudioRenderObject_t studioRenderObjectComponent;
	studioRenderObjectComponent.pStudioRenderObject = studioRender.pStudioRender->CreateQuadRenderObject( quad.pMaterial, quad.pVertexBuffer, quad.pIndexBuffer );
	studioRender.pStudioRender->RegisterObject( studioRenderObjectComponent.pStudioRenderObject );
	ecsWorld.SetComponent( entity, eastl::move( studioRenderObjectComponent ) );
}
