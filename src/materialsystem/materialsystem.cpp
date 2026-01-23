#include "pch_materialsystem.h"
#include "resourcesystem/iresourcesystem.h"
#include "materialsystem/material.h"
#include "materialsystem/materialvar.h"
#include "materialsystem/materialsystem.h"

EXPOSE_SINGLE_INTERFACE( CMaterialSystem, IMaterialSystem, MATERIALSYSTEM_INTERFACE_VERSION );

// Application systems factory. It used for connect materialsystem's submodules (e.g: stdshaders)
createInterfaceFn_t g_pAppSystemFactory = NULL;

/*
==================
CMaterialSystem::Connect
==================
*/
bool CMaterialSystem::Connect( createInterfaceFn_t pFactory )
{
	// Connect Tier1
	if ( !ConnectTier1( pFactory ) )
	{
		return false;
	}
	ConVar_Register();

	// Get the StudioAPI
	g_pStudioAPI = (IStudioAPI*)pFactory( STUDIOAPI_INTERFACE_VERSION );
	if ( !g_pStudioAPI )
	{
		return false;
	}

	// Get the shader manager
	g_pShaderMgr = (IShaderMgr*)pFactory( SHADERMGR_INTERFACE_VERSION );
	if ( !g_pShaderMgr )
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

	g_pAppSystemFactory = pFactory;
	return true;
}

/*
==================
CMaterialSystem::Disconnect
==================
*/
void CMaterialSystem::Disconnect()
{
	g_pResourceSystem	= NULL;
	g_pStudioRender		= NULL;
	g_pShaderMgr		= NULL;
	g_pStudioAPI		= NULL;
	g_pAppSystemFactory = NULL;

	ConVar_Unregister();
	DisconnectTier1();
}

/*
==================
CMaterialSystem::QueryInterface
==================
*/
void* CMaterialSystem::QueryInterface( const char* pInterfaceName )
{
	createInterfaceFn_t pFactory = Sys_GetFactoryThis();
	return pFactory( pInterfaceName );
}

/*
==================
CMaterialSystem::Init
==================
*/
bool CMaterialSystem::Init()
{
	// Initialize the shader manager
	g_pShaderMgr->Init();

	// Initialize all resource factories
	textureFactory.Init();
	materialFactory.Init();
	return true;
}

/*
==================
CMaterialSystem::Shutdown
==================
*/
void CMaterialSystem::Shutdown()
{
	// Shutdown all resource factories
	materialFactory.Shutdown();
	textureFactory.Shutdown();

	// Shutdown the shader manager
	g_pShaderMgr->Shutdown();
}