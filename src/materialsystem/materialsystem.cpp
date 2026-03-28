#include "pch_materialsystem.h"
#include "resourcesystem/iresourcesystem.h"
#include "materialsystem/texture.h"
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
	PROFILER_SCOPE_FUNC();

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
	PROFILER_SCOPE_FUNC();
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
	PROFILER_SCOPE_FUNC();

	// Initialize the shader manager
	g_pShaderMgr->Init();

	// Register texture resource type
	IResourceTypeMgr* pTexturesMgr = g_pResourceSystem->InstallResourceManagerForType<CTexture>();
	pTexturesMgr->RegisterResourceFactory( &textureFactory );
	pTexturesMgr->RegisterResourceLoader( &textureLoader );

	// Register material texture type
	IResourceTypeMgr* pMaterialsMgr = g_pResourceSystem->InstallResourceManagerForType<CMaterial>();
	pMaterialsMgr->RegisterResourceFactory( &materialFactory );
	pMaterialsMgr->RegisterResourceLoader( &materialLoader );

	// Set a default resource for textures
	CResourcePtr<CTexture> pDefaultTexture = pTexturesMgr->LoadResource( "//core/materials/default" );
	if ( !pDefaultTexture )
	{
		uint8			data[4] = { 0xFF, 0x00, 0xFF, 0xFF };
		textureMipMap_t mipmap0 = {};
		mipmap0.sizeX			= 1;
		mipmap0.sizeY			= 1;
		mipmap0.sizeZ			= 1;

		studioAPISamplerCreateInfo_t studioAPISamplerInfo = {};
		studioAPISamplerInfo.filer						  = STUDIOAPI_SAMPLER_FILTER_POINT;
		studioAPISamplerInfo.addressModeU				  = STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
		studioAPISamplerInfo.addressModeV				  = STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
		studioAPISamplerInfo.addressModeW				  = STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP;
		studioAPISamplerInfo.minLod						  = S_MinValue<float>();
		studioAPISamplerInfo.maxLod						  = S_MaxValue<float>();

		pDefaultTexture = pTexturesMgr->CreateResource( "__default" );
		pDefaultTexture->Init( STUDIOAPI_TEXTURE_TYPE_2D, STUDIOAPI_PIXEL_FORMAT_RGBA8, 1, &mipmap0, 1, studioAPISamplerInfo, data, ARRAYSIZE( data ) );
	}
	pTexturesMgr->SetDefaultResource( pDefaultTexture );

	// Set a default resource for materials
	CResourcePtr<CMaterial> pDefaultMaterial = pMaterialsMgr->LoadResource( "//core/materials/default" );
	if ( !pDefaultMaterial )
	{
		pDefaultMaterial = pMaterialsMgr->CreateResource( "__default" );
		pDefaultMaterial->SetShader( "wireframe" );
	}
	pMaterialsMgr->SetDefaultResource( pDefaultMaterial );

	return true;
}

/*
==================
CMaterialSystem::Shutdown
==================
*/
void CMaterialSystem::Shutdown()
{
	PROFILER_SCOPE_FUNC();

	// Unregister all material resource types
	g_pResourceSystem->RemoveResourceManagerForType<CMaterial>();
	g_pResourceSystem->RemoveResourceManagerForType<CTexture>();

	// Shutdown the shader manager
	g_pShaderMgr->Shutdown();
}
