#include "pch_resourcesystem.h"
#include "stdlib/filetools.h"
#include "stdlib/convar.h"
#include "resourcesystem/resource.h"
#include "resourcesystem/resourcesystem.h"
#include "resourcesystem/iresourcefactory.h"

CResourceSystem		g_ResourceSystem;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CResourceSystem, IResourceSystem, RESOURCESYSTEM_INTERFACE_VERSION, g_ResourceSystem );


/*
==================
CResourceSystem::Connect
==================
*/
bool CResourceSystem::Connect( createInterfaceFn_t pFactory )
{
	// Connect StdLib
	if ( !ConnectStdLib( pFactory ) )
	{
		return false;
	}

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
	DisconnectStdLib();
	g_pResourceSystem = NULL;
}

/*
==================
CResourceSystem::Init
==================
*/
bool CResourceSystem::Init()
{
	// Register cvars
	ConVar_Register();

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
		std::unordered_map<std::string, TRefPtr<CResource>>&	resourcesDict = resourcesDicts[resourceTypeIdx];
		AssertMsg( resourcesDict.empty(), "Before shutdown of the resource system all resource factories must be unregistered" );
	}

	// Reset all resource factories
	Mem_Memzero( pResourceFactories, RESOURCE_NUM_TYPES * sizeof( IResourceFactory* ) );

	// Unregister cvars
	ConVar_Unregister();
}

/*
==================
CResourceSystem::RegisterResourceFactory
==================
*/
void CResourceSystem::RegisterResourceFactory( resourceType_t type, IResourceFactory* pFactory )
{
	// Before to register the factory make sure that it is valid,
	// because one can depends on specific format types
	Assert( type < RESOURCE_NUM_TYPES && pFactory );
	pFactory->Validate();
	pResourceFactories[type] = pFactory;
	Msg( "ResourceSystem: Resource factory for type 0x%X registered (format: '%s')", type, pFactory->GetFormatType() );
}

/*
==================
CResourceSystem::UnRegisterResourceFactory
==================
*/
void CResourceSystem::UnRegisterResourceFactory( resourceType_t type )
{
	Assert( type < RESOURCE_NUM_TYPES );
	if ( IResourceFactory* pResourceFactory = pResourceFactories[type] )
	{
		// Unload all resource of the type
		std::unordered_map<std::string, TRefPtr<CResource>>&	resourcesDict = resourcesDicts[type];
		for ( auto it = resourcesDict.begin(); it != resourcesDict.end(); ++it )
		{
			pResourceFactory->UnloadResource( it->second->GetData() );
			Msg( "ResourceSystem: Removed resource '%s' (type: 0x%X)", it->first.c_str(), type );
		}
		resourcesDict.clear();

		Msg( "ResourceSystem: Resource factory for type 0x%X unregistered (format: '%s')", type, pResourceFactory->GetFormatType() );
		pResourceFactories[type] = NULL;
	}

	// Make sure that rest factories are valid, because one factory can depends on specific format types
	for ( uint32 factoryIdx = 0; factoryIdx < RESOURCE_NUM_TYPES; ++factoryIdx )
	{
		if ( IResourceFactory* pResourceFactory = pResourceFactories[factoryIdx] )
		{
			pResourceFactory->Validate();
		}
	}
}

/*
==================
CResourceSystem::CreateProceduralResource
==================
*/
TRefPtr<IResource> CResourceSystem::CreateProceduralResource( const achar* pName, resourceType_t type )
{
	PROFILE_SCOPE();
	Assert( type < RESOURCE_NUM_TYPES );

	IResourceFactory*		pResourceFactory = pResourceFactories[type];
	if ( !pResourceFactory )
	{
		Warning( "ResourceSystem: Failed to create procedural resource '%s'. Resource factory for type 0x%X isn't registered", pName, type );
		return NULL;
	}

	std::string		resourceName = pName;
	S_Strlwr( resourceName.data() );

	TRefPtr<CResource>		pResource = new CResource( "", pResourceFactory->CreateProceduralResource(), type, true );
	resourcesDicts[type][resourceName] = pResource;
	Msg( "ResourceSystem: Created procedural resource '%s' (type: 0x%X)", pName, type );
	return pResource;
}

/*
==================
CResourceSystem::FindOrLoadResource
==================
*/
TRefPtr<IResource> CResourceSystem::FindOrLoadResource( const achar* pPath, resourceType_t type, uint32 loadFlags /* = RESOURCE_LOAD_FLAG_NONE */ )
{
	PROFILE_SCOPE();

	// Get a resource name in lower case
	std::string		resourceName;
	S_GetFileBaseName( pPath, resourceName, false );
	S_Strlwr( resourceName.data() );

	// Try find already loaded texture
	Assert( type < RESOURCE_NUM_TYPES );
	auto	it = resourcesDicts[type].find( resourceName );
	if ( it != resourcesDicts[type].end() )
	{
		return it->second;
	}

	// Make sure that we have a factory for the resource type
	IResourceFactory*	pResourceFactory = pResourceFactories[type];
	if ( !pResourceFactory )
	{
		Warning( "ResourceSystem: Failed to load resource '%s'. Resource factory for type 0x%X isn't registered", pPath, type );
		return NULL;
	}

	// Do nothing if we won't load a new resource
	TRefPtr<IResource>		pDefaultResource = !( loadFlags & RESOURCE_LOAD_FLAG_WITHOUT_DEFAULT ) ? pResourceFactory->GetDefaultResource() : NULL;
	if ( loadFlags & RESOURCE_LOAD_FLAG_ONLY_FIND )
	{
		// In the case we return a default resource or NULL
		Error( "ResourceSystem: Failed to find resource '%s' (type: 0x%X)", pPath, type );
		return pDefaultResource;
	}

	TRefPtr<IRefCounted>	pData = pResourceFactory->LoadResource( pPath, loadFlags );
	if ( !pData )
	{
		Error( "ResourceSystem: Failed to load resource '%s' (type: 0x%X)", pPath, type );
		return pDefaultResource;
	}
	
	TRefPtr<CResource>		pResource = new CResource( pPath, pData, type );
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
	PROFILE_SCOPE();

	// TODO BS yehor.pohuliaka - Maybe need rework it like in CStudioAPIDescriptorPoolsMgrVk::FreeUnusedPoolSets
	// Remove unused resources from the system
	for ( uint32 resourceTypeIdx = 0; resourceTypeIdx < RESOURCE_NUM_TYPES; ++resourceTypeIdx )
	{
		IResourceFactory*										pResourceFactory = pResourceFactories[resourceTypeIdx];
		IResource*												pDefaultResource = pResourceFactory->GetDefaultResource();
		std::unordered_map<std::string, TRefPtr<CResource>>&	resourcesDict = resourcesDicts[resourceTypeIdx];
		for ( auto it = resourcesDict.begin(); it != resourcesDict.end(); )
		{
			// Remove a resource if it hasn't more then 1 reference count and it isn't a default resource
			CResource*		pResource = it->second;
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
TRefPtr<IResource> CResourceSystem::GetDefaultResource( resourceType_t type ) const
{
	Assert( type < RESOURCE_NUM_TYPES );
	IResourceFactory*		pResourceFactory = pResourceFactories[type];
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
	IResourceFactory*		pResourceFactory = pResourceFactories[type];
	return pResourceFactory ? pResourceFactory : NULL;
}