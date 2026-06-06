#include "pch_resourcesystem.h"
#include "tier1/convar.h"
#include "tier1/cmdlink.h"
#include "resourcesystem/resourcesystem.h"

CResourceSystem g_resourceSystem;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CResourceSystem, IResourceSystem, RESOURCESYSTEM_INTERFACE_VERSION, g_resourceSystem );

/*
==================
CResourceSystem::CResourceSystem
==================
*/
CResourceSystem::CResourceSystem()
	: frameNumber( 0 )
{
}

/*
==================
CResourceSystem::Connect
==================
*/
bool CResourceSystem::Connect( createInterfaceFn_t pFactory )
{
	PROFILER_SCOPE_FUNC();
	if ( !ConnectTier1( pFactory ) )
	{
		return false;
	}
	LinkCmds();
	ConVar_Register();

	g_pResourceSystem = this;
	return true;
}

/*
==================
CResourceSystem::Disconnect
==================
*/
void CResourceSystem::Disconnect()
{
	PROFILER_SCOPE_FUNC();
	UnlinkCmds();
	ConVar_Unregister();
	DisconnectTier1();
	g_pResourceSystem = NULL;
}

/*
==================
CResourceSystem::Shutdown
==================
*/
void CResourceSystem::Shutdown()
{
	PROFILER_SCOPE_FUNC();
	frameNumber = 0;
	resourceTypeMgrDict.clear();
}

/*
==================
CResourceSystem::InstallResourceManager
==================
*/
IResourceTypeMgr* CResourceSystem::InstallResourceManager( resourceType_t type )
{
	Assert( type != RESOURCE_TYPE_NONE );
	auto it = resourceTypeMgrDict.find( type );
	if ( it != resourceTypeMgrDict.end() )
	{
		return &it->second;
	}

	it = resourceTypeMgrDict.insert( eastl::make_pair( type, CResourceTypeMgr( type ) ) ).first;
	Msg( "ResourceSystem: Resource manager for type 0x%X installed", type );
	return &it->second;
}

/*
==================
CResourceSystem::RemoveResourceManager
==================
*/
void CResourceSystem::RemoveResourceManager( resourceType_t type )
{
	Assert( type != RESOURCE_TYPE_NONE );
	resourceTypeMgrDict.erase( type );
	Msg( "ResourceSystem: Resource manager for type 0x%X removed", type );
}

/*
==================
CResourceSystem::FrameUpdate
==================
*/
void CResourceSystem::FrameUpdate()
{
	PROFILER_SCOPE_FUNC();
	for ( auto it = resourceTypeMgrDict.begin(), itEnd = resourceTypeMgrDict.end(); it != itEnd; ++it )
	{
		it->second.FrameUpdate();
	}
	++frameNumber;
}

/*
==================
CResourceSystem::UncacheAllResources
==================
*/
void CResourceSystem::UncacheAllResources()
{
	PROFILER_SCOPE_FUNC();
	for ( auto it = resourceTypeMgrDict.begin(), itEnd = resourceTypeMgrDict.end(); it != itEnd; ++it )
	{
		it->second.UncacheAllResources();
	}
}

/*
==================
CResourceSystem::HasResourceManager
==================
*/
bool CResourceSystem::HasResourceManager( resourceType_t type ) const
{
	Assert( type != RESOURCE_TYPE_NONE );
	return resourceTypeMgrDict.find( type ) != resourceTypeMgrDict.end();
}

/*
==================
CResourceSystem::GetResourceManager
==================
*/
IResourceTypeMgr* CResourceSystem::GetResourceManager( resourceType_t type ) const
{
	Assert( type != RESOURCE_TYPE_NONE );
	auto it = resourceTypeMgrDict.find( type );
	return it != resourceTypeMgrDict.end() ? (IResourceTypeMgr*)&it->second : NULL;
}
