#include "pch_sandbox.h"
#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/istudio_rendercmd.h"
#include "studiorender/studio_vertextypes.h"
#include "studiorender/istudiorender.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialvar.h"
#include "resourcesystem/iresourcesystem.h"
#include "gameframework/igame.h"

/**
 * @ingroup sandbox
 * @brief Singularity Sandbox game
 */
class CSandboxGame : public CGame
{
public:
	/**
	 * @brief Initialize the game DLL
	 *
	 * @param pFactory	Pointer to interface factory
	 * @return Return TRUE if the game is initialized, otherwise returns FALSE
	 */
	virtual bool Init( createInterfaceFn_t pFactory ) override;

	/**
	 * @brief Shutdown the game DLL
	 */
	virtual void Shutdown() override;

	/**
	 * @brief Process one game frame
	 */
	virtual void FrameUpdate() override;

	/**
	 * @brief Get game description
	 * @return Return string describing current module. For example: Singularity Sandbox, Nuclear Frost, etc
	 */
	virtual const achar* GetGameDescription() const override;

private:
	TRefPtr<IStudioAPIBuffer>	pQuadVertexBuffer;	/**< StudioAPI quad vertex buffer */
	TRefPtr<IStudioAPIBuffer>	pQuadIndexBuffer;	/**< StudioAPI quad index buffer */
	TResourcePtr<IMaterial>		pQuadMaterial;		/**< Quad material */
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
	g_pStudioRender->DrawQuad( *pQuadMaterial, pQuadVertexBuffer, pQuadIndexBuffer );
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