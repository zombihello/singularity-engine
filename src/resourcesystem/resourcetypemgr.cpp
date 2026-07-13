#include "pch_resourcesystem.h"
#include "resourcesystem/resourcetypemgr.h"
#include "resourcesystem/resourcesystem.h"
#include "resourcesystem/cvars.h"

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
	// Uncache all resources and mark they as anonymous
	UncacheAllResources();
	for ( auto it = resourcesDict.begin(), itEnd = resourcesDict.end(); it != itEnd; ++it )
	{
		CResource* pResource = it->second;
		pResource->AddFlags( RESOURCE_FLAG_ANONYMOUS );
		Msg( "ResourceSystem: Resource '%s' removed from the manager (type: 0x%X)", pResource->GetName(), resourceType );
	}
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
	PROFILER_SCOPE_FUNC();

	// Try to find already exists the resource (only if we won't an anonymous resource)
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
	pResource->ChangeData( "", pResourceTypeFactory->Create( pResource ) );
	Msg( "ResourceSystem: Created resource '%s' (type: 0x%X)", pName, resourceType );

	// Add into the manager if it need
	if ( !bRequestedAnonymous )
	{
		AddResource( pResource );
	}

	// We are done!
	MarkUsedResource( pResource );
	return pResource;
}

/*
==================
CResourceTypeMgr::LoadResource
==================
*/
CRefPtr<IResource> CResourceTypeMgr::LoadResource( const char* pName, const char* pPath, uint8 flags /* = RESOURCE_FLAG_NONE */ )
{
	PROFILER_SCOPE_FUNC();

	// Try to find already exists the resource (only if we won't an anonymous resource)
	bool			   bRequestedAnonymous = flags & RESOURCE_FLAG_ANONYMOUS;
	CRefPtr<CResource> pResource		   = !bRequestedAnonymous ? FindResource( pName, false ) : NULL;
	if ( pResource )
	{
		// Cache the resource if it has been uncached
		if ( !pResource->HasAnyFlags( RESOURCE_FLAG_CACHED ) && !CacheResource( pResource ) && !LoadResource( pResource, pPath ) )
		{
			Warning( "ResourceSystem: Resource '%s' found, but failed to cache it or load a new one from '%s' (type: 0x%X)", pName, pPath, resourceType );
		}

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

	// Create a new resource and try to load it
	pResource = new CResource( this, pName, resourceType, flags );

	// Try to load a resource from a file
	bool				 bHasBeenLoaded		 = false;
	IResourceTypeLoader* pResourceTypeLoader = NULL;
	IResourceData*		 pData				 = pResourceTypeFactory->Create( pResource );
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

	// Update resource data in the resource
	pResource->ChangeData( pPath, pData );

	// Add into the manager if it need
	if ( !bRequestedAnonymous )
	{
		AddResource( pResource );
	}

	// We are done!
	MarkUsedResource( pResource );
	return pResource;
}

/*
==================
CResourceTypeMgr::LoadResource
==================
*/
bool CResourceTypeMgr::LoadResource( IResource* pResource, const char* pPath ) const
{
	PROFILER_SCOPE_FUNC();

	// Make sure that we have a factory for the resource type
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
	IResourceData*		 pData				 = pResourceTypeFactory->Create( pResource );
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
	const_cast<CResourceTypeMgr*>( this )->MarkUsedResource( pResourceLocal );
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
	PROFILER_SCOPE_FUNC();

	// Make sure that the resource is valid and has right a resource type
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
	PROFILER_SCOPE_FUNC();

	// Make sure that the resource is valid and has right a resource type
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
	CRefPtr<CResource> pResourceLocal = (CResource*)pResource;
	const char*		   pName		  = pResourceLocal->GetName();
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
	PROFILER_SCOPE_FUNC();

	// Do nothing if the resource already cached
	Assert( pResource && pResource->GetType() == resourceType );
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
	IResourceData*		 pData				 = pResourceTypeFactory->Create( pResource );
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
	pResource->pData = pData;
	pResource->AddFlags( RESOURCE_FLAG_CACHED );

	// Rebuild the dependency list and place permanent holds on it if needed
	pResource->RebuildDependencies();
	MarkUsedResource( pResource );

	// We are done
	pResource->onCached.Invoke( pResource );
	return true;
}

/*
==================
CResourceTypeMgr::UncacheResource
==================
*/
void CResourceTypeMgr::UncacheResource( CResource* pResource, bool bIgnorePermanent /* = false */ )
{
	PROFILER_SCOPE_FUNC();

	// Do nothing if the resource has permanent flag or it already uncached
	Assert( pResource && pResource->GetType() == resourceType );
	if ( !pResource->HasAnyFlags( RESOURCE_FLAG_CACHED ) || ( !bIgnorePermanent && pResource->HasAnyFlags( RESOURCE_FLAG_PERMANENT ) ) )
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

	// Remove cached flag
	pResource->RemoveFlags( RESOURCE_FLAG_CACHED );

	// Delete the resource data
	pResourceTypeFactory->Delete( pResource->pData );
	pResource->pData = NULL;

	// Rebuild the (now empty) dependency list, releasing any permanent holds
	// this resource had placed on its former dependencies
	pResource->RebuildDependencies();

	pResource->onUncached.Invoke( pResource );
	Msg( "ResourceSystem: Uncached resource '%s' (type: 0x%X)", pName, resourceType );
}

/*
==================
CResourceTypeMgr::MarkUsedResource
==================
*/
void CResourceTypeMgr::MarkUsedResource( CResource* pResource )
{
	PROFILER_SCOPE_FUNC();

	// Do nothing if the resource already marked as pending to mark used
	Assert( pResource && pResource->GetType() == resourceType );
	if ( pResource->bPendingMarkUsed.load( eastl::memory_order_relaxed )
		 || !pResource->HasAnyFlags( RESOURCE_FLAG_CACHED )
		 || pResource->HasAnyFlags( RESOURCE_FLAG_PERMANENT | RESOURCE_FLAG_ANONYMOUS | RESOURCE_FLAG_DEFAULT )
		 || pResource->HasPermanentHolders() )
	{
		return;
	}

	// Add the resource into the pending list
	{
		CScopeLock scopeLock( pendingMarkUsedResourcesMutex );
		pendingMarkUsedResourcesWriteList.emplace_back( pResource );
		pResource->bPendingMarkUsed.store( true, eastl::memory_order_release );
	}

	// Mark all dependent resources as used
	pResource->MarkUsedDependencies();
}

/*
==================
CResourceTypeMgr::LinkResourceToLruTail
==================
*/
void CResourceTypeMgr::LinkResourceToLruTail( CResource* pResource )
{
	PROFILER_SCOPE_FUNC();

	// Do nothing if the resource already in the lru list
	Assert( pResource && pResource->GetType() == resourceType );
	if ( pResource->bInLruList )
	{
		return;
	}

	// Add the resource into the lru list
	lruResourcesList.emplace_back( pResource );
	pResource->lruIt	  = --lruResourcesList.cend();
	pResource->bInLruList = true;
}

/*
==================
CResourceTypeMgr::MoveResourceToLruTail
==================
*/
void CResourceTypeMgr::MoveResourceToLruTail( CResource* pResource )
{
	PROFILER_SCOPE_FUNC();

	// Do nothing if the resource isn't in the lru list
	Assert( pResource && pResource->GetType() == resourceType );
	if ( !pResource->bInLruList )
	{
		return;
	}

	// Move the resource to the end of the lru list
	lruResourcesList.splice( lruResourcesList.end(), lruResourcesList, pResource->lruIt );
}

/*
==================
CResourceTypeMgr::UnlinkResourceFromLru
==================
*/
void CResourceTypeMgr::UnlinkResourceFromLru( CResource* pResource )
{
	PROFILER_SCOPE_FUNC();

	// Do nothing if the resource isn't in the lru list
	Assert( pResource && pResource->GetType() == resourceType );
	if ( !pResource->bInLruList )
	{
		return;
	}

	// Unlink the resource from the lru list
	lruResourcesList.erase( pResource->lruIt );
	pResource->bInLruList = false;
}

/*
==================
CResourceTypeMgr::UncacheAllResources
==================
*/
void CResourceTypeMgr::UncacheAllResources()
{
	PROFILER_SCOPE_FUNC();

	// Before uncache all resources we have to update the lru list
	ProcessPendingMarkUsedResources();

	// Do nothing if the lru list is empty
	if ( lruResourcesList.empty() )
	{
		return;
	}

	// Uncache all resources
	for ( auto it = lruResourcesList.begin(), itEnd = lruResourcesList.end(); it != itEnd; ++it )
	{
		// Skip a resource if it isn't valid
		CResource* pResource = *it;
		if ( !pResource )
		{
			continue;
		}

		// Mark that the resource isn't in the lru list
		pResource->bInLruList = false;

		// Uncache and remove the resource only if it isn't permanent, anonymous and default
		if ( !pResource->HasAnyFlags( RESOURCE_FLAG_PERMANENT | RESOURCE_FLAG_ANONYMOUS | RESOURCE_FLAG_DEFAULT ) )
		{
			// Uncache the resource
			UncacheResource( pResource );

			// Remove the resource from the manager if it last reference
			if ( pResource->GetRefCount() <= 1 )
			{
				RemoveResource( pResource );
			}
		}
	}

	// Clear the lru list
	lruResourcesList.clear();
}

/*
==================
CResourceTypeMgr::ProcessPendingMarkUsedResources
==================
*/
void CResourceTypeMgr::ProcessPendingMarkUsedResources()
{
	PROFILER_SCOPE_FUNC();

	// Swap write and read lists
	{
		CScopeLock scopeLock( pendingMarkUsedResourcesMutex );
		S_Swap( pendingMarkUsedResourcesWriteList, pendingMarkUsedResourcesReadList );
	}

	// Do nothing if the read list is empty
	if ( pendingMarkUsedResourcesReadList.empty() )
	{
		return;
	}

	// Mark as used each resource in the read list
	uint64 frameNumber = g_resourceSystem.GetFrameNumber();
	for ( auto it = pendingMarkUsedResourcesReadList.begin(), itEnd = pendingMarkUsedResourcesReadList.end(); it != itEnd; ++it )
	{
		// Skip the resource if it is invalid
		CResource* pResource = *it;
		if ( !pResource )
		{
			continue;
		}
		pResource->bPendingMarkUsed.store( false, eastl::memory_order_release );

		// Skip the resource if it has some flags
		if ( !pResource->HasAnyFlags( RESOURCE_FLAG_CACHED )
			 || pResource->HasAnyFlags( RESOURCE_FLAG_PERMANENT | RESOURCE_FLAG_ANONYMOUS | RESOURCE_FLAG_DEFAULT )
			 || pResource->HasPermanentHolders() )
		{
			continue;
		}

		// Update the lru list
		pResource->lastUsedFrame = frameNumber;
		if ( pResource->bInLruList )
		{
			MoveResourceToLruTail( pResource );
		}
		else
		{
			LinkResourceToLruTail( pResource );
		}
	}

	// Clear the read list
	pendingMarkUsedResourcesReadList.clear();
}

/*
==================
CResourceTypeMgr::ProcessLruResources
==================
*/
void CResourceTypeMgr::ProcessLruResources()
{
	PROFILER_SCOPE_FUNC();

	// Do nothing if the lru list is empty
	if ( lruResourcesList.empty() )
	{
		return;
	}

	// Uncache all unused resources
	uint64		 frameNumber	   = g_resourceSystem.GetFrameNumber();
	const uint64 expireFrameNumber = frameNumber - S_Min<uint64>( frameNumber, res_unusedFrameThreshold.GetInt() );
	while ( !lruResourcesList.empty() )
	{
		// Remove a resource if it isn't valid
		CResource* pResource = lruResourcesList.front();
		if ( !pResource )
		{
			lruResourcesList.pop_front();
			continue;
		}

		// Since the lru list is sorted by "recency", then as soon as a fresh resource is found,
		// you don't need to look further
		if ( pResource->lastUsedFrame >= expireFrameNumber )
		{
			break;
		}

		// Unlink the resource from the lru list
		UnlinkResourceFromLru( pResource );

		// Uncache and remove the resource only if it isn't permanent (or held), anonymous and default
		if ( !pResource->HasAnyFlags( RESOURCE_FLAG_PERMANENT | RESOURCE_FLAG_ANONYMOUS | RESOURCE_FLAG_DEFAULT ) && !pResource->HasPermanentHolders() )
		{
			// Uncache the resource
			UncacheResource( pResource );

			// Remove the resource from the manager if it last reference
			if ( pResource->GetRefCount() <= 1 )
			{
				RemoveResource( pResource );
			}
		}
	}
}

/*
==================
CResourceTypeMgr::FrameUpdate
==================
*/
void CResourceTypeMgr::FrameUpdate()
{
	PROFILER_SCOPE_FUNC();
	ProcessPendingMarkUsedResources();
	ProcessLruResources();
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
		pDefaultResource->MarkUsed();
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
