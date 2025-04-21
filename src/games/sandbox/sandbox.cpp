#include "pch_sandbox.h"
#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/istudio_rendercmd.h"
#include "studiorender/studio_vertextypes.h"
#include "studiorender/istudiorender.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialvar.h"
#include "resourcesystem/iresourcesystem.h"
#include "gameframework/igame.h"
#include "gameframework/ecs/ecs.h"
#include "sandbox/ecs/ecs_testdraw.gen.h"

//-----------------------------------------------------------------------------
// Singularity Sandbox game
//-----------------------------------------------------------------------------
class CSandboxGame : public CGame
{
public:
	// IGame interface
	virtual bool Init( createInterfaceFn_t pFactory ) override;
	virtual void Shutdown() override;

	virtual void FrameUpdate() override;

	virtual const achar* GetGameDescription() const override;

private:
	TRefPtr<IStudioAPIBuffer>	pQuadVertexBuffer;
	TRefPtr<IStudioAPIBuffer>	pQuadIndexBuffer;
	TResourcePtr<IMaterial>		pQuadMaterial;
	CEcsWorld					ecsWorld;
};

EXPOSE_SINGLE_INTERFACE( CSandboxGame, IGame, GAME_INTERFACE_VERSION );
EXPOSE_SINGLE_INTERFACE( CGameAppSystems, IGameAppSystems, GAME_APPSYSTEMS_INTERFACE_VERSION );


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

	// Get StudioRender
	g_pStudioRender = ( IStudioRender* )pFactory( STUDIORENDER_INTERFACE_VERSION );
	if ( !g_pStudioRender )
	{
		return false;
	}

	// Get the resource system
	g_pResourceSystem = ( IResourceSystem* )pFactory( RESOURCESYSTEM_INTERFACE_VERSION );
	if ( !g_pResourceSystem )
	{
		return false;
	}
	
	// Load a quad material
	pQuadMaterial = g_pResourceSystem->FindOrLoadResource( "materials/nelson", RESOURCE_TYPE_MATERIAL );

	ecsWorld.RegisterModule<ecsTestDrawModule_t>();

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

			pGame->pQuadVertexBuffer = g_pStudioAPI->CreateBuffer( ( byte* )&quadVerteces[0], ARRAYSIZE( quadVerteces ) * sizeof( studioSimpleElementVertex_t ), sizeof( studioSimpleElementVertex_t ), STUDIOAPI_BUFFER_USAGE_FLAG_STATIC | STUDIOAPI_BUFFER_USAGE_FLAG_VERTEX_BUFFER | STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST );
			pGame->pQuadIndexBuffer = g_pStudioAPI->CreateBuffer( ( byte* )&quadIndices[0], ARRAYSIZE( quadIndices ) * sizeof( uint16 ), sizeof( uint16 ), STUDIOAPI_BUFFER_USAGE_FLAG_STATIC | STUDIOAPI_BUFFER_USAGE_FLAG_INDEX_BUFFER | STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST );
		}
	};

	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioRenderCmd_InitQuad,
										CSandboxGame*, pGame, this,
										{
											CInitQuadHelper::R_InitQuad( pGame );
										} );

	Studio_FlushRenderCommands();
	
	ecsQuadComponent_t				quadComponent;
	quadComponent.pVertexBuffer		= pQuadVertexBuffer;
	quadComponent.pIndexBuffer		= pQuadIndexBuffer;
	quadComponent.pMaterial			= pQuadMaterial;
	ecsEntity_t						quadEntity = ecsWorld.CreateEntity( "quad" );
	ecsWorld.SetComponent( quadEntity, std::move( quadComponent ) );
	ecsWorld.AddComponent<ecsDrawableComponent_t>( quadEntity );
	return true;
}

/*
==================
CSandboxGame::Shutdown
==================
*/
void CSandboxGame::Shutdown()
{
	CGame::Shutdown();
	ecsWorld.Reset();
	g_pStudioAPI		= NULL;
	g_pStudioRender		= NULL;
	g_pMaterialSystem	= NULL;
}

/*
==================
CSandboxGame::FrameUpdate
==================
*/
void CSandboxGame::FrameUpdate()
{
	ecsWorld.Update( 0.f );
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
CSandboxGame::GetGameDescription
==================
*/
void CEcsQuadDrawSystem::OnUpdate( CEcsWorld ecsWorld, ecsEntity_t entity, const ecsQuadComponent_t& quad )
{
	g_pStudioRender->DrawQuad( *quad.pMaterial, quad.pVertexBuffer, quad.pIndexBuffer );
}