#include "pch_sandbox.h"
#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/istudio_rendercmd.h"
#include "studiorender/studio_vertextypes.h"
#include "studiorender/istudiorender.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialvar.h"
#include "resourcesystem/iresourcesystem.h"
#include "gameframework/game.h"
#include "gameframework/ientity.h"
#include "gameframework/ientitydesc.h"
#include "gameframework/imap.h"
#include "gameframework/ecs/ecs_core.h"
#include "gameframework/ecs/ecs_common.gen.h"
#include "gameframework/ecs/ecs_movement.gen.h"
#include "gameframework/ecs/ecs_camera.gen.h"
#include "games/sandbox/ecs/ecs_testdraw.gen.h"

//-----------------------------------------------------------------------------
// Singularity Sandbox game
//-----------------------------------------------------------------------------
class CSandboxGame : public CGame
{
public:
	// IGame interface
	virtual bool Init( createInterfaceFn_t pFactory ) override;
	virtual void Shutdown() override;

	virtual const achar* GetGameDescription() const override;

private:
	TRefPtr<IEntity>		pQuadEntity;
	TRefPtr<IEntity>		pPlayerEntity;
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
	static CSandboxGame		s_SandboxGame;
	return &s_SandboxGame;
}


/*
==================
CSandboxGame::Init
==================
*/
bool CSandboxGame::Init( createInterfaceFn_t pFactory )
{
	if ( !CGame::Init( pFactory ) )
	{
		return false;
	}

	// Get StudioAPI
	g_pStudioAPI = ( IStudioAPI* )pFactory( STUDIOAPI_INTERFACE_VERSION );
	if ( !g_pStudioAPI )
	{
		return false;
	}

	// Load a quad material
	class CInitQuadHelper
	{
	public:
		static void R_InitQuad( CSandboxGame* pGame )
		{
			studioSimpleElementVertex_t			quadVerteces[] =
			{
				{ { -0.5f, -0.5f, 0.f, 1.f },	{ -1.f, 0.f },	{ 255, 0, 0 } },
				{ { 0.5f, -0.5f , 0.f, 1.f },	{ 0.f, 0.f },	{ 0, 255, 0 } },
				{ { 0.5f, 0.5f , 0.f, 1.f },	{ 0.f, 1.f },	{ 0, 0, 255 } },
				{ { -0.5f, 0.5f , 0.f, 1.f },	{ -1.f, 1.f },	{ 255, 255, 255 } }
			};
			uint16		quadIndices[] = { 0, 1, 2, 2, 3, 0 };

			TResourcePtr<IMaterial>		pMaterial				= g_pResourceSystem->FindOrLoadResource( "materials/nelson", RESOURCE_TYPE_MATERIAL );
			TRefPtr<IStudioAPIBuffer>	pStudioAPIVertexBuffer	= g_pStudioAPI->CreateBuffer( ( byte* )&quadVerteces[0], ARRAYSIZE( quadVerteces ) * sizeof( studioSimpleElementVertex_t ), sizeof( studioSimpleElementVertex_t ), STUDIOAPI_BUFFER_USAGE_FLAG_STATIC | STUDIOAPI_BUFFER_USAGE_FLAG_VERTEX_BUFFER | STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST );
			TRefPtr<IStudioAPIBuffer>	pStudioAPIIndexBuffer	= g_pStudioAPI->CreateBuffer( ( byte* )&quadIndices[0], ARRAYSIZE( quadIndices ) * sizeof( uint16 ), sizeof( uint16 ), STUDIOAPI_BUFFER_USAGE_FLAG_STATIC | STUDIOAPI_BUFFER_USAGE_FLAG_INDEX_BUFFER | STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST );
			Quad().Init(pStudioAPIVertexBuffer, pStudioAPIIndexBuffer, pMaterial);
		}
	};

	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioRenderCmd_InitQuad,
										CSandboxGame*, pGame, this,
										{
											CInitQuadHelper::R_InitQuad( pGame );
										} );
	Studio_FlushRenderCommands();

	// Initialize the ECS world
	extern void EcsInitModules_Sandbox();
	EcsInitModules_Sandbox();

	// Load and set as active a map
	TResourcePtr<IMap>		pMap = g_pResourceSystem->FindOrLoadResource( "maps/test", RESOURCE_TYPE_MAP, RESOURCE_LOAD_FLAG_WITHOUT_DEFAULT );
	if ( !pMap )
	{
		Sys_Error( "Sandbox: Failed to load 'maps/test'" );
		return false;
	}

	SetActiveMap( pMap );
	return true;
}

/*
==================
CSandboxGame::Shutdown
==================
*/
void CSandboxGame::Shutdown()
{
	Quad().Shutdown();
	pQuadEntity			= NULL;
	pPlayerEntity		= NULL;
	g_pStudioAPI		= NULL;
	g_pMaterialSystem	= NULL;
	CGame::Shutdown();
}

/*
==================
CSandboxGame::GetGameDescription
==================
*/
const achar* CSandboxGame::GetGameDescription() const
{
	return "Singularity Sandbox";
}


/*
==================
CEcsSystemQuadDraw::OnUpdate
==================
*/
void CEcsSystemQuadInit::OnUpdate( CEcsWorld ecsWorld, ecsEntity_t entity, const ecsComponentQuad_t& quad, const ecsResourceStudioRender_t& studioRender )
{
	ecsComponentStudioRenderObject_t					studioRenderObjectComponent;
	studioRenderObjectComponent.pStudioRenderObject		= studioRender.pStudioRender->CreateQuadRenderObject( *quad.pMaterial, quad.pVertexBuffer, quad.pIndexBuffer );
	studioRender.pStudioRender->RegisterObject( studioRenderObjectComponent.pStudioRenderObject );
	ecsWorld.SetComponent( entity, std::move( studioRenderObjectComponent ) );
}