#include "pch_resourcesystem.h"
#include "resourcesystem/resourcetypemgr.h"

/*
==================
CResourceTypeMgr::CResourceTypeMgr
==================
*/
CResourceTypeMgr::CResourceTypeMgr( resourceType_t resourceType )
	: resourceType( resourceType )
	, pResourceTypeFactory( NULL )
{
}

/*
==================
CResourceTypeMgr::~CResourceTypeMgr
==================
*/
CResourceTypeMgr::~CResourceTypeMgr()
{
	UncacheAllResources();
}

/*
==================
CResourceTypeMgr::RegisterResourceFactory
==================
*/
void CResourceTypeMgr::RegisterResourceFactory( IResourceTypeFactory* pResourceTypeFactory )
{
	PROFILER_SCOPE_FUNC();
	Assert( pResourceTypeFactory );
	if ( CResourceTypeMgr::pResourceTypeFactory )
	{
		UnregisterResourceFactory();
	}

	CResourceTypeMgr::pResourceTypeFactory = pResourceTypeFactory;
	Msg( "ResourceSystem: Resource factory for type 0x%X registered (ptr: %p)", resourceType, pResourceTypeFactory );
}

/*
==================
CResourceTypeMgr::UnregisterResourceFactory
==================
*/
void CResourceTypeMgr::UnregisterResourceFactory()
{
	PROFILER_SCOPE_FUNC();
	if ( !pResourceTypeFactory )
	{
		return;
	}

	UncacheAllResources();
	Msg( "ResourceSystem: Resource factory for type 0x%X unregistered (ptr: %p)", resourceType, pResourceTypeFactory );
	pResourceTypeFactory = NULL;
}

/*
==================
CResourceTypeMgr::RegisterResourceLoader
==================
*/
void CResourceTypeMgr::RegisterResourceLoader( IResourceTypeLoader* pResourceTypeLoader )
{
	PROFILER_SCOPE_FUNC();
	Assert( pResourceTypeLoader );
	bool bAlreadyRegistered = false;
	for ( uint32 index = 0, count = (uint32)resourceTypeLoaders.size(); index < count; ++index )
	{
		if ( resourceTypeLoaders[index] == pResourceTypeLoader )
		{
			bAlreadyRegistered = true;
			break;
		}
	}

	if ( !bAlreadyRegistered )
	{
		Msg( "ResourceSystem: Resource loader for type 0x%X registered (format: '%s')", resourceType, pResourceTypeLoader->GetFormatName() );
		resourceTypeLoaders.emplace_back( pResourceTypeLoader );
	}
}

/*
==================
CResourceTypeMgr::UnregisterResourceLoader
==================
*/
void CResourceTypeMgr::UnregisterResourceLoader( IResourceTypeLoader* pResourceTypeLoader )
{
	PROFILER_SCOPE_FUNC();
	Assert( pResourceTypeLoader );
	for ( uint32 index = 0, count = (uint32)resourceTypeLoaders.size(); index < count; ++index )
	{
		if ( resourceTypeLoaders[index] == pResourceTypeLoader )
		{
			IResourceTypeLoader* pResourceTypeLoader = resourceTypeLoaders[index];
			resourceTypeLoaders.erase( resourceTypeLoaders.begin() + index );
			Msg( "ResourceSystem: Resource loader for type 0x%X unregistered (format: '%s')", resourceType, pResourceTypeLoader->GetFormatName() );
			break;
		}
	}
}

/*
==================
CResourceTypeMgr::UnregisterAllResourceLoaders
==================
*/
void CResourceTypeMgr::UnregisterAllResourceLoaders()
{
	resourceTypeLoaders.clear();
	Msg( "ResourceSystem:: All resource loaders for type 0x%X unregistered", resourceType );
}

/*
==================
CResourceTypeMgr::CreateResource
==================
*/
CRefPtr<IResource> CResourceTypeMgr::CreateResource( const char* pName, uint8 flags /* = RESOURCE_FLAG_NONE */ )
{
	// Try to find already exists the resource (only if we won't an anonymous resource)
	PROFILER_SCOPE_FUNC();
	bool			   bRequestedAnonymous = flags & RESOURCE_FLAG_ANONYMOUS;
	CRefPtr<CResource> pResource		   = !bRequestedAnonymous ? FindResource( pName, false ) : NULL;
	if ( pResource )
	{
		return pResource;
	}

	// Make sure that we have a factory for the resource type
	if ( !Ensure( pResourceTypeFactory ) )
	{
		Warning( "ResourceSystem: Failed to create resource '%s'. Resource factory for type 0x%X isn't registered", pName, resourceType );
		return NULL;
	}

	// Strip forbidden flags
	const uint8 forbiddenFlags = RESOURCE_FLAG_DEFAULT;
	if ( flags & forbiddenFlags )
	{
		flags &= ~forbiddenFlags;
		Warning( "ResourceSystem: In resource '%s' stripped forbidden flags (forbiddenFlags: 0x%X)", pName, forbiddenFlags );
	}

	// Create a resource
	pResource = new CResource( this, pName, resourceType, RESOURCE_FLAG_PERMANENT | flags );
	pResource->ChangeData( "", pResourceTypeFactory->Create() );
	pResource->MarkUsed();
	Msg( "ResourceSystem: Created resource '%s' (type: 0x%X)", pName, resourceType );

	// Add into the manager if it need
	if ( !bRequestedAnonymous )
	{
		AddResource( pResource );
	}

	// We are done!
	return pResource;
}

/*
==================
CResourceTypeMgr::LoadResource
==================
*/
CRefPtr<IResource> CResourceTypeMgr::LoadResource( const char* pName, const char* pPath, uint8 flags /* = RESOURCE_FLAG_NONE */ )
{
	// Try to find already exists the resource (only if we won't an anonymous resource)
	PROFILER_SCOPE_FUNC();
	bool			   bRequestedAnonymous = flags & RESOURCE_FLAG_ANONYMOUS;
	CRefPtr<CResource> pResource		   = !bRequestedAnonymous ? FindResource( pName, false ) : NULL;
	if ( pResource )
	{
		return pResource;
	}

	// Make sure that we have a factory for the resource type
	if ( !Ensure( pResourceTypeFactory ) )
	{
		Warning( "ResourceSystem: Failed to load resource '%s' as '%s'. Resource factory for type 0x%X isn't registered", pPath, pName, resourceType );
		return NULL;
	}

	// Make sure that we have at least one a resource loader
	if ( !Ensure( !resourceTypeLoaders.empty() ) )
	{
		Warning( "ResourceSystem: Failed to load resource '%s' as '%s'. There are no resource loaders for type 0x%X", pPath, pName, resourceType );
		return NULL;
	}

	// Try to load a resource from a file
	bool				 bHasBeenLoaded		 = false;
	IResourceTypeLoader* pResourceTypeLoader = NULL;
	void*				 pData				 = pResourceTypeFactory->Create();
	for ( uint32 index = 0, count = (uint32)resourceTypeLoaders.size(); index < count && !bHasBeenLoaded; ++index )
	{
		pResourceTypeLoader = resourceTypeLoaders[index];
		bHasBeenLoaded		= pResourceTypeLoader->Load( pPath, pData );
	}

	// Delete the resource data if failed to load the resource
	if ( !bHasBeenLoaded )
	{
		Error( "ResourceSystem: Failed to load resource '%s' as '%s' (type: 0x%X)", pPath, pName, resourceType );
		pResourceTypeFactory->Delete( pData );
		return pDefaultResource;
	}
	Msg( "ResourceSystem: Loaded resource '%s' as '%s' (type: 0x%X, format: '%s')", pPath, pName, resourceType, pResourceTypeLoader->GetFormatName() );

	// Strip forbidden flags
	const uint8 forbiddenFlags = RESOURCE_FLAG_DEFAULT;
	if ( flags & forbiddenFlags )
	{
		flags &= ~forbiddenFlags;
		Warning( "ResourceSystem: In resource '%s' stripped forbidden flags (forbiddenFlags: 0x%X)", pName, forbiddenFlags );
	}

	// Create a resource
	pResource = new CResource( this, pName, resourceType, flags );
	pResource->ChangeData( pPath, pData );
	pResource->MarkUsed();

	// Add into the manager if it need
	if ( !bRequestedAnonymous )
	{
		AddResource( pResource );
	}

	// We are done!
	return pResource;
}

/*
==================
CResourceTypeMgr::LoadResource
==================
*/
bool CResourceTypeMgr::LoadResource( IResource* pResource, const char* pPath ) const
{
	// Make sure that we have a factory for the resource type
	PROFILER_SCOPE_FUNC();
	Assert( pResource );
	CResource*	pResourceLocal = (CResource*)pResource;
	const char* pName		   = pResourceLocal->GetName();
	if ( !Ensure( pResourceTypeFactory ) )
	{
		Warning( "ResourceSystem: Failed to load resource '%s' as '%s'. Resource factory for type 0x%X isn't registered", pPath, pName, resourceType );
		return false;
	}

	// Make sure that we have at least one a resource loader
	if ( !Ensure( !resourceTypeLoaders.empty() ) )
	{
		Warning( "ResourceSystem: Failed to load resource '%s' as '%s'. There are no resource loaders for type 0x%X", pPath, pName, resourceType );
		return false;
	}

	// Try to load a resource from a file
	bool				 bHasBeenLoaded		 = false;
	IResourceTypeLoader* pResourceTypeLoader = NULL;
	void*				 pData				 = pResourceTypeFactory->Create();
	for ( uint32 index = 0, count = (uint32)resourceTypeLoaders.size(); index < count && !bHasBeenLoaded; ++index )
	{
		pResourceTypeLoader = resourceTypeLoaders[index];
		bHasBeenLoaded		= pResourceTypeLoader->Load( pPath, pData );
	}

	// Delete the resource data if failed to load the resource
	if ( !bHasBeenLoaded )
	{
		Error( "ResourceSystem: Failed to load resource '%s' as '%s' (type: 0x%X)", pPath, pName, resourceType );
		pResourceTypeFactory->Delete( pData );
		return false;
	}
	Msg( "ResourceSystem: Loaded resource '%s' as '%s' (type: 0x%X, format: '%s')", pPath, pName, resourceType, pResourceTypeLoader->GetFormatName() );

	// Change a data in the resource
	pResourceLocal->ChangeData( pPath, pData );
	pResourceLocal->MarkUsed();
	return true;
}

/*
==================
CResourceTypeMgr::FindResource
==================
*/
CRefPtr<IResource> CResourceTypeMgr::FindResource( const char* pName, bool bDefaultResourceIfNotFound /* = true */ ) const
{
	PROFILER_SCOPE_FUNC();
	auto it = resourcesDict.find( pName );
	if ( it != resourcesDict.end() )
	{
		return it->second;
	}
	return bDefaultResourceIfNotFound ? pDefaultResource : NULL;
}

/*
==================
CResourceTypeMgr::AddResource
==================
*/
void CResourceTypeMgr::AddResource( IResource* pResource )
{
	// Make sure that the resource is valid and has right a resource type
	PROFILER_SCOPE_FUNC();
	Assert( pResource );
	if ( !Ensure( pResource->GetType() == resourceType ) )
	{
		Error( "ResourceSystem: Resource '%s' wasn't added to the manager due to a resource type mismatch (resourceType: 0x%X, mgrType: 0x%X)", pResource->GetName(), pResource->GetType(), resourceType );
		return;
	}

	// Do nothing if the resource already is tracked by the manager
	if ( !pResource->HasAnyFlags( RESOURCE_FLAG_ANONYMOUS ) )
	{
		return;
	}

	// Add the resource into the manager
	CResource*	pResourceLocal = (CResource*)pResource;
	const char* pName		   = pResourceLocal->GetName();
	resourcesDict[pName]	   = pResourceLocal;
	pResourceLocal->RemoveFlags( RESOURCE_FLAG_ANONYMOUS );
	Msg( "ResourceSystem: Resource '%s' added to the manager (type: 0x%X)", pName, resourceType );
}

/*
==================
CResourceTypeMgr::RemoveResource
==================
*/
void CResourceTypeMgr::RemoveResource( IResource* pResource )
{
	// Make sure that the resource is valid and has right a resource type
	PROFILER_SCOPE_FUNC();
	Assert( pResource );
	if ( !Ensure( pResource->GetType() == resourceType ) )
	{
		Error( "ResourceSystem: Resource '%s' wasn't removed from the manager due to a resource type mismatch (resourceType: 0x%X, mgrType: 0x%X)", pResource->GetName(), pResource->GetType(), resourceType );
		return;
	}

	// Do nothing if the resource already isn't tracked by the manager
	if ( pResource->HasAnyFlags( RESOURCE_FLAG_ANONYMOUS ) )
	{
		return;
	}

	// Remove the resource from the manager
	CResource*	pResourceLocal = (CResource*)pResource;
	const char* pName		   = pResourceLocal->GetName();
	resourcesDict.erase( pName );
	pResourceLocal->AddFlags( RESOURCE_FLAG_ANONYMOUS );
	Msg( "ResourceSystem: Resource '%s' removed from the manager (type: 0x%X)", pName, resourceType );
}

/*
==================
CResourceTypeMgr::CacheResource
==================
*/
bool CResourceTypeMgr::CacheResource( CResource* pResource )
{
	// Do nothing if the resource already cached
	PROFILER_SCOPE_FUNC();
	Assert( pResource );
	Assert( pResource->GetType() == resourceType );
	if ( pResource->HasAnyFlags( RESOURCE_FLAG_CACHED ) )
	{
		return true;
	}

	// Make sure that we have a factory for the resource type
	const char* pName = pResource->GetName();
	const char* pPath = pResource->GetPath();
	if ( !Ensure( pResourceTypeFactory ) )
	{
		Warning( "ResourceSystem: Failed to cache resource '%s'. Resource factory for type 0x%X isn't registered", pName, resourceType );
		return false;
	}

	// Make sure that we have at least one a resource loader
	if ( !Ensure( !resourceTypeLoaders.empty() ) )
	{
		Warning( "ResourceSystem: Failed to cache resource '%s'. There are no resource loaders for type 0x%X", pName, resourceType );
		return false;
	}

	// Try to load a resource from a file
	bool				 bHasBeenLoaded		 = false;
	IResourceTypeLoader* pResourceTypeLoader = NULL;
	void*				 pData				 = pResourceTypeFactory->Create();
	for ( uint32 index = 0, count = (uint32)resourceTypeLoaders.size(); index < count && !bHasBeenLoaded; ++index )
	{
		pResourceTypeLoader = resourceTypeLoaders[index];
		bHasBeenLoaded		= pResourceTypeLoader->Load( pPath, pData );
	}

	// Delete the resource data if failed to load the resource
	if ( !bHasBeenLoaded )
	{
		Error( "ResourceSystem: Failed to cache resource '%s' (type: 0x%X, path: '%s')", pName, resourceType, pPath );
		pResourceTypeFactory->Delete( pData );
		return false;
	}
	Msg( "ResourceSystem: Cached resource '%s' (type: 0x%X, path: '%s', format: '%s')", pName, resourceType, pPath, pResourceTypeLoader->GetFormatName() );

	// Change a data in the resource
	pResource->ChangeData( pPath, pData );
	pResource->MarkUsed();
	return true;
}

/*
==================
CResourceTypeMgr::UncacheResource
==================
*/
void CResourceTypeMgr::UncacheResource( CResource* pResource )
{
	// Do nothing if the resource has permanent flag or it already uncached
	PROFILER_SCOPE_FUNC();
	Assert( pResource );
	Assert( pResource->GetType() == resourceType );
	if ( pResource->HasAnyFlags( RESOURCE_FLAG_PERMANENT ) || !pResource->HasAnyFlags( RESOURCE_FLAG_CACHED ) )
	{
		return;
	}

	// Make sure that we have a factory for the resource type
	const char* pName = pResource->GetName();
	if ( !Ensure( pResourceTypeFactory ) )
	{
		Warning( "ResourceSystem: Failed to uncache resource '%s'. Resource factory for type 0x%X isn't registered", pName, resourceType );
		return;
	}

	// Delete the resource data
	pResource->RemoveFlags( RESOURCE_FLAG_CACHED );
	pResourceTypeFactory->Delete( pResource->pData );
	pResource->pData = NULL;
	Msg( "ResourceSystem: Uncached resource '%s' (type: 0x%X)", pName, resourceType );
}

/*
==================
CResourceTypeMgr::UncacheAllResources
==================
*/
void CResourceTypeMgr::UncacheAllResources()
{
	PROFILER_SCOPE_FUNC();
}

/*
==================
CResourceTypeMgr::FrameUpdate
==================
*/
void CResourceTypeMgr::FrameUpdate()
{
	PROFILER_SCOPE_FUNC();
}

/*
==================
CResourceTypeMgr::SetDefaultResource
==================
*/
void CResourceTypeMgr::SetDefaultResource( IResource* pResource )
{
	if ( pDefaultResource )
	{
		pDefaultResource->RemoveFlags( RESOURCE_FLAG_DEFAULT );
	}
	pDefaultResource = (CResource*)pResource;
	if ( pDefaultResource )
	{
		pDefaultResource->AddFlags( RESOURCE_FLAG_DEFAULT );
	}
}

/*
==================
CResourceTypeMgr::GetDefaultResource
==================
*/
CRefPtr<IResource> CResourceTypeMgr::GetDefaultResource() const
{
	return pDefaultResource;
}

/*
==================
CResourceTypeMgr::GetResourceType
==================
*/
resourceType_t CResourceTypeMgr::GetResourceType() const
{
	return resourceType;
}
