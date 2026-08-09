#include "pch_citadel.h"
#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/istudio_rendercmd.h"
#include "studiorender/istudiorender.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialvar.h"
#include "modelsystem/modeltypes.h"
#include "resourcesystem/iresourcesystem.h"
#include "game/shared/game.h"
#include "game/ientity.h"
#include "game/ientitydesc.h"
#include "game/imap.h"
#include "game/shared/ecs/ecs_core.h"
#include "game/shared/ecs/ecs_common.gen.h"
#include "game/citadel/ecs/ecs_testdraw.gen.h"
#include "tier1/cmdlink.h"

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
};

EXPOSE_INTERFACE_FN( Game, IGame, GAME_INTERFACE_VERSION );
EXPOSE_SINGLE_INTERFACE( CGameAppSystems, IGameAppSystems, GAME_APPSYSTEMS_INTERFACE_VERSION );

/*
==================
test_unload_texture_nelson
==================
*/
CONSOLE_COMMAND( test_unload_texture_nelson, "Unload texture 'materials/nelson' to test uncache case while rendering", CMD_FLAG_NONE )
{
	IResourceTypeMgr*	   pTextureMgr = g_pResourceSystem->GetResourceManagerForType<ITexture>();
	CResourcePtr<ITexture> pTexture	   = pTextureMgr->FindResource( "materials/nelson", false );
	if ( pTexture )
	{
		pTexture.Uncache();
	}
}

/*
==================
test_load_texture_nelson
==================
*/
CONSOLE_COMMAND( test_load_texture_nelson, "Load texture 'materials/nelson' to test cache case while rendering", CMD_FLAG_NONE )
{
	IResourceTypeMgr*	   pTextureMgr = g_pResourceSystem->GetResourceManagerForType<ITexture>();
	CResourcePtr<ITexture> pTexture	   = pTextureMgr->FindResource( "materials/nelson", false );
	if ( pTexture )
	{
		pTexture.Cache();
	}
}

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

	// Create a quad model
	IResourceTypeMgr*	pMaterialsMgr  = g_pResourceSystem->GetResourceManagerForType<IMaterial>();
	IResourceTypeMgr*	pModelsMgr	   = g_pResourceSystem->GetResourceManagerForType<IModel>();
	modelSimpleVertex_t quadVerteces[] = {
		{ { -0.5f, -0.5f, 0.f, 1.f }, { -1.f, 0.f }, { 255, 0, 0 } },
		{ { 0.5f, -0.5f, 0.f, 1.f }, { 0.f, 0.f }, { 0, 255, 0 } },
		{ { 0.5f, 0.5f, 0.f, 1.f }, { 0.f, 1.f }, { 0, 0, 255 } },
		{ { -0.5f, 0.5f, 0.f, 1.f }, { -1.f, 1.f }, { 255, 255, 255 } }
	};
	uint16					quadIndices[] = { 0, 1, 2, 2, 3, 0 };
	CResourcePtr<IMaterial> pQuadMaterial = pMaterialsMgr->LoadResource( "materials/nelson" );
	modelSurface_t			quadSurface	  = {};
	quadSurface.numIndices				  = ARRAYSIZE( quadIndices );

	CResourcePtr<IModel> pQuadModel		  = pModelsMgr->CreateResource( "quad" );
	modelInitialData_t	 modelInitialData = {};
	modelInitialData.vertexType			  = MODEL_VERTEXTYPE_SIMPLE;
	modelInitialData.indexType			  = MODEL_INDEXTYPE_UINT16;
	modelInitialData.sizeVertices		  = ARRAYSIZE( quadVerteces ) * sizeof( modelSimpleVertex_t );
	modelInitialData.sizeIndices		  = ARRAYSIZE( quadIndices ) * sizeof( uint16 );
	modelInitialData.numMaterials		  = 1;
	modelInitialData.numSurfaces		  = 1;
	modelInitialData.pVertices			  = (byte*)&quadVerteces[0];
	modelInitialData.pIndices			  = (byte*)&quadIndices[0];
	modelInitialData.pMaterials			  = &pQuadMaterial;
	modelInitialData.pSurfaces			  = &quadSurface;
	pQuadModel->Init( modelInitialData );
	Quad().Init( pQuadModel, pQuadMaterial );

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
CEcsSystemQuadInit::OnUpdate
==================
*/
void CEcsSystemQuadInit::OnUpdate( CEcsWorld ecsWorld, ecsEntity_t entity, const ecsComponentQuad_t& quad, ecsResourceStudioScene_t& studioScene )
{
	// TODO BS yehor.pohuliaka - CIT-81 Implement observer/system to free a studio entity when the ecs entity has been destroyed
	studioEntityParams_t	   studioEntityParams	 = {};
	ecsComponentStudioEntity_t studioEntityComponent = {};
	studioEntityParams.pModel						 = quad.pModel;
	studioEntityParams.localToWorld					 = g_matrix43Identity;
	studioEntityComponent.id						 = studioScene.pStudioScene->AddEntity( studioEntityParams );
	ecsWorld.SetComponent( entity, eastl::move( studioEntityComponent ) );
}

/*
==================
CEcsSystemModelInit::OnUpdate
==================
*/
void CEcsSystemModelInit::OnUpdate( CEcsWorld ecsWorld, ecsEntity_t entity, const ecsComponentModel_t& model, ecsResourceStudioScene_t& studioScene )
{
	// TODO BS yehor.pohuliaka - CIT-81 Implement observer/system to free a studio entity when the ecs entity has been destroyed
	static uint32			   modelIdx				 = 0;
	studioEntityParams_t	   studioEntityParams	 = {};
	ecsComponentStudioEntity_t studioEntityComponent = {};
	studioEntityParams.pModel						 = model.pModel;
	studioEntityParams.localToWorld					 = S_MatrixTranslate<matrix4x4_t>( vector3_t( 0.f, -200.f * modelIdx, 0.f ) ) * S_QuaternionToMatrix<matrix4x4_t>( S_AnglesToQuaternionYZX( vector3_t( 0, 90.f, 0 ) ) );
	studioEntityComponent.id						 = studioScene.pStudioScene->AddEntity( studioEntityParams );
	ecsWorld.SetComponent( entity, eastl::move( studioEntityComponent ) );
	++modelIdx;
}
