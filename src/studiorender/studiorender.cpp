#include "pch_studiorender.h"
#include "filesystem/ifilesystem.h"
#include "materialsystem/ishadermgr.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "studiorender/studio_renderthread.h"
#include "studiorender/studio_viewport.h"
#include "studiorender/studio_renderpipelineset.h"
#include "studiorender/studio_vertexdeclarations.h"
#include "studiorender/studiorender.h"

CStudioRender		g_StudioRender;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CStudioRender, IStudioRender, STUDIORENDER_INTERFACE_VERSION, g_StudioRender );


/*
==================
CStudioRender::CStudioRender
==================
*/
CStudioRender::CStudioRender()
	: studioAPIHandle( INVALID_DLL_HANDLE )
	, pStudioAPIFactory( nullptr )
	, pAppSystemFactory( nullptr )
{}

/*
==================
CStudioRender::Connect
==================
*/
bool CStudioRender::Connect( createInterfaceFn_t pFactory )
{
	// Connect StdLib
	if ( !ConnectStdLib( pFactory ) )
	{
		return false;
	}

	// Before connect studiorender must be Studio API loaded by CStudioRender::SetStudioAPI
	if ( !pStudioAPIFactory )
	{
		Warning( "StudioRender: The studiorender requires Studio API to run!" );
		return false;
	}

	// Get Studio API
	g_pStudioAPI = ( IStudioAPI* )pStudioAPIFactory( STUDIOAPI_INTERFACE_VERSION );
	if ( !g_pStudioAPI )
	{
		return false;
	}

	// Get shader manager
	g_pShaderMgr = ( IShaderMgr* )pFactory( SHADERMGR_INTERFACE_VERSION );
	if ( !g_pShaderMgr )
	{
		return false;
	}

	pAppSystemFactory	= pFactory;
	g_pStudioRender		= this;
	return true;
}

/*
==================
CStudioRender::Disconnect
==================
*/
void CStudioRender::Disconnect()
{
	// Disconnect StdLib
	DisconnectStdLib();
	g_pStudioAPI		= NULL;
	pAppSystemFactory	= NULL;
	g_pStudioRender		= NULL;
	g_pShaderMgr		= NULL;
}

/*
==================
CStudioRender::QueryInterface
==================
*/
void* CStudioRender::QueryInterface( const achar* pInterfaceName )
{
	// Returns various interfaces supported by the Studio API dll
	void*	pInterface = NULL;
	if ( pStudioAPIFactory )
	{
		pInterface = pStudioAPIFactory( pInterfaceName );
	}

	// Otherwise if not found look in our factory
	if ( !pInterface )
	{
		createInterfaceFn_t		pFactory = Sys_GetFactoryThis();
		pInterface = pFactory( pInterfaceName );
	}

	return pInterface;
}

/*
==================
CStudioRender::Init
==================
*/
bool CStudioRender::Init()
{
	// Register cvars
	ConVar_Register();
	
	// Initialize Studio API
	if ( !g_pStudioAPI->Init( pAppSystemFactory ) )
	{
		Warning( "StudioRender: Failed to init Studio API '%s'", studioAPIDLLName.c_str() );
		DestroyStudioAPI();
		return false;
	}

	// Initialize all global resources
	CStudioGlobalRenderResources::InitResources();

	// Initialize the present pass
	presentRenderPass.Init();

	// Start the render thread
	Studio_StartRenderThread();

	// We are done!
	return true;
}

/*
==================
CStudioRender::Shutdown
==================
*/
void CStudioRender::Shutdown()
{
	// Stop the render thread
	Studio_StopRenderThread();
	
	// Shutdown the present pass
	presentRenderPass.Shutdown();

	// Release all global resources
	CStudioGlobalRenderResources::ReleaseResources();

	// Destroy Studio API and unregister cvars
	DestroyStudioAPI();
	ConVar_Unregister();
}

/*
==================
CStudioRender::SetStudioAPI
==================
*/
void CStudioRender::SetStudioAPI( const achar* pStudioAPIDLL )
{
	// We cannot set the studio API twice
	if ( pStudioAPIFactory )
	{
		Error( "StudioRender: Cannot set the Studio API twice!" );
		return;
	}

	// Set default StudioAPI module if pStudioAPIDLL isn't valid
	if ( !pStudioAPIDLL )
	{
		pStudioAPIDLL = "studioapi_vk" DLL_EXT_STRING;
	}

	// Load Studio API module
	studioAPIDLLName	= pStudioAPIDLL;
	pStudioAPIFactory	= CreateStudioAPI( pStudioAPIDLL );
	if ( !pStudioAPIFactory )
	{
		DestroyStudioAPI();
	}
}

/*
==================
CStudioRender::DrawQuad
==================
*/
void CStudioRender::DrawQuad( IMaterial* pMaterial, IStudioAPIBuffer* pVertexBuffer, IStudioAPIBuffer* pIndexBuffer )
{
	UNIQUE_RENDER_COMMAND_THREEPARAMETER( CStudioRenderCmd_DrawQuad,
										  TRefPtr<IMaterial>, pMaterial, pMaterial,
										  TRefPtr<IStudioAPIBuffer>, pVertexBuffer, pVertexBuffer,
										  TRefPtr<IStudioAPIBuffer>, pIndexBuffer, pIndexBuffer,
										  {
												g_StudioRender.presentRenderPass.R_SubmitQuad( pMaterial, pVertexBuffer, pIndexBuffer );
										} );
}

/*
==================
CStudioRender::CreateViewport
==================
*/
IStudioViewport* CStudioRender::CreateViewport() const
{
	return new CStudioViewport();
}

/*
==================
CStudioRender::CreateRenderPipelineSet
==================
*/
IStudioRenderPipelineSet* CStudioRender::CreateRenderPipelineSet() const
{
	return new CStudioRenderPipelineSet();
}

/*
==================
CStudioRender::BeginFrame
==================
*/
void CStudioRender::BeginFrame()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	Assert( Sys_IsInMainThread() );
}

/*
==================
CStudioRender::EndFrame
==================
*/
void CStudioRender::EndFrame()
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	Assert( Sys_IsInMainThread() );
}

/*
==================
CStudioRender::R_DrawFrame
==================
*/

void CStudioRender::R_DrawFrame( CStudioViewport* pViewport )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	Assert( Studio_IsInRenderThread() );
	presentRenderPass.R_DrawPass( pViewport );
}

/*
==================
CStudioRender::GetCommandBuffer
==================
*/
IStudioCmdBuffer* CStudioRender::GetCommandBuffer() const
{
	return &g_StudioCmdBuffer;
}

/*
==================
CStudioRender::IsInRenderThreads
==================
*/
bool CStudioRender::IsInRenderThread() const
{
	return Studio_IsInRenderThread();
}

/*
==================
CStudioRender::StudioAPI_Load
==================
*/
createInterfaceFn_t CStudioRender::CreateStudioAPI( const achar* pStudioAPIDLL )
{
	// Do nothing if pStudioAPIDLL isn't valid
	if ( !pStudioAPIDLL )
	{
		return NULL;
	}

	// Clean up the old Studio API
	DestroyStudioAPI();

	// Load the new Studio API
	studioAPIHandle = Sys_DLL_LoadModule( pStudioAPIDLL );
	if ( !studioAPIHandle )
	{
		Warning( "StudioRender: Failed to load Studio API '%s'", pStudioAPIDLL );
		return NULL;
	}

	// Get our class factory methods
	createInterfaceFn_t		pFactory = Sys_GetFactory( studioAPIHandle );
	if ( !pFactory )
	{
		Warning( "StudioRender: Failed to get interface factory from '%s'", pStudioAPIDLL );
		DestroyStudioAPI();
		return NULL;
	}

	// We are done!
	Msg( "StudioRender: Studio API '%s' loaded", pStudioAPIDLL );
	return pFactory;
}

/*
==================
CStudioRender::DestroyStudioAPI
==================
*/
void CStudioRender::DestroyStudioAPI()
{
	if ( studioAPIHandle )
	{
		// Shutdown Studio API if pointer is valid
		if ( g_pStudioAPI )
		{
			g_pStudioAPI->Shutdown();
		}

		Msg( "StudioRender: Studio API '%s' unloaded", studioAPIDLLName.c_str() );
		Sys_DLL_UnloadModule( studioAPIHandle );

		studioAPIDLLName.clear();
		studioAPIHandle		= NULL;
		pStudioAPIFactory	= NULL;
		g_pStudioAPI		= NULL;
	}
}