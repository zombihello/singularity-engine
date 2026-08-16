#include "pch_modelsystem.h"
#include "studiorender/studioapi/istudioapi.h"
#include "resourcesystem/iresourcesystem.h"
#include "modelsystem/vertexfactory_simple.h"
#include "modelsystem/vertexfactory_static.h"
#include "modelsystem/modelsystem.h"

CModelSystem g_modelSystem;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CModelSystem, IModelSystem, MODELSYSTEM_INTERFACE_VERSION, g_modelSystem );

/*
==================
CModelSystem::CModelSystem
==================
*/
CModelSystem::CModelSystem()
	: onStudioRenderBeginFrameHandle( INVALID_HANDLE )
{
}

/*
==================
CModelSystem::Connect
==================
*/
bool CModelSystem::Connect( createInterfaceFn_t pFactory )
{
	// Connect Tier1
	PROFILER_SCOPE_FUNC();
	if ( !ConnectTier1( pFactory ) )
	{
		return false;
	}
	LinkCmds();
	LinkCVars();

	// Get the studio api
	g_pStudioAPI = (IStudioAPI*)pFactory( STUDIOAPI_INTERFACE_VERSION );
	if ( !g_pStudioAPI )
	{
		return false;
	}

	// Get the studiorender
	g_pStudioRender = (IStudioRender*)pFactory( STUDIORENDER_INTERFACE_VERSION );
	if ( !g_pStudioRender )
	{
		return false;
	}

	// Get the resource system
	g_pResourceSystem = (IResourceSystem*)pFactory( RESOURCESYSTEM_INTERFACE_VERSION );
	if ( !g_pResourceSystem )
	{
		return false;
	}

	// We are done
	g_pModelSystem = this;
	return true;
}

/*
==================
CModelSystem::Disconnect
==================
*/
void CModelSystem::Disconnect()
{
	PROFILER_SCOPE_FUNC();
	UnlinkCVars();
	UnlinkCmds();
	DisconnectTier1();

	g_pStudioAPI	  = NULL;
	g_pStudioRender	  = NULL;
	g_pResourceSystem = NULL;
	g_pModelSystem	  = NULL;
}

/*
==================
CModelSystem::Init
==================
*/
bool CModelSystem::Init()
{
	// Initialize all global resources
	PROFILER_SCOPE_FUNC();
	CStudioGlobalRenderResources::InitResources();

	// Register model resource type
	IResourceTypeMgr* pModelsMgr = g_pResourceSystem->InstallResourceManagerForType<CModel>();
	pModelsMgr->RegisterResourceFactory( &modelFactory );
	pModelsMgr->RegisterResourceLoader( &modelLoader );

	// Set a default resource for models
	CResourcePtr<CModel> pDefaultModel = pModelsMgr->LoadResource( "__default", "//core/models/default" );
	if ( !pDefaultModel )
	{
		pDefaultModel = pModelsMgr->CreateResource( "__default" );
	}
	pModelsMgr->SetDefaultResource( pDefaultModel );

	// Subscribe to the begin draw frame event
	onStudioRenderBeginFrameHandle = g_pStudioRender->OnBeginFrame()->Subscribe( &CModelSystem::OnStudioRenderBeginFrame, this );
	return true;
}

/*
==================
CModelSystem::OnStudioRenderBeginFrame
==================
*/
void CModelSystem::OnStudioRenderBeginFrame( void* pUserData )
{
	// Update each pending model
	PROFILER_SCOPE_FUNC();
	CModelSystem* pThis = (CModelSystem*)pUserData;
	for ( uint32 index = 0, count = (uint32)pThis->pendingUpdateModels.size(); index < count; ++index )
	{
		pThis->pendingUpdateModels[index]->UpdateStudioResource();
	}
	pThis->pendingUpdateModels.clear();
}

/*
==================
CModelSystem::Shutdown
==================
*/
void CModelSystem::Shutdown()
{
	// Unsubscribe from the begin draw frame event
	PROFILER_SCOPE_FUNC();
	if ( onStudioRenderBeginFrameHandle != INVALID_HANDLE )
	{
		g_pStudioRender->OnBeginFrame()->Unsubscribe( onStudioRenderBeginFrameHandle );
		onStudioRenderBeginFrameHandle = INVALID_HANDLE;
	}
	pendingUpdateModels.clear();

	// Unregister model resource type
	g_pResourceSystem->RemoveResourceManagerForType<CModel>();

	// Release all global resources
	CStudioGlobalRenderResources::ReleaseResources();
}

/*
==================
CModelSystem::CreateVertexFactory
==================
*/
CRefPtr<IVertexFactory> CModelSystem::CreateVertexFactory( modelVertexType_t vertexType, const char* pDebugName /* = "" */ ) const
{
	PROFILER_SCOPE_FUNC();
	switch ( vertexType )
	{
	case MODEL_VERTEXTYPE_SIMPLE: return new CVertexFactorySimple( pDebugName );
	case MODEL_VERTEXTYPE_STATIC: return new CVertexFactoryStatic( pDebugName );
	default:
		AssertMsg( false, "Unknown vertex type 0x%X", vertexType );
		return NULL;
	}
}
