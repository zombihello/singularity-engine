#include "pch_resourcesystem.h"
#include "tier1/filetools.h"
#include "tier1/convar.h"
#include "resourcesystem/resource.h"
#include "resourcesystem/resourcesystem.h"
#include "resourcesystem/iresourcefactory.h"

CResourceSystem g_ResourceSystem;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CResourceSystem, IResourceSystem, RESOURCESYSTEM_INTERFACE_VERSION, g_ResourceSystem );

/*
==================
CResourceSystem::Connect
==================
*/
bool CResourceSystem::Connect( createInterfaceFn_t pFactory )
{
	// Connect Tier1
	if ( !ConnectTier1( pFactory ) )
	{
		return false;
	}
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
	ConVar_Unregister();
	DisconnectTier1();
	g_pResourceSystem = NULL;
}

/*
==================
CResourceSystem::Init
==================
*/
bool CResourceSystem::Init()
{
	// Reset all resource factories
	Mem_Memzero( pResourceFactories, RESOURCE_NUM_TYPES * sizeof( IResourceFactory* ) );
	return true;
}

/*
==================
CResourceSystem::Shutdown
==================
*/
void CResourceSystem::Shutdown()
{
	// Remove all resources
	for ( uint32 resourceTypeIdx = 0; resourceTypeIdx < RESOURCE_NUM_TYPES; ++resourceTypeIdx )
	{
		eastl::unordered_map<eastl::string, CRefPtr<CResource>>& resourcesDict = resourcesDicts[resourceTypeIdx];
		AssertMsg( resourcesDict.empty(), "Before shutdown of the resource system all resource factories must be unregistered" );
	}

	// Reset all resource factories
	Mem_Memzero( pResourceFactories, RESOURCE_NUM_TYPES * sizeof( IResourceFactory* ) );
}

/*
==================
CResourceSystem::RegisterResourceFactory
==================
*/
bool CResourceSystem::RegisterResourceFactory( resourceType_t type, IResourceFactory* pFactory )
{
	// If a factory is already registered for this type,
	// then in this case we can redefine it only in cases where it
	// does not have RESOURCE_FACTORY_FLAG_STATIC or has RESOURCE_FACTORY_FLAG_NOT_USED
	Assert( type < RESOURCE_NUM_TYPES && pFactory );
	IResourceFactory* pOldFactory = pResourceFactories[type];
	if ( pOldFactory && pOldFactory->GetFlags() & RESOURCE_FACTORY_FLAG_STATIC && !( pOldFactory->GetFlags() & RESOURCE_FACTORY_FLAG_NOT_USED ) )
	{
		Warning( "ResourceSystem: Resource factory for type 0x%X already registered and can be override (old format: '%s', new format: '%s')", type, pOldFactory->GetFormatType(), pFactory->GetFormatType() );
		return false;
	}

	pResourceFactories[type] = pFactory;
	Msg( "ResourceSystem: Resource factory for type 0x%X registered (format: '%s')", type, pFactory->GetFormatType() );
	return true;
}

/*
==================
CResourceSystem::UnRegisterResourceFactory
==================
*/
bool CResourceSystem::UnRegisterResourceFactory( resourceType_t type )
{
	Assert( type < RESOURCE_NUM_TYPES );
	if ( IResourceFactory* pResourceFactory = pResourceFactories[type] )
	{
		// We can't unregister a static resource factory that in use now
		if ( pResourceFactory->GetFlags() & RESOURCE_FACTORY_FLAG_STATIC && !( pResourceFactory->GetFlags() & RESOURCE_FACTORY_FLAG_NOT_USED ) )
		{
			Warning( "ResourceSystem: Resource factory for type 0x%X can be unregistered (format: '%s')", type, pResourceFactory->GetFormatType() );
			return false;
		}

		// Unload all resource of the type
		eastl::unordered_map<eastl::string, CRefPtr<CResource>>& resourcesDict = resourcesDicts[type];
		for ( auto it = resourcesDict.begin(); it != resourcesDict.end(); ++it )
		{
			pResourceFactory->UnloadResource( it->second->GetData() );
			Msg( "ResourceSystem: Removed resource '%s' (type: 0x%X)", it->first.c_str(), type );
		}
		resourcesDict.clear();

		Msg( "ResourceSystem: Resource factory for type 0x%X unregistered (format: '%s')", type, pResourceFactory->GetFormatType() );
		pResourceFactories[type] = NULL;
	}

	return true;
}

/*
==================
CResourceSystem::CreateProceduralResource
==================
*/
CRefPtr<IResource> CResourceSystem::CreateProceduralResource( const char* pName, resourceType_t type )
{
	PROFILER_SCOPE_FUNC();
	Assert( type < RESOURCE_NUM_TYPES );

	IResourceFactory* pResourceFactory = pResourceFactories[type];
	if ( !pResourceFactory )
	{
		Warning( "ResourceSystem: Failed to create procedural resource '%s'. Resource factory for type 0x%X isn't registered", pName, type );
		return NULL;
	}

	eastl::string resourceName = pName;
	S_Strlwr( resourceName.data() );

	CRefPtr<CResource> pResource	   = new CResource( "", pResourceFactory->CreateProceduralResource(), type, true );
	resourcesDicts[type][resourceName] = pResource;
	Msg( "ResourceSystem: Created procedural resource '%s' (type: 0x%X)", pName, type );
	return pResource;
}

/*
==================
CResourceSystem::FindOrLoadResource
==================
*/
CRefPtr<IResource> CResourceSystem::FindOrLoadResource( const char* pPath, resourceType_t type, uint32 loadFlags /* = RESOURCE_LOAD_FLAG_NONE */ )
{
	PROFILER_SCOPE_FUNC();

	// Get a resource name in lower case
	eastl::string resourceName;
	S_GetFileBaseName( pPath, resourceName, false );
	S_Strlwr( resourceName.data() );

	// Try find already loaded texture
	Assert( type < RESOURCE_NUM_TYPES );
	auto it = resourcesDicts[type].find( resourceName );
	if ( it != resourcesDicts[type].end() )
	{
		return it->second;
	}

	// Make sure that we have a factory for the resource type
	IResourceFactory* pResourceFactory = pResourceFactories[type];
	if ( !pResourceFactory )
	{
		Warning( "ResourceSystem: Failed to load resource '%s'. Resource factory for type 0x%X isn't registered", pPath, type );
		return NULL;
	}

	// Do nothing if we won't load a new resource
	CRefPtr<IResource> pDefaultResource = !( loadFlags & RESOURCE_LOAD_FLAG_WITHOUT_DEFAULT ) ? pResourceFactory->GetDefaultResource() : NULL;
	if ( loadFlags & RESOURCE_LOAD_FLAG_ONLY_FIND )
	{
		// In the case we return a default resource or NULL
		Error( "ResourceSystem: Failed to find resource '%s' (type: 0x%X)", pPath, type );
		return pDefaultResource;
	}

	CRefPtr<IRefCounted> pData = pResourceFactory->LoadResource( pPath, loadFlags );
	if ( !pData )
	{
		Error( "ResourceSystem: Failed to load resource '%s' (type: 0x%X)", pPath, type );
		return pDefaultResource;
	}

	CRefPtr<CResource> pResource	   = new CResource( pPath, pData, type );
	resourcesDicts[type][resourceName] = pResource;
	Msg( "ResourceSystem: Loaded resource '%s' (type: 0x%X)", pPath, type );
	return pResource;
}

/*
==================
CResourceSystem::RemoveUnusedResources
==================
*/
void CResourceSystem::RemoveUnusedResources()
{
	PROFILER_SCOPE_FUNC();

	// TODO BS yehor.pohuliaka - Maybe need rework it like in CStudioAPIDescriptorPoolsMgrVk::FreeUnusedPoolSets
	// Remove unused resources from the system
	for ( uint32 resourceTypeIdx = 0; resourceTypeIdx < RESOURCE_NUM_TYPES; ++resourceTypeIdx )
	{
		IResourceFactory*									 pResourceFactory = pResourceFactories[resourceTypeIdx];
		IResource*											 pDefaultResource = pResourceFactory->GetDefaultResource();
		eastl::unordered_map<eastl::string, CRefPtr<CResource>>& resourcesDict	  = resourcesDicts[resourceTypeIdx];
		for ( auto it = resourcesDict.begin(); it != resourcesDict.end(); )
		{
			// Remove a resource if it hasn't more then 1 reference count and it isn't a default resource
			CResource* pResource = it->second;
			if ( pResource != pDefaultResource && pResource->GetRefCount() <= 1 )
			{
				Msg( "ResourceSystem: Removed resource '%s' (type: 0x%X)", it->first.c_str(), resourceTypeIdx );
				pResourceFactory->UnloadResource( pResource->GetData() );
				it = resourcesDict.erase( it );
				continue;
			}

			// Otherwise go to the next resource
			++it;
		}
	}
}

/*
==================
CResourceSystem::GetDefaultResource
==================
*/
CRefPtr<IResource> CResourceSystem::GetDefaultResource( resourceType_t type ) const
{
	Assert( type < RESOURCE_NUM_TYPES );
	IResourceFactory* pResourceFactory = pResourceFactories[type];
	return pResourceFactory ? pResourceFactory->GetDefaultResource() : NULL;
}

/*
==================
CResourceSystem::HasResourceFactory
==================
*/
bool CResourceSystem::HasResourceFactory( resourceType_t type ) const
{
	Assert( type < RESOURCE_NUM_TYPES );
	return !!pResourceFactories[type];
}

/*
==================
CResourceSystem::GetResourceFactory
==================
*/
IResourceFactory* CResourceSystem::GetResourceFactory( resourceType_t type ) const
{
	Assert( type < RESOURCE_NUM_TYPES );
	IResourceFactory* pResourceFactory = pResourceFactories[type];
	return pResourceFactory ? pResourceFactory : NULL;
}
